#include "Blog.hpp"

Blog::Blog(std::string basePath) : m_BasePath(basePath), m_Redirections()
{
    m_Redirections.push_back(basePath + "-data");
}

bool Blog::Init(void)
{
  return true;
}

void Blog::Shutdown(void)
{
}

bool Blog::Render(std::string & out)
{
  out = "blog-bla";

  return true;
}

std::string const & Blog::GetBasePath(void) const
{
  return m_BasePath;
}

Redirections const &
Blog::GetRedirections(void) const
{
  return m_Redirections;
}
