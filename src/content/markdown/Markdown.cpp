#include "Markdown.hpp"

Markdown::Markdown(std::string uriBasePath, std::string markdownFilesPath, std::string mainTemplatePath)
    : Content(),
      m_UriBasePath(uriBasePath),
      m_MainTemplatePath(mainTemplatePath),
      m_MarkdownFilesPath(markdownFilesPath)
{
}

bool Markdown::Init()
{
    std::cout << "Markdown files path: " << m_MarkdownFilesPath << std::endl;

    std::vector<std::string> extensions = {"md"};

    Filesystem fs;
    if (fs.Scan(m_MarkdownFilesPath, extensions, false) == false)
    {
        return false;
    }

    for (auto const & mfile : fs.GetFiles())
    {
        m_Markdowns[mfile.first] =
            std::make_shared<std::string>(std::string(mfile.second.data.begin(), mfile.second.data.end()));
    }

    return true;
}

void Markdown::Shutdown()
{
    std::cout << "Markdown module shutdown" << std::endl;
}

bool Markdown::Render(RequestResponse & rr, RenderData & rd, std::string & out)
{
    (void)rr;
    (void)rd;
    (void)out;

    if (m_MainTemplatePath.empty() == true)
    {
        return false;
    }

    return false; /* TODO: Make markdown module usable as standalone module?! */
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

bool Markdown::HasMarkdownFile(std::string filePath) const
{
    return m_Markdowns.find(filePath) != m_Markdowns.end();
}

bool Markdown::HasMarkdownURI(std::string uriPath) const
{
    return HasMarkdownFile(uriPath.substr(m_UriBasePath.length() + 1, std::string::npos));
}

Markdowns const & Markdown::GetMarkdowns() const
{
    return m_Markdowns;
}

std::shared_ptr<std::string> const & Markdown::GetMarkdownHTML(std::string uriPath)
{
    return m_Markdowns[uriPath];
}
