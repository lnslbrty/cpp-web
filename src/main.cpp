#include "ContentManager.hpp"
#include "EventManager.hpp"
#include "Filesystem.hpp"
#include "TemplateManager.hpp"

#include "content/blog/Blog.hpp"

#include <event2/buffer.h>

#include <inja/inja.hpp>
#include <iostream>

static void example_inja_render(struct evhttp_request * const req, EvUserData ud)
{
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

    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/html");

    struct evbuffer * const output = evbuffer_new();
    if (output != nullptr)
    {
        evbuffer_add(output, reply.c_str(), reply.size());
        evhttp_send_reply(req, 200, "OK", output);
        evbuffer_free(output);
    }
}

int main(int argc, char ** argv)
{
    char const * host = "127.0.0.1";
    uint16_t port = 9000;

    if (argc <= 1 || argc > 5)
    {
        std::cout << "usage: cpp-web [HOST] [PORT] [STATIC-WWWROOT] [DYNAMIC-WWWROOT]" << std::endl;
        if (argc > 5)
        {
            return 1;
        }
    }

    if (argc > 1)
    {
        host = argv[1];
    }
    if (argc > 2)
    {
        port = atoi(argv[2]);
    }

    std::shared_ptr<Filesystem> static_fs = std::make_shared<Filesystem>();
    if (argc > 3)
    {
        if (static_fs->Scan(argv[3]) != true)
        {
            return 1;
        }
    }
    static_fs->Scan("./wwwroot", {"html", "tmpl"}, true);

    Filesystem dynamic_fs;
    if (argc > 4)
    {
        if (dynamic_fs.Scan(argv[4]) != true)
        {
            return 1;
        }
    }
    dynamic_fs.Scan("./wwwroot", {"html", "tmpl"}, false);

    std::shared_ptr<TemplateManager> tmgr = std::make_shared<TemplateManager>();
    tmgr->ParseTemplates(dynamic_fs);

    std::shared_ptr<ContentManager> ctmgr = std::make_shared<ContentManager>();
    ctmgr->SetStaticFilesystem(static_fs);
    ctmgr->SetTemplateSystem(tmgr);
    ctmgr->RegisterModule(std::make_shared<Blog>("/blog", "index.html"));

    if (ctmgr->InitAll() == false)
    {
        std::cout << "InitAll() failed." << std::endl;
        return 1;
    }

    EventManager evmgr(ctmgr);
    // evmgr.SetDefaultCallback(example_inja_render, {});
    evmgr.AddCallback("/bla", example_inja_render, {});
    evmgr.Init(host, port);

    // ctmgr.ShutdownAll();
}
