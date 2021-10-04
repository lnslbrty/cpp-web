#include "ContentManager.hpp"

void ContentManager::SetTemplateSystem(std::shared_ptr<TemplateManager> const & tmgr)
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
        m_ContentModulesRoutes[redirect] = ctnt;
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

    m_ContentModulesRoutes.clear();
    m_ContentModules.clear();
}

bool ContentManager::Render(char const * basePath, RequestResponse & rr, std::string & out)
{
    std::shared_ptr<Content> cntm;

    if (m_ContentModules.find(basePath) == m_ContentModules.end())
    {
        if (m_ContentModulesRoutes.find(basePath) == m_ContentModulesRoutes.end())
        {
            return false;
        }
        else
        {
            cntm = m_ContentModulesRoutes[basePath];
        }
    }
    else
    {
        cntm = m_ContentModules[basePath];
    }

    RenderData rd;
    auto & main = cntm->GetMainTemplate();

    out.clear();
    if (cntm->Render(rr, rd, out) == false)
    {
        return false;
    }

    if (main.empty() == false && out.empty() == true && m_TemplateManager->RenderTemplate(main, rd, out) == false)
    {
        return false;
    }

    return true;
}

ContentModules const & ContentManager::GetAllModules() const
{
    return m_ContentModules;
}

ContentModules const & ContentManager::GetAllModulesRoutes() const
{
    return m_ContentModulesRoutes;
}
