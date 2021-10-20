#include "Blog.hpp"

#include <filesystem>

Blog::Blog(std::string uriBasePath, std::string mainTemplatePath, std::string blogPath)
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

    std::cout << "Blog entries path: " << m_BlogPath << std::endl;
    std::vector<std::string> extensions = {"json"};

    Filesystem fs;
    if (fs.Scan(m_BlogPath, extensions, false) == false)
    {
        return false;
    }

    for (auto const & jfile : fs.GetFiles())
    {
        auto const json_metadata = inja::json::parse(jfile.second.data);
        BlogEntry be =
            std::make_shared<struct blog_entry>(jfile.first,
                                                std::string(std::filesystem::path(jfile.first).stem()) + ".md");
        if (Blog::ValidateAndSetMetdadata(json_metadata, be) == false)
        {
            std::cerr << "Blog Metadata validation failed." << std::endl;
            retval = false;
        }
        m_BlogEntriesSortedByDate.push_back(be);

        m_Redirections.push_back(std::filesystem::path(jfile.first).stem());
    }
    std::sort(m_BlogEntriesSortedByDate.begin(),
              m_BlogEntriesSortedByDate.end(),
              [](auto const & a, auto const & b) { return a->publishDate > b->publishDate; });

    m_BlogContents.Init();

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
        rd["blog_content"] = "bla";
    }

    return true;
}

std::string const & Blog::GetUriBasePath() const
{
    return m_UriBasePath;
}

std::string const & Blog::GetMainTemplate() const
{
    return m_MainTemplatePath;
}

Redirections const & Blog::GetRedirections() const
{
    return m_Redirections;
}

bool Blog::ValidateAndSetMetdadata(BlogMetadata const & blogMetadata, BlogEntry & blogEntry)
{
    bool retval = true;
    std::function<bool(BlogMetadata const &, std::string const)> validateMetadata =
        [blogEntry](BlogMetadata const & bm, std::string const tname)
    {
        if (bm.find(tname) == bm.cend())
        {
            std::cerr << "Metadata validation: JSON key '" << tname << "' missing in " << blogEntry->metadata_filename
                      << std::endl;
            return false;
        }
        return true;
    };
    std::function<bool(std::string const &, std::time_t &)> parseDateTime =
        [](std::string const & timeStr, std::time_t & time)
    {
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

    return retval;
}

bool Blog::ValidateEntries() const
{
    bool retval = true;

    for (auto const & e : m_BlogEntriesSortedByDate)
    {
        if (m_BlogContents.HasMarkdownFile(e->content_filename) == false)
        {
            std::cerr << "Blog entry metadata " << e->metadata_filename << " exists, but markdown file "
                      << e->content_filename << " not." << std::endl;
            retval = false;
        }
    }
    for (auto const & m : m_BlogContents.GetMarkdowns())
    {
        if (std::any_of(m_BlogEntriesSortedByDate.cbegin(),
                        m_BlogEntriesSortedByDate.cend(),
                        [m](BlogEntry const & be) { return m.first == be->content_filename; }) == false)
        {
            std::cerr << "Blog entry markdown " << m.first << " exists, but metadata not." << std::endl;
            retval = false;
        }
    }

    return retval;
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
        re["metadata_filename"] = e->metadata_filename;
        re["content_filename"] = e->content_filename;
        re["title"] = e->title;
        re["tags"] = e->tags;
        re["author"] = e->author;
        re["createDate"] = e->createDate;
        re["publishDate"] = e->publishDate;
        re["published"] = e->published;
        if (m_BlogContents.HasMarkdownFile(e->content_filename) == true)
        {
            re["content"] = m_BlogContents.GetMarkdownHTML(e->content_filename)->c_str();
        }
        else
        {
            re["content"] = "";
        }

        rd += re;
    }
}
