#include "EventManager.hpp"

#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/keyvalq_struct.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <signal.h>

extern "C" {
static void EvCallbackInterceptor(struct evhttp_request *const req,
                                  void *ev_c_callback) {
  if (ev_c_callback != nullptr) {
    struct ev_callback *const evcb = (struct ev_callback *)ev_c_callback;
    evcb->cb(req, evcb->ud);
  }
}

static void do_term(int sig, short events, void *arg) {
  (void)events;
  struct event_base *base = (struct event_base *)arg;
  event_base_loopbreak(base);
  fprintf(stderr, "Got %i, Terminating\n", sig);
}
}

EventManager::~EventManager() {
  if (m_EvConfig != nullptr)
    event_config_free(m_EvConfig);
  if (m_EvHttp != nullptr)
    evhttp_free(m_EvHttp);
  if (m_EvTermEvent != nullptr)
    event_free(m_EvTermEvent);
  if (m_EvBase != nullptr)
    event_base_free(m_EvBase);
}

bool EventManager::Init(std::string host, uint16_t port) {
  struct event_config *cfg = nullptr;

  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    return false;
  }

  event_enable_debug_logging(EVENT_DBG_ALL);

  cfg = event_config_new();
  event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);

  m_EvBase = event_base_new_with_config(cfg);
  if (m_EvBase == nullptr) {
    fprintf(stderr, "Couldn't create an event_base: exiting\n");
    return false;
  }
  event_config_free(cfg);
  cfg = nullptr;

  /* Create a new evhttp object to handle requests. */
  m_EvHttp = evhttp_new(m_EvBase);
  if (m_EvHttp == nullptr) {
    fprintf(stderr, "couldn't create evhttp. Exiting.\n");
    return false;
  }

  for (auto &uc : m_UrlCallbacks) {
    struct ev_callback *const evcb = &std::get<1>(uc);
    if (evhttp_set_cb(m_EvHttp, std::get<0>(uc).c_str(), EvCallbackInterceptor,
                      evcb) != 0) {
      return false;
    }
  }
  evhttp_set_gencb(m_EvHttp, EvCallbackInterceptor, &m_DefaultCallback);

  m_EvSocket = evhttp_bind_socket_with_handle(m_EvHttp, host.c_str(), port);
  if (m_EvSocket == nullptr) {
    fprintf(stderr, "couldn't bind to %s:%d. Exiting.\n", host.c_str(), port);
    return false;
  }

  m_EvTermEvent = evsignal_new(m_EvBase, SIGINT, do_term, m_EvBase);
  if (m_EvTermEvent == nullptr) {
    return false;
  }
  if (event_add(m_EvTermEvent, NULL) != 0) {
    return false;
  }

  event_base_dispatch(m_EvBase);

  return true;
}

void EventManager::setDefaultCallback(EvFunction fn, EvUserData dat) {
  m_DefaultCallback.cb = fn;
  m_DefaultCallback.ud = dat;
}

void EventManager::addCallback(std::string url, EvFunction fn, EvUserData dat) {
  m_UrlCallbacks.push_back(EvUrlCallback(url, {fn, dat}));
}
