#include "Blog.hpp"

#include "../../ContentManager.hpp"

#include <filesystem>

Blog::Blog(std::string uriBasePath, std::string blogPath, std::string mainTemplatePath)
    : Content(),
      m_UriBasePath(uriBasePath),
      m_MainTemplatePath(mainTemplatePath),
      m_BlogPath(blogPath),
      m_BlogContents("", blogPath)
{
    m_Redirections.push_back(uriBasePath + "/");
    m_Redirections.push_back(uriBasePath + "/index.html");
}

bool Blog::Init()
{
    bool retval = true;

    std::cout << "Blog entries filesystem path: " << m_BlogPath << std::endl;
    std::cout << "Blog entries URI base path: " << m_UriBasePath << std::endl;
    std::vector<std::string> extensions = {"json"};

    Filesystem fs;
    if (fs.Scan(m_BlogPath, extensions, false) == false)
    {
        return false;
    }

    for (auto const & jfile : fs.GetFiles())
    {
        std::string full_uri_path = m_UriBasePath + "/" + std::string(std::filesystem::path(jfile.first).stem());
        ContentManager::RemoveGarbageSlashes(full_uri_path);

        auto const json_metadata = inja::json::parse(jfile.second.data);
        BlogEntry be = std::make_shared<struct blog_entry>(full_uri_path);
        if (Blog::ValidateAndSetMetdadata(json_metadata, be) == false)
        {
            std::cerr << "Blog Metadata validation failed." << std::endl;
            retval = false;
        }

        m_BlogEntriesSortedByDate.push_back(be);
        m_BlogEntries[full_uri_path] = be;
        m_Redirections.push_back(full_uri_path);
    }
    std::sort(m_BlogEntriesSortedByDate.begin(), m_BlogEntriesSortedByDate.end(), [](auto const & a, auto const & b) {
        return a->publishDate > b->publishDate;
    });

    m_BlogContents.Init(m_UriBasePath);

    if (retval == false)
    {
        return false;
    }

    return ValidateEntries();
}

void Blog::Shutdown()
{
    std::cout << "Blog module shutdown" << std::endl;

    m_BlogContents.Shutdown();
}

bool Blog::Render(RequestResponse & rr, RenderData & rd, std::string & out)
{
    (void)rr;
    (void)rd;
    (void)out;

    if (rr.GetUriPath() == m_UriBasePath || rr.GetUriPath() == m_UriBasePath + "/" ||
        rr.GetUriPath() == m_UriBasePath + "/index.html")
    {
        GenerateBlogListing(rd["blog_listing"]);
    }
    else
    {
        if (GetBlogPost(rd["blog_post"], rr.GetUriPath()) == false)
        {
            return false;
        }
    }

    return true;
}

std::string & Blog::GetUriBasePath()
{
    return m_UriBasePath;
}

std::string const & Blog::GetMainTemplate() const
{
    return m_MainTemplatePath;
}

Redirections & Blog::GetRedirections()
{
    return m_Redirections;
}

