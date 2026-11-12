#pragma once

/* C++ includes */
#include <vector>
#include <string>

using namespace std;

class LocationConfig {
    private:
        string path;
        string root;
        string index;
        vector<string> allowedMethods;
        bool autoindex;

    public:
        /* Constructor */
        LocationConfig();
        ~LocationConfig();

        /* Setters */
        void setPath(const string &path);
        void setRoot(const string &root);
        void setIndex(const string &index);
        void setAllowedMethod(const vector<string> &methods);
        void setAutoIndex(bool value);

        /* Getters */
        const string &getPath() const;
        const string &getRoot() const;
        const string &getIndex() const;
        const vector<string> &getAllowedMethods() const;
        bool getAutoIndex() const;
    
    friend ostream &operator<<(ostream &os, const LocationConfig &location);
};