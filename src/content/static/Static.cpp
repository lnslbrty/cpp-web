#include "Static.hpp"

Static::Static(std::string uriBasePath, std::string staticFilesPath)
    : Content(), m_UriBasePath(uriBasePath), m_MainTemplatePath(""), m_StaticFilesPath(staticFilesPath)
{
}

bool Static::Init()
{
    std::cout << "Static files path: " << m_StaticFilesPath << std::endl;

    std::vector<std::string> extensions = {"json"};

    if (m_StaticFiles.Scan(m_StaticFilesPath, extensions, false) == false)
    {
        return false;
    }

    return true;
}

void Static::Shutdown()
{
    std::cout << "Static files module shutdown" << std::endl;
}

bool Static::Render(RequestResponse & rr, RenderData & rd, std::string & out)
{
    (void)rr;
    (void)rd;
    (void)out;

    rd["blah"] = "Yooooh!";

    return true;
}

std::string const & Static::GetUriBasePath() const
{
    return m_UriBasePath;
}

std::string const & Static::GetMainTemplate() const
{
    return m_MainTemplatePath;
}

Redirections const & Static::GetRedirections() const
{
    return m_Redirections;
}
