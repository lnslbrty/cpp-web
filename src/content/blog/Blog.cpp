#include "Blog.hpp"

Blog::Blog(std::string baseUri, std::string templatePath) : TemplatedContent(templatePath), m_BaseUri(baseUri), m_Redirections()
{
    m_Redirections.push_back(baseUri + "-data");
}

bool Blog::Init(void)
{
  return true;
}

void Blog::Shutdown(void)
{
}

bool Blog::Render(RequestResponse & rr, std::string & out)
{
  (void)rr;

  rr.UseOutputHeader();
  rr.AddOutputHeader2("bla", "blubb");
  out = "blog-bla";

  return true;
}

std::string const & Blog::GetBaseUri(void) const
{
  return m_BaseUri;
}

Redirections const &
Blog::GetRedirections(void) const
{
  return m_Redirections;
}
