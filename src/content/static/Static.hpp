#ifndef STATIC_H
#define STATIC_H 1

#include "../../Content.hpp"
#include "../../Filesystem.hpp"

class Static : public Content
{
public:
    explicit Static(std::string uriBasePath, std::shared_ptr<Filesystem> const & fs);

    bool Init();
    void Shutdown();
    bool Render(RequestResponse & rr, RenderData & rd, std::string & out);

    std::string & GetUriBasePath();
    std::string const & GetMainTemplate() const;
    Redirections & GetRedirections();

private:
    std::string m_UriBasePath;
    std::string m_MainTemplatePath;
    Redirections m_Redirections;
    std::shared_ptr<Filesystem> const m_StaticFiles;
};

#endif
