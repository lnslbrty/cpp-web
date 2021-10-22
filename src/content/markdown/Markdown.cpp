#include "Markdown.hpp"

#include <md4c-html.h>

Markdown::Markdown(std::string uriBasePath, std::string markdownFilesPath, std::string mainTemplatePath)
    : Content(),
      m_UriBasePath(uriBasePath),
      m_MainTemplatePath(mainTemplatePath),
      m_MarkdownFilesPath(markdownFilesPath)
{
    if (m_MainTemplatePath.empty() == false)
    {
        m_Redirections.push_back(uriBasePath + "/");
        m_Redirections.push_back(uriBasePath + "/index.html");
    }
}

extern "C" void markdown_to_html_conversion(const MD_CHAR * const text, MD_SIZE size, void * const userdata)
{
    std::string * html = (std::string *)userdata;

    html->append(text, size);
}

bool Markdown::Init()
{
    std::cout << "Markdown files path: " << m_MarkdownFilesPath << std::endl;
    if (m_UriBasePath.empty() == false)
    {
        std::cout << "Markdown files URI base path: " << m_UriBasePath << std::endl;
    }

    std::vector<std::string> extensions = {"md"};

    Filesystem fs;
    if (fs.Scan(m_MarkdownFilesPath, extensions, false) == false)
    {
        return false;
    }

    for (auto const & mfile : fs.GetFiles())
    {
        Data const & data = mfile.second.data;
        std::string html;

        html.reserve(data.size() / 8 + 64);
        int ret = md_html((MD_CHAR const *)data.data(),
                          data.size(),
                          markdown_to_html_conversion,
                          &html,
                          MD_DIALECT_GITHUB,
                          MD_FLAG_COLLAPSEWHITESPACE | MD_FLAG_PERMISSIVEURLAUTOLINKS | MD_FLAG_PERMISSIVEWWWAUTOLINKS |
                              MD_FLAG_PERMISSIVEEMAILAUTOLINKS | MD_FLAG_PERMISSIVEAUTOLINKS | MD_FLAG_TABLES |
                              MD_FLAG_STRIKETHROUGH | MD_FLAG_LATEXMATHSPANS | MD_FLAG_WIKILINKS | MD_FLAG_TASKLISTS |
                              MD_FLAG_UNDERLINE);

        if (ret != 0)
        {
            std::cerr << "Markdown HTML rendering failed." << std::endl;
            return false;
        }

        m_Markdowns[mfile.first] = std::make_shared<std::string>(html);
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

    std::string md_file;

    if (m_MainTemplatePath.empty() == true)
    {
        return false;
    }

    if (rr.GetUriPath() == m_UriBasePath || rr.GetUriPath() == m_UriBasePath + "/" ||
        rr.GetUriPath() == m_UriBasePath + "/index.html")
    {
        rr.UseUri();
        std::string requested_markdown;

        rd["uri"] = rr.GetUriPath();
        if (rr.GetQueryValue("get", requested_markdown) == true)
        {
            requested_markdown += ".md";
            if (HasMarkdownFile(requested_markdown) == true)
            {
                rd["content"] = *GetMarkdownHTML(requested_markdown);
            } else if (HasMarkdownFile("index.md") == true) {
                rd["content"] = *GetMarkdownHTML("index.md");
            } else {
                return false;
            }
        }
        else if (HasMarkdownFile("index.md") == true)
        {
            rd["content"] = *GetMarkdownHTML("index.md");
        } else {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

std::string & Markdown::GetUriBasePath()
{
    return m_UriBasePath;
}

std::string const & Markdown::GetMainTemplate() const
{
    return m_MainTemplatePath;
}

Redirections & Markdown::GetRedirections()
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
