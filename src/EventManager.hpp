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

typedef void *EvUserData;
typedef void ev_c_callback(struct evhttp_request *, EvUserData);
typedef std::function<void(struct evhttp_request *, EvUserData)> EvFunction;

struct ev_callback {
  EvFunction cb;
  EvUserData ud;
};

typedef std::tuple<std::string, struct ev_callback> EvUrlCallback;

class EventManager {
public:
  EventManager();
  ~EventManager();

  bool Init(std::string = "127.0.0.1", uint16_t port = 9000);
  void SetDefaultCallback(EvFunction fn, EvUserData dat);
  void AddCallback(std::string url, EvFunction fn, EvUserData dat);
  void AddContentManager(ContentManager const & cmgr);

private:
  struct ev_callback m_DefaultCallback;
  std::vector<EvUrlCallback> m_UrlCallbacks;
  ContentModules m_ContentModules;

  struct event_config *m_EvConfig = nullptr;
  struct event_base *m_EvBase = nullptr;
  struct evhttp *m_EvHttp = nullptr;
  struct evhttp_bound_socket *m_EvSocket = nullptr;
  struct evconnlistener *m_EvListener = nullptr;
  struct event *m_EvTermEvent = nullptr;
};

#endif
