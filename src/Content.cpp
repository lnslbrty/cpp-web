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

bool TemplatedContent::Render(std::string & out)
{
  out = "tmpl";

  return false;
}

void TemplatedContent::SetTemplateData(nlohmann::json & data)
{
  m_TemplateData = data;
}
