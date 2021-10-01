#ifndef STATIC_H
#define STATIC_H 1

#include "../../Content.hpp"
#include "../../Filesystem.hpp"
#include "../markdown/Markdown.hpp"

class Static : public Content
{
public:
    explicit Static(std::string uriBasePath, std::string staticFilesPath);

    bool Init();
    void Shutdown();
    bool Render(RequestResponse & rr, RenderData & rd, std::string & out);

    std::string const & GetUriBasePath() const;
    std::string const & GetMainTemplate() const;
    Redirections const & GetRedirections() const;

private:
    std::string m_UriBasePath;
    std::string m_MainTemplatePath;
    std::string m_StaticFilesPath;
    Redirections m_Redirections;
    Filesystem m_StaticFiles;
};

#endif
