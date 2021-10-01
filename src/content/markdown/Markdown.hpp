#ifndef MARKDOWN_H
#define MARKDOWN_H 1

#include "../../Content.hpp"
#include "../../Filesystem.hpp"

class Markdown : public Content
{
public:
    explicit Markdown(std::string uriBasePath, std::string markdownFilesPath);

    bool Init();
    void Shutdown();
    bool Render(RequestResponse & rr, RenderData & rd, std::string & out);

    std::string const & GetUriBasePath() const;
    std::string const & GetMainTemplate() const;
    Redirections const & GetRedirections() const;

private:
    std::string m_UriBasePath;
    std::string m_MainTemplatePath;
    std::string m_MarkdownFilesPath;
    Redirections m_Redirections;
    Filesystem m_MarkdownFiles;
};

#endif
