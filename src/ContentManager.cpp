#include "ContentManager.hpp"

void ContentManager::SetStaticFilesystem(std::shared_ptr<Filesystem> & static_fs)
{
    m_StaticFilesystem = static_fs;
}

void ContentManager::SetTemplateSystem(std::shared_ptr<TemplateManager> & tmgr)
{
    m_TemplateManager = tmgr;
}

bool ContentManager::RegisterModule(std::shared_ptr<Content> ctnt)
{
    std::string const & basePath = ctnt->GetUriBasePath();
    Redirections const & rs = ctnt->GetRedirections();

    m_ContentModules[basePath] = ctnt;
    for (auto & redirect : rs)
    {
        m_ContentModules[redirect] = ctnt;
    }

    return false;
}

bool ContentManager::InitAll(void)
{
    bool ret = true;

    for (auto & content : m_ContentModules)
    {
        if (content.second->Init() == false)
        {
            ret = false;
        }
    }

    return ret;
}

void ContentManager::ShutdownAll(void)
{
    std::unordered_map<std::shared_ptr<Content>, bool> shutdownModules;

    for (auto & content : m_ContentModules)
    {
        auto const & search = shutdownModules.find(content.second);
        if (search != shutdownModules.end())
        {
            continue;
        }
        else
        {
            content.second->Shutdown();
            shutdownModules[content.second] = true;
        }
    }

    m_ContentModules.clear();
}

bool ContentManager::Render(char const * basePath, RequestResponse & rr, std::string & out)
{
    if (m_ContentModules.find(basePath) == m_ContentModules.end())
    {
        return false;
    }

    RenderData rd;
    auto & cntm = m_ContentModules[basePath];
    auto & main = cntm->GetMainTemplate();

    if (m_ContentModules[basePath]->Render(rr, rd) == false)
    {
        return false;
    }

    if (m_TemplateManager->RenderTemplate(main, rd, out) == false)
    {
        return false;
    }

    return true;
}

ContentModules const & ContentManager::GetAllModules() const
{
    return m_ContentModules;
}
