#include "Static.hpp"

Static::Static(std::string uriBasePath, std::shared_ptr<Filesystem> const & fs)
    : Content(), m_UriBasePath(uriBasePath), m_MainTemplatePath(""), m_StaticFiles(fs)
{
    for (auto const & file : fs->GetFiles())
    {
        m_Redirections.push_back(uriBasePath + "/" + file.first);
        m_UriToFsMapping[uriBasePath + "/" + file.first] = file.first;
    }
}

bool Static::Init()
{
    std::cout << "Static files: " << m_StaticFiles->GetFiles().size() << std::endl;

    return true;
}

void Static::Shutdown()
{
    std::cout << "Static files module shutdown" << std::endl;
}

bool Static::Render(RequestResponse & rr, RenderData & rd, std::string & out)
{
    (void)rd;

    rr.UseOutputHeader();
    auto & files = m_StaticFiles->GetFiles();
    auto const & path = std::string(rr.GetUriPath());

    if (rr.AddOutputHeader("Content-Type", files[m_UriToFsMapping[path]].mime) == false)
    {
        return false;
    }

    out = std::string(files[m_UriToFsMapping[path]].data.begin(), files[m_UriToFsMapping[path]].data.end());

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
