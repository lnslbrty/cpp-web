#ifndef CONTENT_H
#define CONTENT_H 1

#include <inja/inja.hpp>
#include <string>
#include <tuple>
#include <vector>

typedef std::vector<std::string> Redirections;

class Content {
public:
  virtual bool Init() = 0;
  virtual void Shutdown() = 0;
  virtual bool Render(std::string & out) = 0;

  virtual std::string const & GetBasePath() const = 0;
  virtual Redirections const & GetRedirections() const = 0;
};

class TemplatedContent : public Content {
public:
  explicit TemplatedContent(std::string filesystemPath);

  virtual bool Init();
  virtual void Shutdown();
  virtual bool Render(std::string & out);

  void SetTemplateData(nlohmann::json & data);

private:
  std::string m_FilesystemPath;
  nlohmann::json m_TemplateData;
};

#endif
