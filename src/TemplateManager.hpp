#ifndef TEMPLATEMANAGER_H
#define TEMPLATEMANAGER_H 1

#include "Content.hpp"
#include "Filesystem.hpp"

#include <inja/inja.hpp>

class TemplateManager
{
public:
    TemplateManager();
    ~TemplateManager()
    {
    }

    void ParseTemplates(Filesystem & fs);
    void AddInjaCallback(std::string functionName, std::size_t numberOfArgs, inja::CallbackFunction function);
    void AddVoidInjaCallback(std::string functionName, std::size_t numberOfArgs, inja::VoidCallbackFunction function);
    bool TemplateExists(std::string const & templatePath);
    bool RenderTemplate(std::string const & templatePath, RenderData const & rd, std::string & out);

private:
    inja::TemplateStorage m_Templates;
    inja::Environment m_Inja;
};

#endif
