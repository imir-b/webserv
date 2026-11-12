
/* HPP headers */
#include "Config.hpp"
#include "Main.hpp"

/* Constructor */
Config::Config() : listen(PORT), serverName("localhost"), root("./www"), index("index.html"), clientMaxBodySize(DEFAULT_MAX_BODY_SIZE) {}

Config::~Config() {}

/* Ostream */

ostream &operator<<(ostream &os, const Config &config) {
    os << "Config {" << endl;
    os << "    Listen: " << config.getListen() << endl;
    os << "    ServerName: " << config.getServerName() << endl;
    os << "    Root: " << config.getRoot() << endl;
    os << "    Index: " << config.getIndex() << endl;
    os << "    ClientMaxBodySize: " << config.getClientMaxBodySize() << endl;

    os << "    Locations: [" << endl;

    const vector<LocationConfig> &locations = config.getLocations();

    for (size_t i = 0; i < locations.size(); ++i)
    {
        os << "        Location " << i + 1 << ":" << endl;
        os << locations[i] << endl;
    }

    os << "    ]" << endl;
    os << "}";

    return os;
}

/* Setters */
void Config::setListen(int port) {
    listen = port;
}

void Config::setServerName(const string &name) {
    serverName = name;
}

void Config::setRoot(const string &value) {
    root = value;
}

void Config::setIndex(const string &value) {
    index = value;
}

void Config::setClientMaxBodySize(size_t size) {
    clientMaxBodySize = size;
}

void Config::addLocation(const LocationConfig &location) {
    locations.push_back(location);
}

/* Getters */
int Config::getListen() const {
    return listen;
}

const string Config::getServerName() const {
    return serverName;
}

const string Config::getRoot() const {
    return root;
}

const string Config::getIndex() const {
    return index;
}

size_t Config::getClientMaxBodySize() const {
    return clientMaxBodySize;
}

const vector<LocationConfig> &Config::getLocations() const {
    return locations;
}
