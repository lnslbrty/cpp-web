#ifndef FILESYSTEM_H
#define FILESYSTEM_H 1

#include <inja/inja.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct file_data
{
    std::vector<unsigned char> data;
};

class Filesystem
{
public:
    Filesystem()
    {
    }
    ~Filesystem()
    {
    }

    bool AddSingleFile(std::string path, std::string root);
    bool Scan(std::string root = "./wwwroot");
    bool Scan(std::string root, std::vector<std::string> extensions, bool exclude_extensions = false);
    const std::unordered_map<std::string, struct file_data> & GetFiles() const;

private:
    std::unordered_map<std::string, struct file_data> m_Files;
};

#endif
