
/* HPP headers */
#include "LocationConfig.hpp"

/* Constructor */
LocationConfig::LocationConfig() : path("/"), root(""), index("index.html"), autoindex(false) {}

LocationConfig::~LocationConfig() {}

/* Setters */
void LocationConfig::setPath(const string &value) {
    path = value;
}

void LocationConfig::setRoot(const string &value) {
    root = value;
}

void LocationConfig::setIndex(const string &value) {
    index = value;
}

void LocationConfig::setAllowedMethod(const vector<string> &methods) {
    allowedMethods = methods;
}

void LocationConfig::setAutoIndex(bool value) {
    autoindex = value;
}

/* Getters */
const string &LocationConfig::getPath() const {
    return path;
}

const string &LocationConfig::getRoot() const {
    return root;
}

const string &LocationConfig::getIndex() const {
    return index;
}

const vector<string> &LocationConfig::getAllowedMethods() const {
    return allowedMethods;
}

bool LocationConfig::getAutoIndex() const {
    return autoindex;
}
