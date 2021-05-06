#include "Blog.hpp"

bool Blog::Init(std::string & basePath)
{
  (void)basePath;

  return false;
}

void Blog::Shutdown()
{
}

bool Blog::Render()
{
  return false;
}

Redirections const
Blog::GetRedirections()
{
    return Redirections();
}
