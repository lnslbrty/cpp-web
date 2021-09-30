#include "TemplateManager.hpp"

#include <filesystem>

TemplateManager::TemplateManager()
{
    AddInjaCallback("test_fn", 0, [](inja::Arguments & args) {
        (void)args;
        return "Just a test fn.";
    });
    AddInjaCallback("test_return_true", 0, [](inja::Arguments & args) {
        (void)args;
        return true;
    });
}

void TemplateManager::ParseTemplates(Filesystem const & fs)
{
    for (auto & tpl : fs.GetFiles())
    {
        std::string tmpl(tpl.second.data.data(), tpl.second.data.data() + tpl.second.data.size());
        m_Templates[tpl.first] = m_Inja.parse(tmpl);
        std::cout << "File: " << tpl.first << " may contain a renderable template." << std::endl;
    }
}

void TemplateManager::AddInjaCallback(std::string functionName,
                                      std::size_t numberOfArgs,
                                      inja::CallbackFunction function)
{
    m_Inja.add_callback(functionName, numberOfArgs, function);
}

void TemplateManager::AddVoidInjaCallback(std::string functionName,
                                          std::size_t numberOfArgs,
                                          inja::VoidCallbackFunction function)
{
    m_Inja.add_void_callback(functionName, numberOfArgs, function);
}

bool TemplateManager::TemplateExists(std::string const & templatePath)
{
    return m_Templates.find(templatePath) != m_Templates.end();
}

bool TemplateManager::RenderTemplate(std::string const & templatePath, RenderData const & rd, std::string & out)
{
    if (TemplateExists(templatePath) == false)
    {
        return false;
    }

    inja::json ij(rd);
    out = m_Inja.render(m_Templates[templatePath].content, ij);

    return true;
}
