#include "ContentManager.hpp"
#include "EventManager.hpp"
#include "Filesystem.hpp"
#include "TemplateManager.hpp"

#include "content/blog/Blog.hpp"
#include "content/static/Static.hpp"

#include <event2/buffer.h>

#include <inja/inja.hpp>
#include <iostream>

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

    std::shared_ptr<TemplateManager> tmgr = std::make_shared<TemplateManager>();
    std::shared_ptr<ContentManager> ctmgr = std::make_shared<ContentManager>();
    ctmgr->SetTemplateSystem(tmgr);

    {
        std::shared_ptr<Filesystem> static_fs = std::make_shared<Filesystem>();
        if (argc > 3)
        {
            if (static_fs->Scan(argv[3]) != true)
            {
                return 1;
            }
        }
        static_fs->Scan("./wwwroot", {"html", "tmpl"}, true);

        ctmgr->RegisterModule(std::make_shared<Static>("/static", static_fs));
    }

    {
        Filesystem dynamic_fs;
        if (argc > 4)
        {
            if (dynamic_fs.Scan(argv[4]) != true)
            {
                return 1;
            }
        }
        dynamic_fs.Scan("./wwwroot", {"html", "tmpl"}, false);

        tmgr->ParseTemplates(dynamic_fs);
    }

    ctmgr->RegisterModule(std::make_shared<Blog>("/blog", "index.html", "./blog"));

    if (ctmgr->InitAll() == false)
    {
        std::cout << "InitAll() failed." << std::endl;
        return 1;
    }

    EventManager evmgr(ctmgr);
    evmgr.Init(host, port);

    // ctmgr.ShutdownAll();
}
