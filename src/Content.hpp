#ifndef CONTENT_H
#define CONTENT_H 1

#include <string>
#include <tuple>
#include <vector>

typedef std::vector<std::tuple<std::string, std::string>> Redirections;

class Content {
public:
  virtual bool Init(std::string & basePath) = 0;
  virtual void Shutdown() = 0;
  virtual bool Render() = 0;
  virtual Redirections const GetRedirections() = 0;
};

#endif
