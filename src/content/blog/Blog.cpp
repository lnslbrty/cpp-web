#include "Blog.hpp"

Blog::Blog(std::string uriBasePath, std::string mainTemplatePath, std::string blogPath)
    : Content(),
      m_UriBasePath(uriBasePath),
      m_MainTemplatePath(mainTemplatePath),
      m_BlogPath(blogPath),
      m_BlogEntries("", blogPath)
{
    m_Redirections.push_back(uriBasePath + "/");
    m_Redirections.push_back(uriBasePath + "/index.html");
}

bool Blog::Init()
{
    std::cout << "Blog entries path: " << m_BlogPath << std::endl;

    std::vector<std::string> extensions = {"json"};

    if (m_BlogEntriesMetadata.Scan(m_BlogPath, extensions, false) == false)
    {
        return false;
    }

    m_BlogEntries.Init();

    return true;
}

void Blog::Shutdown()
{
    std::cout << "Blog module shutdown" << std::endl;

    m_BlogEntries.Shutdown();
}

bool Blog::Render(RequestResponse & rr, RenderData & rd, std::string & out)
{
    (void)rr;
    (void)rd;
    (void)out;

    rd["blah"] = "Yooooh!";

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
