
/* C++ includes */
#include <iostream>

/* HPP headers */
#include "LocationConfig.hpp"

/* Canonical orthodox form */

LocationConfig::LocationConfig(void)
    : path("/"),
    root(""),
    index("index.html"),
    autoindex(false) {
}

LocationConfig::LocationConfig(const LocationConfig &locationConfig) 
    : path(locationConfig.path),
    root(locationConfig.root),
    index(locationConfig.index),
    allowedMethods(locationConfig.allowedMethods),
    autoindex(locationConfig.autoindex) {
}

LocationConfig &LocationConfig::operator=(const LocationConfig &locationConfig) {
    if (this != &locationConfig) {
        path = locationConfig.path;
        root = locationConfig.root;
        index = locationConfig.index;
        allowedMethods = locationConfig.allowedMethods;
        autoindex = locationConfig.autoindex;
    }
    return *this;
}

LocationConfig::~LocationConfig(void) {
}

/* Ostream */

ostream &operator<<(ostream &os, const LocationConfig &location)
{
    os << "Location {" << endl;
    os << "    Path: " << location.getPath() << endl;
    os << "    Root: " << location.getRoot() << endl;
    os << "    Index: " << location.getIndex() << endl;

    os << "    Allowed Methods: [";

    const vector<string> &methods = location.getAllowedMethods();

    for (size_t i = 0; i < methods.size(); ++i)
    {
        os << methods[i];

        if (i + 1 < methods.size())
            os << ", ";
    }

    os << "]" << endl;

    os << "    AutoIndex: "
       << (location.getAutoIndex() ? "true" : "false") << endl;

    os << "}";

    return os;
}

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
