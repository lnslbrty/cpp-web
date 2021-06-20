#ifndef FILESYSTEM_H
#define FILESYSTEM_H 1

#include <inja/inja.hpp>
#include <unordered_map>
#include <string>
#include <vector>

struct file_data {
  std::vector<unsigned char> data;
};

class Filesystem {
public:
  Filesystem() {}
  ~Filesystem() {}

  bool AddSingleFile(std::string path, std::string root);
  bool Scan(std::string root = "./wwwroot");
  void AddInjaCallback(std::string functionName, std::size_t numberOfArgs, inja::CallbackFunction function);
  void AddVoidInjaCallback(std::string functionName, std::size_t numberOfArgs, inja::VoidCallbackFunction function);

private:
  std::unordered_map<std::string, struct file_data> m_Files;
  inja::TemplateStorage m_Templates;
  inja::Environment m_Inja;
};

#endif
