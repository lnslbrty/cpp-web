#include "Content.hpp"

TemplatedContent::TemplatedContent(std::string filesystemPath) : m_FilesystemPath(filesystemPath)
{
}

bool TemplatedContent::Init()
{
  return false;
}

void TemplatedContent::Shutdown()
{
}

bool TemplatedContent::Render(RequestResponse & rr, std::string & out)
{
  (void)rr;

  out = "tmpl";

  return false;
}

void TemplatedContent::GetRequiredFiles(std::vector<std::string> & requiredFiles) const
{
  requiredFiles.push_back(m_FilesystemPath);
}
