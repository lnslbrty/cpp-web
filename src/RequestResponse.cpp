#include "RequestResponse.hpp"

RequestResponse::RequestResponse(struct evhttp_request * const req) : m_Request(req)
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

bool RequestResponse::AddOutputHeader(std::string & key, std::string & value)
{
  return evhttp_add_header(m_OutputHeader, key.c_str(), value.c_str());
}

bool RequestResponse::AddOutputHeader2(std::string key, std::string value)
{
  return AddOutputHeader(key, value);
}

bool RequestResponse::GetInputHeader(std::string & key, std::string & value)
{
  char const * const v = evhttp_find_header(m_InputHeader, key.c_str());

  if (v == nullptr)
  {
    return false;
  }

  value = v;
  return true;
}

bool RequestResponse::GetInputHeader2(std::string key, std::string value)
{
  return GetInputHeader(key, value);
}
