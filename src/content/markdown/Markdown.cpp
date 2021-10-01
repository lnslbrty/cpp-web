#include "Markdown.hpp"

Markdown::Markdown(std::string uriBasePath, std::string markdownFilesPath)
    : Content(), m_UriBasePath(uriBasePath), m_MainTemplatePath(""), m_MarkdownFilesPath(markdownFilesPath)
{
}

bool Markdown::Init()
{
    std::cout << "Markdown files path: " << m_MarkdownFilesPath << std::endl;

    std::vector<std::string> extensions = {"md"};

    if (m_MarkdownFiles.Scan(m_MarkdownFilesPath, extensions, false) == false)
    {
        return false;
    }

    return true;
}

void Markdown::Shutdown()
{
    std::cout << "Markdown module shutdown" << std::endl;
}

bool Markdown::Render(RequestResponse & rr, RenderData & rd, std::string & out)
{
    (void)out;

    rd["blub"] = "Yoh21!";
    rr.UseOutputHeader();
    rr.AddOutputHeader("blaaaa", "blubb");

    return true;
}

std::string const & Markdown::GetUriBasePath() const
{
    return m_UriBasePath;
}

std::string const & Markdown::GetMainTemplate() const
{
    return m_MainTemplatePath;
}

Redirections const & Markdown::GetRedirections() const
{
    return m_Redirections;
}
