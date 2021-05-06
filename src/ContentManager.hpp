#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H 1

#include "Content.hpp"

#include <map>

class ContentManager {
public:
  ContentManager() {}
  ~ContentManager() {}

  bool Register(Content const & ctnt);

private:
  std::map<std::string, Content const &> m_ContentModules;
};

#endif
