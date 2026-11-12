#pragma once

/* HPP headers */
#include "Server.hpp"

/* C++ includes */
#include <string>
#include <vector>

using namespace std;

class ConfigParser {
    private:
        bool isSeparator(char c) const;
    public:
        ConfigParser();
        ~ConfigParser();

        vector<string> tokenize(const string &content);
        int readConfigFile(const string &filename, string &content);
        int parseConfig(const string &filename, Server &server);
};
