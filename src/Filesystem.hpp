#ifndef FILESYSTEM_H
#define FILESYSTEM_H 1

#include <map>
#include <string>
#include <vector>

struct file_data {
  bool inja_renderable;
  std::vector<unsigned char> data;
};

class Filesystem {
public:
  Filesystem() {}
  ~Filesystem() {}

  bool AddSingleFile(std::string path, std::string root);
  bool Scan(std::string root = "./wwwroot");

private:
  std::map<std::string, struct file_data> files;
};

#endif
