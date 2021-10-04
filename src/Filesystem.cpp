#include "Filesystem.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

static std::string make_path_relative(std::string & path, std::string & root)
{
    return std::filesystem::relative(path, root);
}

static bool is_directory(std::string & path)
{
    return std::filesystem::is_directory(path);
}

bool Filesystem::AddSingleFile(std::string path, std::string root)
{
    if (is_directory(path) == true)
    {
        return true;
    }

    std::ifstream ifs(path, std::ios::binary | std::ios::ate);

    if (!ifs)
    {
        return false;
    }

    auto end = ifs.tellg();
    if (end <= 0)
    {
        return false;
    }
    if (!ifs.seekg(0, std::ios::beg))
    {
        return false;
    }

    auto size = end - ifs.tellg();

    if (size <= 0)
    {
        return false;
    }

    struct file_data fd = {};
    try
    {
        fd.data.resize(size);
    }
    catch (const std::exception & e)
    {
        return false;
    }

    if (!ifs.read((char *)fd.data.data(), fd.data.size()))
    {
        return false;
    }

    std::string relpath = make_path_relative(path, root);
    if (m_Files.count(relpath) > 0)
    {
        std::cout << "Adding file: " << path << " (" << size << " bytes) and overwriting " << relpath << " to " << this
                  << std::endl;
    }
    else
    {
        std::cout << "Adding file: " << path << " (" << size << " bytes) as " << relpath << " to " << this << std::endl;
    }

    fd.mime = magic_file(m_Magic, path.c_str());
    m_Files[relpath] = fd;

    return true;
}

bool Filesystem::Scan(std::string root)
{
    return Scan(root, {}, true);
}

bool Filesystem::Scan(std::string root, std::vector<std::string> extensions, bool exclude_extensions)
{
    bool retval = true;

    for (const auto & entry : std::filesystem::recursive_directory_iterator(root))
    {
        std::string ext = std::filesystem::path(entry).extension();
        bool found_extension = false;

        for (const auto & extension : extensions)
        {
            if (ext == "." + extension)
            {
                found_extension = true;
                break;
            }
        }

        if (found_extension == true && exclude_extensions == false)
        {
            if (AddSingleFile(entry.path(), root) == false)
            {
                retval = false;
            }
        }
        if (found_extension == false && exclude_extensions == true)
        {
            if (AddSingleFile(entry.path(), root) == false)
            {
                retval = false;
            }
        }
    }

    return retval;
}

FilesDict & Filesystem::GetFiles()
{
    return m_Files;
}

void Filesystem::MagicInit()
{
    m_Magic = magic_open(MAGIC_MIME_TYPE);

    magic_load(m_Magic, NULL);
    magic_compile(m_Magic, NULL);
}

void Filesystem::MagicClose()
{
    magic_close(m_Magic);
}
