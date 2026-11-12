#pragma once

/* HPP headers */
#include "LocationConfig.hpp"

/* C++ includes */
#include <map>
#include <string>
#include <vector>

using namespace std;

class Config {
    private:
        int listen;
        string serverName;
        string root;
        string index;
        size_t clientMaxBodySize;
        vector<LocationConfig> locations;

    public:
        /* Constructor */
        Config();
        ~Config();

        /* Setters */
        void setListen(int port);
        void setServerName(const string &name);
        void setRoot(const string &root);
        void setIndex(const string &index);
        void setClientMaxBodySize(size_t size);
        void addLocation(const LocationConfig &location);

        /* Getters */
        int getListen() const;
        const string getServerName() const;
        const string getRoot() const;
        const string getIndex() const;
        size_t getClientMaxBodySize() const;
        const vector<LocationConfig> &getLocations() const;

    friend ostream &operator<<(ostream &os, const Config &config);
};

