
/* HPP headers */
#include "../headers/Config.hpp"
#include "../headers/Main.hpp"

/* Canonical orthodox form */

Config::Config(void)
    : listen(PORT),
    serverName("localhost"),
    root("./www"),
    index("index.html"),
    clientMaxBodySize(DEFAULT_MAX_BODY_SIZE) {
}

Config::Config(const Config &config)
    : listen(config.listen),
    host(config.host),
    serverName(config.serverName),
    root(config.root),
    index(config.index),
    clientMaxBodySize(config.clientMaxBodySize),
    locations(config.locations) {
}

Config &Config::operator=(const Config &config) {
    if (this != &config) {
        listen = config.listen;
        host = config.host;
        serverName = config.serverName;
        root = config.root;
        index = config.index;
        clientMaxBodySize = config.clientMaxBodySize;
        locations = config.locations;
    }
    return *this;
}

Config::~Config(void) {
}

/* Ostream */

ostream &operator<<(ostream &os, const Config &config) {
    os << "Config {" << endl;
    os << "    Listen: " << config.getListen() << endl;
    os << "    Host: " << config.getHost() << endl;
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

void Config::setHost(const string &addr) {
    host = addr;
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
int Config::getListen(void) const {
    return listen;
}

const string Config::getHost(void) const {
    return host;
}

const string Config::getServerName(void) const {
    return serverName;
}

const string Config::getRoot(void) const {
    return root;
}

const string Config::getIndex(void) const {
    return index;
}

size_t Config::getClientMaxBodySize(void) const {
    return clientMaxBodySize;
}

const vector<LocationConfig> &Config::getLocations(void) const {
    return locations;
}
