#ifndef CONTENT_H
#define CONTENT_H 1

#include "RequestResponse.hpp"

#include <inja/inja.hpp>
#include <string>
#include <tuple>
#include <vector>

using Redirections = std::vector<std::string>;
using RenderData = std::unordered_map<std::string, std::string>;

class Content
{
public:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

    virtual bool Render(RequestResponse & rr, RenderData & rd, std::string & out) = 0;

    virtual std::string const & GetUriBasePath() const = 0;
    virtual std::string const & GetMainTemplate() const = 0;
    virtual Redirections const & GetRedirections() const = 0;
};

#endif
