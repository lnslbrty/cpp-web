#ifndef CONTENT_H
#define CONTENT_H 1

#include "RequestResponse.hpp"

#include <inja/inja.hpp>
#include <string>
#include <tuple>
#include <vector>

typedef std::vector<std::string> Redirections;

class Content {
public:
  virtual bool Init() = 0;
  virtual void Shutdown() = 0;
  virtual bool Render(RequestResponse & rr, std::string & out) = 0;

  virtual std::string const & GetBaseUri() const = 0;
  virtual Redirections const & GetRedirections() const = 0;
  virtual void GetRequiredFiles(std::vector<std::string> & requiredFiles) const = 0;
};

class TemplatedContent : public Content {
public:
  explicit TemplatedContent(std::string mainTemplate);

  virtual bool Init();
  virtual void Shutdown();
  virtual bool Render(RequestResponse & rr, std::string & out);
  virtual void GetRequiredFiles(std::vector<std::string> & requiredFiles) const;

private:
  std::string m_FilesystemPath;
};

#endif
