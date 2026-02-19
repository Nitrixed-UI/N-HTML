#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

std::string readFile(const std::string &path)
{
    std::ifstream f(path);
    if (!f)
        throw std::runtime_error("Cannot open file");
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

void logLine(const std::string &line)
{
    static std::ofstream log("../latest.log", std::ios::app);
    log << line << '\n';
}

void findMatches(const std::string &html,
                 const std::regex &re,
                 const std::string &label)
{
    for (std::sregex_iterator it(html.begin(), html.end(), re), end;
         it != end; ++it)
    {
        std::string out = label + ": " + it->str();
        std::cout << out << '\n';
        logLine(out);
    }
}

int main()
{
    const std::regex cookieRe(
        R"((Set-Cookie:\s*[^\r\n;]+|document\.cookie\s*=\s*["'][^"']+))",
        std::regex::icase);

    const std::regex tokenRe(
        R"((api[_-]?key|token|auth|access[_-]?token)["'\s:=]+([A-Za-z0-9\-_\.]+))",
        std::regex::icase);

    while (true)
    {
        std::cout << "Enter a file path (or 'exit'): ";
        std::string path;
        std::getline(std::cin, path);
        if (path == "exit")
            break;

        try
        {
            std::string html = readFile(path);
            findMatches(html, cookieRe, "Cookie");
            findMatches(html, tokenRe, "Token");
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}
