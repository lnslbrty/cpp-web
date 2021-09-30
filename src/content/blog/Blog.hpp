#ifndef BLOG_H
#define BLOG_H 1

#include "../../Content.hpp"

class Blog : public Content
{
public:
    explicit Blog(std::string uriBasePath, std::string mainTemplatePath);

    bool Init();
    void Shutdown();
    bool Render(RequestResponse & rr, RenderData & rd);

    std::string const & GetUriBasePath() const;
    std::string const & GetMainTemplate() const;
    Redirections const & GetRedirections() const;

private:
    std::string m_UriBasePath;
    std::string m_MainTemplatePath;
    Redirections m_Redirections;
};

#endif