bool Blog::ValidateAndSetMetdadata(BlogMetadata const & blogMetadata, BlogEntry & blogEntry)
{
    bool retval = true;
    std::function<bool(BlogMetadata const &, std::string const)> validateMetadata =
        [blogEntry](BlogMetadata const & bm, std::string const tname) {
            if (bm.find(tname) == bm.cend())
            {
                std::cerr << "Metadata validation: JSON key '" << tname << "' missing in "
                          << blogEntry->filename + ".json" << std::endl;
                return false;
            }
            return true;
        };
    std::function<bool(std::string const &, std::time_t &)> parseDateTime = [](std::string const & timeStr,
                                                                               std::time_t & time) {
        std::tm tm = {};
        std::stringstream ss(timeStr);
        ss >> std::get_time(&tm, "%d.%m.%y %H:%M");
        time = std::mktime(&tm);
        if (time <= 0)
        {
            std::cerr << "Metadata validation: Invalid time string '" << timeStr
                      << "', format required: '%d.%m.%y %H:%M'" << std::endl;
            return false;
        }
        return true;
    };

    if (validateMetadata(blogMetadata, "title") == false)
    {
        retval = false;
    }
    blogEntry->title = blogMetadata["title"];

    if (validateMetadata(blogMetadata, "tags") == false)
    {
        retval = false;
    }
    for (auto const & tag : blogMetadata["tags"])
    {
        blogEntry->tags.push_back(tag);
    }

    if (validateMetadata(blogMetadata, "author") == false)
    {
        retval = false;
    }
    blogEntry->author = blogMetadata["author"];

    if (validateMetadata(blogMetadata, "createDate") == false ||
        parseDateTime(blogMetadata["createDate"], blogEntry->createDate) == false)
    {
        retval = false;
    }

    if (validateMetadata(blogMetadata, "publishDate") == false ||
        parseDateTime(blogMetadata["publishDate"], blogEntry->publishDate) == false)
    {
        retval = false;
    }

    if (validateMetadata(blogMetadata, "published") == false)
    {
        retval = false;
    }
    blogEntry->published = blogMetadata["published"];

    if (validateMetadata(blogMetadata, "accessAllowed") == false)
    {
        retval = false;
    }
    blogEntry->accessAllowed = blogMetadata["accessAllowed"];

    return retval;
}

bool Blog::ValidateEntries()
{
    bool retval = true;

    for (auto const & e : m_BlogEntriesSortedByDate)
    {
        if (m_BlogContents.HasMarkdownFile(e->filename) == false)
        {
            std::cerr << "Blog entry metadata " << e->filename << ".json exists, but markdown file " << e->filename
                      << ".md not." << std::endl;
            retval = false;
        }
    }
    for (auto & m : m_BlogContents.GetMarkdowns())
    {
        if (std::any_of(m_BlogEntriesSortedByDate.cbegin(),
                        m_BlogEntriesSortedByDate.cend(),
                        [m](BlogEntry const & be) { return m.first == be->filename; }) == false)
        {
            std::cerr << "Blog entry markdown " << m.first << ".md exists, but metadata " << m.first << ".json not."
                      << std::endl;
            retval = false;
        }
    }

    return retval;
}

void Blog::FillRenderData(RenderData & re, BlogEntry const & be)
{
    re["filename"] = be->filename;
    re["title"] = be->title;
    re["tags"] = be->tags;
    re["author"] = be->author;
    {
        char tmstr[64];
        if (std::strftime(tmstr, sizeof(tmstr), "%c", std::localtime(&be->createDate)) > 0)
        {
            re["createDate"] = tmstr;
        }
        else
        {
            re["createDate"] = "<strftime error>";
        }
    }
    {
        char tmstr[64];
        if (std::strftime(tmstr, sizeof(tmstr), "%c", std::localtime(&be->publishDate)) > 0)
        {
            re["publishDate"] = tmstr;
        }
        else
        {
            re["publishDate"] = "<strftime error>";
        }
    }
    re["published"] = be->published;
    re["accessAllowed"] = be->accessAllowed;
    re["content"] = "";
}

void Blog::GenerateBlogListing(RenderData & rd)
{
    for (auto const & e : m_BlogEntriesSortedByDate)
    {
        if (e->published == false)
        {
            continue;
        }

        RenderData re;
        FillRenderData(re, e);
        if (m_BlogContents.HasMarkdownFile(e->filename) == true)
        {
            re["content"] = m_BlogContents.GetMarkdownHTML(e->filename)->c_str();
        }
        rd += re;
    }
}

bool Blog::GetBlogPost(RenderData & rd, char const * blogPostUri)
{
    RenderData re;

    if (m_BlogContents.HasMarkdownFile(blogPostUri) == true)
    {
        if (m_BlogEntries[blogPostUri]->accessAllowed == false)
        {
            return false;
        }

        FillRenderData(re, m_BlogEntries[blogPostUri]);
        re["content"] = m_BlogContents.GetMarkdownHTML(blogPostUri)->c_str();
    }
    rd = re;

    return true;
}
