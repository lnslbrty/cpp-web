#ifndef STATIC_H
#define STATIC_H 1

#include "../../Content.hpp"
#include "../../Filesystem.hpp"

class Static : public Content
{
public:
    explicit Static(std::string uriBasePath, std::shared_ptr<Filesystem> const & fs);

    bool Init() override;
    void Shutdown() override;
    bool Render(RequestResponse & rr, RenderData & rd, std::string & out) override;

    std::string & GetUriBasePath() override;
    std::string const & GetMainTemplate() const override;
    Redirections & GetRedirections() override;

private:
    std::string m_UriBasePath;
    std::string m_MainTemplatePath;
    Redirections m_Redirections;
    std::shared_ptr<Filesystem> const m_StaticFiles;
};

#endif
