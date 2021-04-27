#include "EventManager.hpp"
#include "Filesystem.hpp"

#include <event2/buffer.h>

#include <inja/inja.hpp>
#include <iostream>

static void example_inja_render(struct evhttp_request *const req,
                                EvUserData ud) {
  (void)ud;

  inja::Environment env;
  nlohmann::json data;
  data["name"] = "Peter";
  data["city"] = "Brunswick";
  data["age"] = 29;
  data["names"] = {"Jeff", "Seb"};
  data["brother"]["name"] = "Chris";
  data["brother"]["daughters"] = {"Maria", "Helen"};
  data["brother"]["daughter0"] = {{"name", "Maria"}};
  data["is_happy"] = true;
  data["is_sad"] = false;
  data["relatives"]["mother"] = "Maria";
  data["relatives"]["brother"] = "Chris";
  data["relatives"]["sister"] = "Jenny";
  data["vars"] = {2, 3, 4, 0, -1, -2, -3};

  auto reply = env.render(
      "<html><body>\n"
      "Hello {{ name }}! I come from {{ city }}.<br>\n"
      "Hello {{ names.1 }}!<br>\n"
      "Hello {{ brother.name }}!<br>\n"
      "Hello {{ brother.daughter0.name }}!<br>\n"
      "{{ \"{{ no_value }}\" }}<br>\n"
      "Hello{# This is a comment #}!<br>\n"
      "{# --- #Todo --- #}<br>\n"
      "{% for name in names %}a{% endfor %}<br>\n"
      "Hello {% for name in names %}{{ name }} {% endfor %}!<br>\n"
      "Hello {% for name in names %}{{ loop.index }}: {{ name }}, {% "
      "endfor %}!<br>\n"
      "{% for type, name in relatives %}{{ type }}: {{ name }}, {% endfor "
      "%}<br>\n"
      "{% for v in vars %}{% if v > 0 %}+{% endif %}{% endfor %}<br>\n"
      "{% for name in names %}{{ loop.index }}: {{ name }}{% if not "
      "loop.is_last %}, {% endif %}{% endfor %}!<br>\n"
      "{% for name in names %}{{ loop.index }}: {{ name }}{% if "
      "loop.is_last == false %}, {% endif %}{% endfor %}!<br>\n"
      "{% for name in names %}a{% endfor %}<br>\n"
      "</body></html>\n",
      data);

  evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type",
                    "text/html");

  struct evbuffer *const output = evbuffer_new();
  if (output != nullptr) {
    evbuffer_add(output, reply.c_str(), reply.size());
    evhttp_send_reply(req, 200, "OK", output);
    evbuffer_free(output);
  }
}

int main(int argc, char **argv) {
  char const *host = "127.0.0.1";
  uint16_t port = 9000;

  if (argc <= 1) {
    std::cout << "usage: cpp-web [HOST] [PORT] [WWWROOTs..]" << std::endl;
  }

  if (argc > 1) {
    host = argv[1];
  }
  if (argc > 2) {
    port = atoi(argv[2]);
  }

  Filesystem fs;
  for (auto i = 3; i < argc; ++i) {
    if (fs.Scan(argv[i]) != true) {
      return 1;
    }
  }
  fs.Scan();

  EventManager evmgr;
  evmgr.setDefaultCallback(example_inja_render, {});
  evmgr.Init(host, port);
}
