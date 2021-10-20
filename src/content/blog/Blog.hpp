#ifndef BLOG_H
#define BLOG_H 1

#include <inja/inja.hpp>

#include "../../Content.hpp"
#include "../../Filesystem.hpp"
#include "../markdown/Markdown.hpp"

struct blog_entry
{
    explicit blog_entry(std::string const & metadata_filename, std::string const & content_filename)
        : metadata_filename(metadata_filename), content_filename(content_filename)
    {
    }

    std::string const metadata_filename;
    std::string const content_filename;

    std::string title;
    std::vector<std::string> tags;
    std::string author;
    std::time_t createDate;
    std::time_t publishDate;
    bool published;
};

using BlogMetadata = inja::json;
using BlogEntry = std::shared_ptr<struct blog_entry>;
using BlogEntries = std::vector<BlogEntry>;

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

    static bool ValidateAndSetMetdadata(BlogMetadata const & blogMetadata, BlogEntry & blogEntry);
    bool ValidateEntries() const;
    void GenerateBlogListing(RenderData & rd);

private:
    std::string m_UriBasePath;
    std::string m_MainTemplatePath;
    std::string m_BlogPath;
    Redirections m_Redirections;
    Markdown m_BlogContents;
    BlogEntries m_BlogEntriesSortedByDate;
};

#endif
