#include "RequestResponse.hpp"

RequestResponse::RequestResponse(char const * const uri_path, struct evhttp_request * const req)
    : m_UriPath(uri_path), m_Request(req), m_InputHeader(nullptr), m_OutputHeader(nullptr)
{
}

RequestResponse::~RequestResponse()
{
    evhttp_clear_headers(&m_Query);
}

void RequestResponse::UseInputHeader()
{
    m_InputHeader = evhttp_request_get_input_headers(m_Request);
}

void RequestResponse::UseOutputHeader()
{
    m_OutputHeader = evhttp_request_get_output_headers(m_Request);
}

bool RequestResponse::UseUri()
{
    struct evhttp_uri const * const uri = evhttp_request_get_evhttp_uri(m_Request);
    if (uri == nullptr)
    {
        return false;
    }
    char const * const query = evhttp_uri_get_query(uri);
    evhttp_parse_query_str(query, &m_Query);
    return true;
}

bool RequestResponse::AddOutputHeaderByRef(std::string const & key, std::string const & value)
{
    return evhttp_add_header(m_OutputHeader, key.c_str(), value.c_str()) == 0;
}

bool RequestResponse::AddOutputHeader(std::string const key, std::string const value)
{
    return AddOutputHeaderByRef(key, value);
}

bool RequestResponse::RemoveOutputHeaderByRef(std::string const & key)
{
    return evhttp_remove_header(m_OutputHeader, key.c_str()) == 0;
}

bool RequestResponse::RemoveOutputHeader(std::string const key)
{
    return RemoveOutputHeaderByRef(key);
}

bool RequestResponse::GetInputHeaderByRef(std::string const & key, std::string & value)
{
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

bool RequestResponse::GetQueryValue(std::string & key, std::string & value)
{
    std::string const k = key;
    return GetQueryValue(k, value);
}

bool RequestResponse::GetQueryValue(std::string const key, std::string & value)
{
    char const * const v = evhttp_find_header(&m_Query, key.c_str());

    if (v == nullptr)
    {
        return false;
    }

    value = v;
    return true;
}

bool RequestResponse::QueryValueEquals(std::string & key, std::string value)
{
    std::string const k = key;
    return QueryValueEquals(k, value);
}

bool RequestResponse::QueryValueEquals(std::string const key, std::string value)
{
    char const * const v = evhttp_find_header(&m_Query, key.c_str());

    if (v == nullptr)
    {
        return false;
    }

    return value == std::string(v);
}
