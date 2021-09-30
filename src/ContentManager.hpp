#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H 1

#include "Content.hpp"
#include "Filesystem.hpp"
#include "TemplateManager.hpp"

#include <memory>
#include <unordered_map>

using ContentModules = std::unordered_map<std::string, std::shared_ptr<Content> >;

class ContentManager
{
public:
    ContentManager()
    {
    }
    ~ContentManager()
    {
        ShutdownAll();
    }

    void SetStaticFilesystem(std::shared_ptr<Filesystem> & static_fs);
    void SetTemplateSystem(std::shared_ptr<TemplateManager> & tmgr);
    bool RegisterModule(std::shared_ptr<Content> ctnt);
    bool InitAll(void);
    void ShutdownAll(void);
    bool Render(char const * basePath, RequestResponse & rr, std::string & out);
    ContentModules const & GetAllModules() const;

private:
    std::shared_ptr<Filesystem> m_StaticFilesystem;
    std::shared_ptr<TemplateManager> m_TemplateManager;
    ContentModules m_ContentModules;
};

#endif
