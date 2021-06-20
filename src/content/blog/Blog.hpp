#ifndef BLOG_H
#define BLOG_H 1

#include "../../Content.hpp"

class Blog : public Content
{
public:
  explicit Blog(std::string basePath);

  bool Init(void);
  void Shutdown(void);
  bool Render(std::string & out);

  std::string const & GetBasePath() const;
  Redirections const & GetRedirections() const;

private:
  std::string m_BasePath;
  Redirections m_Redirections;
};

#endif
