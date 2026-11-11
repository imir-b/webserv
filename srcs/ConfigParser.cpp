
/* LibC includes */
#include <unistd.h>
#include <fcntl.h>

/* C++ incldues */
#include <iostream>

/* HPP headers */
#include "ConfigParser.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

bool ConfigParser::isSeparator(char c) const
{
    return (isspace(static_cast<unsigned char>(c)) || c == '{' || c == '}' || c == ';');
}

/* Tokenize content to parse it easier */
vector<string> ConfigParser::tokenize(const string &content)
{
    vector<string> tokens;
    string current;
    size_t i = 0;

    while (i < content.size())
    {
        char c = content[i];

        if (isspace(static_cast<unsigned char>(c))) {
            i++;
            continue;
        }

        if (c == '#') {
            while (i < content.size() && content[i] != '\n')
                i++;
            continue;
        }

        if (c == '{' || c == '}' || c == ';') {
            tokens.push_back(string(1, c));
            i++;
            continue;
        }

        current.clear();

        while (i < content.size() && !isSeparator(content[i]) && content[i] != '#') {
            current += content[i];
            i++;
        }

        if (!current.empty())
            tokens.push_back(current);
    }

    return tokens;
}

/* Try to open and read configuration file */
int ConfigParser::readConfigFile(const string &filename, string content) {
    int fd = open(filename.c_str(), O_RDONLY);

    if (fd < 0) {
        std::cerr << "Failed to open " << filename << std::endl;
        return -1;
    }

    char buffer[1024];
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
        content.append(buffer, bytesRead);

    close(fd);

    if (bytesRead < 0) {
        std::cerr << "Failed to read " << filename << std::endl;
        return -1;
    }

    return 0;
}

/* Parse configuration file to fill Config class */
int ConfigParser::parseConfig(const string &filename, Server &server)
{
    string content;

    if (readConfigFile(filename, content) < 0)
        return -1;

    vector<string> tokens = tokenize(content);

    

    (void)server;
    return 0;
}