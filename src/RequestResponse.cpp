#include "RequestResponse.hpp"

RequestResponse::RequestResponse(char const * const uri_path, struct evhttp_request * const req)
    : m_UriPath(uri_path), m_Request(req)
{
}

RequestResponse::~RequestResponse()
{
}

void RequestResponse::UseInputHeader()
{
    m_InputHeader = evhttp_request_get_input_headers(m_Request);
}

void RequestResponse::UseOutputHeader()
{
    m_OutputHeader = evhttp_request_get_output_headers(m_Request);
}

bool RequestResponse::AddOutputHeaderByRef(std::string const & key, std::string const & value)
{
    if (m_OutputHeader == nullptr)
    {
        return false;
    }

    return evhttp_add_header(m_OutputHeader, key.c_str(), value.c_str()) == 0;
}

bool RequestResponse::AddOutputHeader(std::string const key, std::string const value)
{
    return AddOutputHeaderByRef(key, value);
}

bool RequestResponse::RemoveOutputHeaderByRef(std::string const & key)
{
    if (m_OutputHeader == nullptr)
    {
        return false;
    }

    return evhttp_remove_header(m_OutputHeader, key.c_str()) == 0;
}

bool RequestResponse::RemoveOutputHeader(std::string const key)
{
    return RemoveOutputHeaderByRef(key);
}

bool RequestResponse::GetInputHeaderByRef(std::string const & key, std::string & value)
{
    if (m_InputHeader == nullptr)
    {
        return false;
    }

    char const * const v = evhttp_find_header(m_InputHeader, key.c_str());

    if (v == nullptr)
    {
        return false;
    }

    value = v;
    return true;
}

bool RequestResponse::GetInputHeader(std::string const key, std::string value)
{
    return GetInputHeaderByRef(key, value);
}
