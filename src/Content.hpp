#ifndef CONTENT_H
#define CONTENT_H 1

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

#endif
