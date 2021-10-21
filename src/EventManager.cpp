#include "EventManager.hpp"

#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/keyvalq_struct.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <signal.h>

extern "C"
{
    struct chunk_req_state
    {
        struct event_base * base;
        struct evhttp_request * req;
        struct event * timer;

        off_t out_offset;
    };

    static void GenerateInternalErrorPage(struct evhttp_request * const req, std::string text)
    {
        evhttp_clear_headers(evhttp_request_get_output_headers(req));
        evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/html");

        struct evbuffer * const output = evbuffer_new();
        if (output != nullptr)
        {
            evbuffer_add(output, text.c_str(), text.size());
            evhttp_send_reply(req, 500, "Internal Error", output);
            evbuffer_free(output);
        }
    }

    static void EvGenericInterceptor(struct evhttp_request * const req, void * ev_c_callback)
    {
        if (ev_c_callback != nullptr)
        {
            struct ev_callback * const evcb = (struct ev_callback *)ev_c_callback;
            evcb->cb(req, evcb->ud);
        }
        else
        {
            GenerateInternalErrorPage(req, "EvGenericInterceptor: ev_c_callback == nullptr");
        }
    }

    static void EvContentManagerInterceptor(struct evhttp_request * const req, void * ev_c_callback)
    {
        if (ev_c_callback != nullptr)
        {
            struct evhttp_uri const * const uri = evhttp_request_get_evhttp_uri(req);
            if (uri == nullptr)
            {
                GenerateInternalErrorPage(req, "EvContentManagerInterceptor: uri == nullptr");
                return;
            }

            char const * const path = evhttp_uri_get_path(uri);
            if (path == nullptr)
            {
                GenerateInternalErrorPage(req, "EvContentManagerInterceptor: path == nullptr");
                return;
            }
#if 1
            std::cout << "URI Path: " << path << std::endl;
#endif

            std::shared_ptr<ContentManager> const cmgr = *(std::shared_ptr<ContentManager> const *)ev_c_callback;
            RequestResponse rr(path, req);
            std::string out;

            if (cmgr->Render(path, rr, out) == false)
            {
                std::string text;
                text = "ContentModule(\"" + std::string(path) + "\")->Render() failed.\n";
                GenerateInternalErrorPage(req, text);
            }
            else
            {
                struct evbuffer * const output = evbuffer_new();
                if (output != nullptr)
                {
                    evbuffer_add(output, out.c_str(), out.size());
                    evhttp_send_reply(req, HTTP_OK, "OK", output);
                    evbuffer_free(output);
                }
            }
        }
        else
        {
            GenerateInternalErrorPage(req, "EvContentManagerInterceptor: ev_c_callback == nullptr");
        }
    }

    static void do_term(int sig, short events, void * arg)
    {
        (void)events;
        struct event_base * base = (struct event_base *)arg;
        event_base_loopbreak(base);
        fprintf(stderr, "Got %i, Terminating\n", sig);
    }
}

static inline void default_evhttp_callback(struct evhttp_request * const req, EvUserData ud)
{
    (void)ud;

    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/html");

    struct evbuffer * const output = evbuffer_new();
    if (output != nullptr)
    {
        evbuffer_add_printf(output, "%s\n", "<html><body><b>default page</b></body></html>");
        evhttp_send_reply(req, 200, "OK", output);
        evbuffer_free(output);
    }
}

EventManager::EventManager(std::shared_ptr<ContentManager> & cmgr)
    : m_ContentManager(cmgr), m_DefaultCallback({default_evhttp_callback, nullptr})
{
}

EventManager::~EventManager()
{
    if (m_EvConfig != nullptr)
        event_config_free(m_EvConfig);
    if (m_EvHttp != nullptr)
        evhttp_free(m_EvHttp);
    if (m_EvTermEvent != nullptr)
        event_free(m_EvTermEvent);
    if (m_EvBase != nullptr)
        event_base_free(m_EvBase);
}

bool EventManager::Init(std::string host, uint16_t port)
{
    struct event_config * cfg = nullptr;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        return false;
    }

#if 0
    event_enable_debug_logging(EVENT_DBG_ALL);
#endif

    cfg = event_config_new();
    event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);

    m_EvBase = event_base_new_with_config(cfg);
    if (m_EvBase == nullptr)
    {
        fprintf(stderr, "Couldn't create an event_base: exiting\n");
        return false;
    }
    event_config_free(cfg);
    cfg = nullptr;

    /* Create a new evhttp object to handle requests. */
    m_EvHttp = evhttp_new(m_EvBase);
    if (m_EvHttp == nullptr)
    {
        fprintf(stderr, "couldn't create evhttp. Exiting.\n");
        return false;
    }

    for (auto & uc : m_UrlCallbacks)
    {
        struct ev_callback * const evcb = &std::get<1>(uc);
        if (evhttp_set_cb(m_EvHttp, std::get<0>(uc).c_str(), EvGenericInterceptor, evcb) != 0)
        {
            return false;
        }
    }
    for (auto & cm : m_ContentManager->GetAllModules())
    {
        if (evhttp_set_cb(m_EvHttp, cm.first.c_str(), EvContentManagerInterceptor, &m_ContentManager) != 0)
        {
            fprintf(stderr, "Failed to add module callback: %s\n", cm.first.c_str());
        }
    }
    for (auto & cm : m_ContentManager->GetAllModulesRoutes())
    {
        if (evhttp_set_cb(m_EvHttp, cm.first.c_str(), EvContentManagerInterceptor, &m_ContentManager) != 0)
        {
            fprintf(stderr, "Failed to add route callback: %s\n", cm.first.c_str());
        }
    }
    evhttp_set_gencb(m_EvHttp, EvGenericInterceptor, &m_DefaultCallback);

    if (evhttp_bind_socket(m_EvHttp, host.c_str(), port) != 0)
    {
        fprintf(stderr, "Couldn't bind to %s:%d. Exiting.\n", host.c_str(), port);
        return false;
    }

    evhttp_set_allowed_methods(m_EvHttp, EVHTTP_REQ_GET | EVHTTP_REQ_POST | EVHTTP_REQ_HEAD);
    evhttp_set_default_content_type(m_EvHttp, NULL);

    m_EvTermEvent = evsignal_new(m_EvBase, SIGINT, do_term, m_EvBase);
    if (m_EvTermEvent == nullptr)
    {
        return false;
    }
    if (event_add(m_EvTermEvent, NULL) != 0)
    {
        return false;
    }

    event_base_dispatch(m_EvBase);

    return true;
}

void EventManager::SetDefaultCallback(EvFunction fn, EvUserData dat)
{
    m_DefaultCallback.cb = fn;
    m_DefaultCallback.ud = dat;
}

void EventManager::AddCallback(std::string url, EvFunction fn, EvUserData dat)
{
    m_UrlCallbacks.push_back(EvUrlCallback(url, {fn, dat}));
}

void EventManager::AddChunkedCallback(std::string url, EvFunction fn, EvUserData dat, struct timeval chunk_timer)
{
}
