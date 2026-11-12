
/* LibC includes */
#include <unistd.h>
#include <fcntl.h>

/* C++ incldues */
#include <iostream>
#include <limits>
#include <climits>
#include <cstdlib>

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
int ConfigParser::readConfigFile(const string &filename, string &content) {
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

/* Parse tokens to fill LocationConfig class */
static int parse_location(LocationConfig &config, vector<string> &tokens, int pos) {
    (void)config;
    (void)tokens;
    (void)pos;

    return 0;
}

static int parse_directive(Config &config, const vector<string> &tokens, size_t pos) {
    if (pos + 2 >= tokens.size())
        return -1;

    const string &directive = tokens[pos];
    const string &value = tokens[pos + 1];

    if (value.empty() || tokens[pos + 2] != ";")
        return -1;

    if (directive == "listen")
    {
        char *end;
        long port = strtol(value.c_str(), &end, 10);

        if (*end != '\0' || port < 1 || port > 65535)
            return -1;

        config.setListen(static_cast<int>(port));
    }
    else if (directive == "server_name")
        config.setServerName(value);
    else if (directive == "root")
        config.setRoot(value);
    else if (directive == "index")
        config.setIndex(value);
    else if (directive == "client_max_body_size")
    {
        string number = value;
        size_t multiplier = 1;

        char suffix = number[number.size() - 1];

        if (suffix == 'K' || suffix == 'k')
        {
            multiplier = 1024;
            number.erase(number.size() - 1);
        }
        else if (suffix == 'M' || suffix == 'm')
        {
            multiplier = 1024 * 1024;
            number.erase(number.size() - 1);
        }
        else if (suffix == 'G' || suffix == 'g')
        {
            multiplier = 1024ULL * 1024 * 1024;
            number.erase(number.size() - 1);
        }

        if (number.empty())
            return -1;

        char *end;
        unsigned long long size = strtoull(number.c_str(), &end, 10);

        if (*end != '\0' || size == 0 || size > SIZE_MAX / multiplier)
            return -1;

        size *= multiplier;

        config.setClientMaxBodySize(static_cast<size_t>(size));
    }
    else
        return -1;

    return 0;
}

/* Parse configuration file to fill Config class */
int ConfigParser::parseConfig(const string &filename, Server &server)
{
    string content;

    if (readConfigFile(filename, content) < 0)
        return -1;

    vector<string> tokens = tokenize(content);

    if (tokens[0].compare("server")) {
        return -1;
    }

    Config config = server.getConfig();
    size_t pos = 0;
    int open_brace = 0;

    cout << "Tokens :" << endl;

    while (pos < tokens.size()) {
        cout << "[" << pos << "] " << tokens[pos] << endl; // print tokens <<<---

        pos++;

        if (!tokens[pos].compare("{"))
            open_brace++;
        else if (!tokens[pos].compare("}"))
            open_brace--;
        if (open_brace == 0)
            break;
        
        if (!tokens[pos].compare("location")) {
            LocationConfig location;

            if (parse_location(location, tokens, pos) < 0) {
                cerr << "Error : can't parse location tokens[" << pos << "]" << endl;
                return -1;
            }
            config.addLocation(location);
            
            continue;
        }

        if (parse_directive(config, tokens, pos) < 0) {
            cerr << "Error : can't parse directive : " << tokens[pos] << endl;
            return -1;
        }
        else
            pos += 2;
        
    }

    return 0;
}