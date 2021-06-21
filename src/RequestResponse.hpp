#ifndef REQUEST_RESPONSE_H
#define REQUEST_RESPONSE_H 1

#include <event2/http.h>

#include <string>

class RequestResponse {
public:
  RequestResponse(struct evhttp_request * const req);
  ~RequestResponse();

  void UseInputHeader();
  void UseOutputHeader();

  bool AddOutputHeader(std::string & key, std::string & value);
  bool AddOutputHeader2(std::string key, std::string value);

  bool GetInputHeader(std::string & key, std::string & value);
  bool GetInputHeader2(std::string key, std::string value);

private:
  struct evhttp_request * const m_Request;
  struct evkeyvalq * m_InputHeader;
  struct evkeyvalq * m_OutputHeader;
};

#endif
