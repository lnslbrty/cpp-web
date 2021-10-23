#ifndef FILESYSTEM_H
#define FILESYSTEM_H 1

#include <magic.h>

#include <inja/inja.hpp>
#include <string>
#include <unordered_map>
#include <vector>

using FilesDict = std::unordered_map<std::string, struct file_data>;
using Data = std::vector<unsigned char>;

struct file_data
{
    std::string mime;
    Data data;
};

class Filesystem
{
public:
    Filesystem()
    {
        MagicInit();
    }
    ~Filesystem()
    {
        MagicClose();
    }

    bool Scan(std::string root = "./wwwroot");
    bool Scan(std::string root, std::vector<std::string> extensions, bool exclude_extensions = false);
    FilesDict & GetFiles();
    void GetFilenamesSorted(std::vector<std::string> & sortedFilenames);

private:
    bool AddSingleFile(std::string path, std::string root);
    void MagicInit();
    void MagicClose();

    FilesDict m_Files;
    magic_t m_Magic;
};

#endif
