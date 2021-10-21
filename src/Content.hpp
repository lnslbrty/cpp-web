#ifndef CONTENT_H
#define CONTENT_H 1

#include "RequestResponse.hpp"

#include <inja/inja.hpp>
#include <string>
#include <tuple>
#include <vector>

using Redirections = std::vector<std::string>;
using RenderData = inja::json;

class Content
{
public:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

    virtual bool Render(RequestResponse & rr, RenderData & rd, std::string & out) = 0;

    virtual std::string & GetUriBasePath() = 0;
    virtual std::string const & GetMainTemplate() const = 0;
    virtual Redirections & GetRedirections() = 0;
};

#endif
