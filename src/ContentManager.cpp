#include "ContentManager.hpp"

void ContentManager::SetTemplateSystem(std::shared_ptr<TemplateManager> const & tmgr)
{
    m_TemplateManager = tmgr;
}

bool ContentManager::RegisterModule(std::shared_ptr<Content> ctnt)
{
    std::string & basePath = ctnt->GetUriBasePath();

    ContentManager::RemoveGarbageSlashes(basePath);
    if (basePath.empty() == true)
    {
        return false;
    }
#if 0
    std::cout << "Base URI: " << basePath << std::endl;
#endif
    m_ContentModules[basePath] = ctnt;

    return true;
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

        Redirections & rs = content.second->GetRedirections();
        for (auto & redirect : rs)
        {
            ContentManager::RemoveGarbageSlashes(redirect);
            if (redirect == content.second->GetUriBasePath())
            {
                continue;
            }
            if (m_ContentModulesRoutes.find(redirect) != m_ContentModulesRoutes.end())
            {
                std::cerr << "Redirect URI already exists: " << redirect << std::endl;
            }
            else
            {
#if 0
                std::cout << "Redirect URI: " << redirect << std::endl;
#endif
            }
            m_ContentModulesRoutes[redirect] = content.second;
        }

        std::string & basePath = content.second->GetUriBasePath();
        if (basePath.back() == '/')
        {
            basePath.pop_back();
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

    auto & main = cntm->GetMainTemplate();

    out.clear();
    RenderData rd;
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

void ContentManager::RemoveGarbageSlashes(std::string & uri_basepath)
{
    size_t start_pos = 0;
    static const std::string from = "//";
    static const std::string to = "/";

    if (from.empty())
        return;
    while ((start_pos = uri_basepath.find(from, start_pos)) != std::string::npos)
    {
        uri_basepath.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
