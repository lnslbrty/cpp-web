#include "ContentManager.hpp"

bool ContentManager::RegisterModule(std::shared_ptr<Content> ctnt)
{
  std::string const & basePath = ctnt->GetBasePath();
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
    } else {
      content.second->Shutdown();
      shutdownModules[content.second] = true;
    }
  }

  m_ContentModules.clear();
}

bool ContentManager::Render(std::string & basePath)
{
  (void)basePath;

  return false;
}

ContentModules const & ContentManager::GetAllModules() const
{
  return m_ContentModules;
}
