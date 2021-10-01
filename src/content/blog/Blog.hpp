#ifndef BLOG_H
#define BLOG_H 1

#include "../../Content.hpp"
#include "../../Filesystem.hpp"
#include "../markdown/Markdown.hpp"

class Blog : public Content
{
public:
    explicit Blog(std::string uriBasePath, std::string mainTemplatePath, std::string blogPath);

    bool Init();
    void Shutdown();
    bool Render(RequestResponse & rr, RenderData & rd, std::string & out);

    std::string const & GetUriBasePath() const;
    std::string const & GetMainTemplate() const;
    Redirections const & GetRedirections() const;

private:
    std::string m_UriBasePath;
    std::string m_MainTemplatePath;
    std::string m_BlogPath;
    Redirections m_Redirections;
    Filesystem m_BlogEntriesMetadata;
    Markdown m_BlogEntries;
};

#endif
