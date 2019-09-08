#include <iostream>

#include <httplib.h>
#include <inja/inja.hpp>


void example_inja_render(const httplib::Request& req, httplib::Response& res)
{
	inja::Environment env;
	nlohmann::json data;
	data["name"] = "Peter";
	data["city"] = "Brunswick";
	data["age"] = 29;
	data["names"] = {"Jeff", "Seb"};
	data["brother"]["name"] = "Chris";
	data["brother"]["daughters"] = {"Maria", "Helen"};
	data["brother"]["daughter0"] = { { "name", "Maria" } };
	data["is_happy"] = true;
	data["is_sad"] = false;
	data["relatives"]["mother"] = "Maria";
	data["relatives"]["brother"] = "Chris";
	data["relatives"]["sister"] = "Jenny";
	data["vars"] = {2, 3, 4, 0, -1, -2, -3};

    res.set_content(
        env.render("<html><body>\n"
                   "Hello {{ name }}! I come from {{ city }}.<br>\n"
                   "Hello {{ names.1 }}!<br>\n"
                   "Hello {{ brother.name }}!<br>\n"
                   "Hello {{ brother.daughter0.name }}!<br>\n"
                   "{{ \"{{ no_value }}\" }}<br>\n"
                   "Hello{# This is a comment #}!<br>\n"
                   "{# --- #Todo --- #}<br>\n"
                   "{% for name in names %}a{% endfor %}<br>\n"
                   "Hello {% for name in names %}{{ name }} {% endfor %}!<br>\n"
                   "Hello {% for name in names %}{{ loop.index }}: {{ name }}, {% endfor %}!<br>\n"
                   "{% for type, name in relatives %}{{ type }}: {{ name }}, {% endfor %}<br>\n"
                   "{% for v in vars %}{% if v > 0 %}+{% endif %}{% endfor %}<br>\n"
                   "{% for name in names %}{{ loop.index }}: {{ name }}{% if not loop.is_last %}, {% endif %}{% endfor %}!<br>\n"
                   "{% for name in names %}{{ loop.index }}: {{ name }}{% if loop.is_last == false %}, {% endif %}{% endfor %}!<br>\n"
                   "{% for name in {} %}a{% endfor %}<br>\n"
                   "</body></html>\n",
                   data), "text/html");
}

bool setup_httplib(const char * const addr, uint16_t port)
{
    using namespace httplib;

    Server svr;

    svr.Get("/", [](const Request& req, Response& res) {
        example_inja_render(req, res);
    });

    svr.Get(R"(/numbers/(\d+))", [&](const Request& req, Response& res) {
        auto numbers = req.matches[1];
        res.set_content(numbers, "text/plain");
    });

    svr.Get("/stop", [&](const Request& req, Response& res) {
        svr.stop();
    });

    return svr.listen(addr, port);
}

int main(int argc, char **argv)
{
    setup_httplib("127.0.0.1", 8080);
}
