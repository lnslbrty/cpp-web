#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H 1

#include "ContentManager.hpp"
#include "Filesystem.hpp"

#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/http.h>

#include <functional>
#include <string>
#include <vector>

using EvUserData = void *;
using EvFunction = std::function<void(struct evhttp_request *, EvUserData)>;

struct ev_callback
{
    EvFunction cb;
    EvUserData ud;
};

using EvUrlCallback = std::tuple<std::string, struct ev_callback>;

class EventManager
{
public:
    EventManager(std::shared_ptr<ContentManager> & cmgr);
    ~EventManager();

    bool Init(std::string = "127.0.0.1", uint16_t port = 9000);
    void SetDefaultCallback(EvFunction fn, EvUserData dat);
    void AddCallback(std::string url, EvFunction fn, EvUserData dat);
    void AddChunkedCallback(std::string url, EvFunction fn, EvUserData dat, struct timeval chunk_timer);

private:
    std::shared_ptr<ContentManager> m_ContentManager;
    struct ev_callback m_DefaultCallback;
    std::vector<EvUrlCallback> m_UrlCallbacks;

    struct event_config * m_EvConfig = nullptr;
    struct event_base * m_EvBase = nullptr;
    struct evhttp * m_EvHttp = nullptr;
    struct evhttp_bound_socket * m_EvSocket = nullptr;
    struct evconnlistener * m_EvListener = nullptr;
    struct event * m_EvTermEvent = nullptr;
};

#endif
