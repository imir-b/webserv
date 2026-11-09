
#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

class Request {
    private:
        string method;
        string uri;
        string httpVersion;

        map<string, string> headers;
        string body;

        int status;

    public:
        Request();
        ~Request();

        // Getters
        const string& getMethod() const;
        const string& getUri() const;
        const string& getHttpVersion() const;
        const string& getBody() const;
        const string& getHeader(const string& name) const;
        int getStatus() const;

        // Setters
        void setMethod(const string& method);
        void setUri(const string& uri);
        void setHttpVersion(const string& version);
        void setBody(const string& body);
        void setStatus(int status);

        void addHeader(const string& key, const string& value);
        bool hasHeader(const string &name) const;
        void reset();

};