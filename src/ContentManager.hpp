#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H 1

#include "Content.hpp"

#include <memory>
#include <unordered_map>

typedef std::unordered_map<std::string, std::shared_ptr<Content>> ContentModules;

class ContentManager {
public:
  ContentManager() {}
  ~ContentManager() { ShutdownAll(); }

  bool RegisterModule(std::shared_ptr<Content> ctnt);
  bool InitAll(void);
  void ShutdownAll(void);
  bool Render(std::string & basePath);
  ContentModules const & GetAllModules() const;

private:
  ContentModules m_ContentModules;
};

#endif
