#ifndef BLOG_H
#define BLOG_H 1

#include "../../Content.hpp"

class Blog : public TemplatedContent
{
public:
  explicit Blog(std::string baseUri, std::string templatePath);

  bool Init(void);
  void Shutdown(void);
  bool Render(RequestResponse & rr, std::string & out);

  std::string const & GetBaseUri() const;
  Redirections const & GetRedirections() const;

private:
  std::string m_BaseUri;
  Redirections m_Redirections;
};

#endif
