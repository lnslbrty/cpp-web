#include "Blog.hpp"

Blog::Blog(std::string uriBasePath, std::string mainTemplatePath)
    : Content(), m_UriBasePath(uriBasePath), m_MainTemplatePath(mainTemplatePath)
{
    m_Redirections.push_back(uriBasePath + "/");
    m_Redirections.push_back(uriBasePath + "/index.html");
}

bool Blog::Init()
{
    return true;
}

void Blog::Shutdown()
{
}

bool Blog::Render(RequestResponse & rr, RenderData & rd)
{
    rd["blah"] = "Yoh!";
    rr.UseOutputHeader();
    rr.AddOutputHeader("bla", "blubb");

    return true;
}

std::string const & Blog::GetUriBasePath() const
{
    return m_UriBasePath;
}

std::string const & Blog::GetMainTemplate() const
{
    return m_MainTemplatePath;
}

Redirections const & Blog::GetRedirections() const
{
    return m_Redirections;
}
