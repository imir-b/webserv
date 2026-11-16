
/* LibC includes */
#include <unistd.h>
#include <fcntl.h>

/* C++ incldues */
#include <iostream>
#include <limits>

/* HPP headers */
#include "../headers/ConfigParser.hpp"

/* Canonical orthodox form */

ConfigParser::ConfigParser(void) {
}

ConfigParser::ConfigParser(const ConfigParser &configParser) {
    (void)configParser;
}

ConfigParser &ConfigParser::operator=(const ConfigParser &configParser) {
    (void)configParser;
    return *this;
}

ConfigParser::~ConfigParser(void) {
}

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


static bool isValidMethod(const string &method)
{
    return method == "GET" || method == "POST" || method == "DELETE";
}

/* Extract allowed HTTP methods */
static int extract_methods(vector<string> &methods, const vector<string> &tokens, size_t &pos)
{
    methods.clear();

    size_t i = pos + 1;

    while (i < tokens.size() && tokens[i] != ";" && tokens[i] != "}")
    {
        if (!isValidMethod(tokens[i]))
            return -1;

        methods.push_back(tokens[i]);
        ++i;
    }

    if (i >= tokens.size() || tokens[i] != ";" || methods.empty())
        return -1;

    pos = i + 1;
    return 0;
}

/* Parse a directive inside a location block */
static int parse_location_directive(LocationConfig &config, const vector<string> &tokens, size_t &pos)
{
    if (pos >= tokens.size())
        return -1;

    const string &directive = tokens[pos];

    // Directives with multiple values
    if (directive == "allowed_methods")
    {
        vector<string> methods;

        if (extract_methods(methods, tokens, pos) < 0)
            return -1;

        config.setAllowedMethod(methods);
        return 0;
    }

    // Directives with a single value
    if (pos + 2 >= tokens.size() || tokens[pos + 2] != ";")
        return -1;

    const string &value = tokens[pos + 1];

    if (directive == "root")
        config.setRoot(value);

    else if (directive == "index")
        config.setIndex(value);

    else if (directive == "auto_index" || directive == "autoindex")
    {
        if (value == "1" || value == "true" || value == "on")
            config.setAutoIndex(true);

        else if (value == "0" || value == "false" || value == "off")
            config.setAutoIndex(false);

        else
            return -1;
    }
    else
        return -1;

    pos += 3;
    return 0;
}

/* Parse a location block */
static int parse_location(LocationConfig &config, const vector<string> &tokens, size_t &pos)
{
    if (pos >= tokens.size() || tokens[pos] != "location")
        return -1;

    if (pos + 2 >= tokens.size())
        return -1;

    // Get location path
    if (tokens[pos + 1].empty() || tokens[pos + 2] != "{")
        return -1;

    config.setPath(tokens[pos + 1]);

    // Move to first directive
    pos += 3;

    while (pos < tokens.size() && tokens[pos] != "}")
    {
        if (parse_location_directive(config, tokens, pos) < 0)
        {
            cerr << "Error: invalid location directive: "
                 << tokens[pos] << endl;
            return -1;
        }
    }

    // Check and consume closing brace
    if (pos >= tokens.size() || tokens[pos] != "}")
        return -1;

    pos++;

    return 0;
}

/* Parse a server directive */
static int parse_directive(Config &config, const vector<string> &tokens, size_t pos)
{
    if (pos + 2 >= tokens.size())
        return -1;

    const string &directive = tokens[pos];
    const string &value = tokens[pos + 1];

    if (value.empty() || tokens[pos + 2] != ";")
        return -1;

    if (directive == "listen")
    {
        string host = "0.0.0.0";
        string portStr = value;

        size_t separator = value.find(':');

        if (separator != string::npos)
        {
            host = value.substr(0, separator);
            portStr = value.substr(separator + 1);

            if (host.empty() || portStr.empty())
                return -1;
        }

        char *end;
        long port = strtol(portStr.c_str(), &end, 10);

        if (*end != '\0' || port < 1 || port > 65535)
            return -1;

        config.setHost(host);
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

        if (*end != '\0' ||
            size == 0 ||
            size > numeric_limits<size_t>::max() / multiplier)
            return -1;

        size *= multiplier;

        config.setClientMaxBodySize(static_cast<size_t>(size));
    }
    else
        return -1;

    return 0;
}

/* Parse configuration file to fill Server with multiple Config objects */
int ConfigParser::parseConfig(const string &filename, Server &server)
{
    string content;

    if (readConfigFile(filename, content) < 0)
        return -1;

    vector<string> tokens = tokenize(content);

    if (tokens.empty())
        return -1;

    size_t pos = 0;
    bool parsedAnyServer = false;

    while (pos < tokens.size())
    {
        // Each block must start with server
        if (tokens[pos] != "server")
        {
            cerr << "Error: expected 'server' at token "
                 << pos << endl;
            return -1;
        }

        if (pos + 1 >= tokens.size() || tokens[pos + 1] != "{")
            return -1;

        // Create a new configuration for each server block
        Config config;
        bool serverClosed = false;

        pos += 2;

        while (pos < tokens.size())
        {
            // End of current server block
            if (tokens[pos] == "}")
            {
                serverClosed = true;
                pos++;
                break;
            }

            // Parse location
            if (tokens[pos] == "location")
            {
                LocationConfig location;

                if (parse_location(location, tokens, pos) < 0)
                {
                    cerr << "Error: can't parse location at token "
                         << pos << endl;
                    return -1;
                }

                config.addLocation(location);
                continue;
            }

            // Parse server directive
            if (parse_directive(config, tokens, pos) < 0)
            {
                cerr << "Error: can't parse directive: "
                     << tokens[pos] << endl;
                return -1;
            }

            // Consume directive, value and semicolon
            pos += 3;
        }

        if (!serverClosed)
        {
            cerr << "Error: missing closing server brace" << endl;
            return -1;
        }

        // Add this server configuration without overwriting previous ones
        server.addConfig(config);
        parsedAnyServer = true;
    }

    if (!parsedAnyServer)
        return -1;

    return 0;
}
