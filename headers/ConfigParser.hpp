#pragma once

/* HPP headers */
#include "Server.hpp"

/* C++ includes */
#include <string>
#include <vector>

using namespace std;

class ConfigParser {
    private:
        /* Private methods */
        bool isSeparator(char c) const;
    public:
        /* Canonical orthodox form */
        ConfigParser();
        ConfigParser(const ConfigParser &configParser);
        ConfigParser &operator=(const ConfigParser &configParser);
        ~ConfigParser();

        /* Public methods */
        vector<string> tokenize(const string &content);
        int readConfigFile(const string &filename, string &content);
        int parseConfig(const string &filename, Server &server);
};
