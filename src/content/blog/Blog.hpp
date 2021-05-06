#ifndef BLOG_H
#define BLOG_H 1

#include "../../Content.hpp"

class Blog : public Content
{
public:
  bool Init(std::string & basePath);
  void Shutdown();
  bool Render();
  Redirections const GetRedirections();
};

#endif
