#include "Static.hpp"

Static::Static(std::string uriBasePath, std::shared_ptr<Filesystem> const & fs)
    : Content(), m_UriBasePath(uriBasePath), m_MainTemplatePath(""), m_StaticFiles(fs)
{
    for (auto const & file : fs->GetFiles())
    {
        m_Redirections.push_back(uriBasePath + "/" + file.first);
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

    if (rr.GetUriPath() == m_UriBasePath)
    {
        return false;
    }

    rr.UseOutputHeader();
    auto & files = m_StaticFiles->GetFiles();
    auto const & path = std::string(rr.GetUriPath()).substr(m_UriBasePath.length() + 1, std::string::npos);

    if (rr.AddOutputHeader("Content-Type", files[path].mime) == false)
    {
        return false;
    }

    out = std::string(files[path].data.begin(), files[path].data.end());

    return true;
}

std::string & Static::GetUriBasePath()
{
    return m_UriBasePath;
}

std::string const & Static::GetMainTemplate() const
{
    return m_MainTemplatePath;
}

Redirections & Static::GetRedirections()
{
    return m_Redirections;
}
