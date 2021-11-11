#ifndef BLOG_H
#define BLOG_H 1

#include <inja/inja.hpp>

#include "../../Content.hpp"
#include "../../Filesystem.hpp"
#include "../markdown/Markdown.hpp"

struct blog_entry
{
    explicit blog_entry(std::string const & filename) : filename(filename)
    {
    }

    std::string const filename;

    std::string title;
    std::vector<std::string> tags;
    std::string author;
    std::time_t createDate;
    std::time_t publishDate;
    bool published;
    bool accessAllowed;
};

using BlogMetadata = inja::json;
using BlogEntry = std::shared_ptr<struct blog_entry>;
using BlogEntriesVector = std::vector<BlogEntry>;
using BlogEntriesMap = std::unordered_map<std::string, BlogEntry>;

class Blog : public Content
{
public:
    explicit Blog(std::string uriBasePath, std::string blogPath, std::string mainTemplatePath);

    bool Init() override;
    void Shutdown() override;
    bool Render(RequestResponse & rr, RenderData & rd, std::string & out) override;

    std::string & GetUriBasePath() override;
    std::string const & GetMainTemplate() const override;
    Redirections & GetRedirections() override;

    static bool ValidateAndSetMetdadata(BlogMetadata const & blogMetadata, BlogEntry & blogEntry);
    bool ValidateEntries();
    void FillRenderData(RenderData & re, BlogEntry const & be);
    void GenerateBlogListing(RenderData & rd);
    bool GetBlogPost(RenderData & rd, char const * blogPostUri);

private:
    std::string m_UriBasePath;
    std::string m_MainTemplatePath;
    std::string m_BlogPath;
    Redirections m_Redirections;
    Markdown m_BlogContents;
    BlogEntriesVector m_BlogEntriesSortedByDate;
    BlogEntriesMap m_BlogEntries;
};

#endif
