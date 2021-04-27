#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H 1

#include "Content.hpp"

class ContentManager {
public:
  ContentManager() {}
  ~ContentManager() {}

  bool Register(Content ctnt);
};

#endif
