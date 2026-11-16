
/* C++ includes */
#include <ostream>

/* HPP headers */
#include "../headers/Request.hpp"

/* Canonical orthodox form */

Request::Request(void)
    : status(0) {
}

Request::Request(const Request &request) 
    : method(request.method),
    uri(request.uri),
    httpVersion(request.httpVersion),
    headers(request.headers),
    body(request.body),
    status(request.status),
    isChunked(request.isChunked) {
}

Request &Request::operator=(const Request &request) {
    if (this != &request) {
        method = request.method;
        uri = request.uri;
        httpVersion = request.httpVersion;
        headers = request.headers;
        body = request.body;
        status = request.status;
        isChunked = request.isChunked;
    }
    return *this;
}

Request::~Request(void) {
}

/* Ostream */

ostream &operator<<(ostream &os, const Request &request) {
    os << "# HTTP REQUEST" << endl;

    os << "Method: " << request.method << endl;
    os << "URI: " << request.uri << endl;
    os << "HTTP Version: " << request.httpVersion << endl;

    os << "\nHeaders:" << endl;

    if (request.headers.empty())
        os << "  <No headers>" << endl;

    for (map<string, string>::const_iterator it = request.headers.begin();
         it != request.headers.end(); ++it)
    {
        os << "  " << it->first << ": " << it->second << endl;
    }

    os << "\nBody (" << request.body.size() << " bytes):" << endl;

    if (request.body.empty())
        os << "  <Empty>" << endl;
    else
        os << request.body << endl;

    os << endl;

    return os;
}

/* Getters */

const string& Request::getMethod(void) const {
    return method;
}

const string& Request::getUri(void) const {
    return uri;
}

const string& Request::getHttpVersion(void) const {
    return httpVersion;
}

const string& Request::getBody(void) const {
    return body;
}

const string& Request::getHeader(const string& name) const {
    map<string, string>::const_iterator it = headers.find(name);

    if (it == headers.end())
    {
        static const string empty;
        return empty;
    }

    return it->second;
}

int Request::getStatus(void) const {
    return status;
}

bool Request::getIsChunked(void) const {
    return isChunked;
}

/* Setters */

void Request::setMethod(const string& value) {
    method = value;
}

void Request::setUri(const string& value) {
    uri = value;
}

void Request::setHttpVersion(const string& version) {
    httpVersion = version;
}

void Request::setBody(const string& value) {
    body = value;
}

void Request::setStatus(int value) {
    status = value;
}

void Request::setIsChunked(bool value) {
    isChunked = value;
}

/* Header management */

void Request::addHeader(const string& key, const string& value) {
    headers[key] = value;
}

void Request::setHeader(const string& key, const string& value) {
    headers[key] = value;
}

bool Request::hasHeader(const string& name) const {
    return headers.find(name) != headers.end();
}

void Request::reset(void) {
    method.clear();
    uri.clear();
    httpVersion.clear();
    headers.clear();
    body.clear();
    status = 0;
    isChunked = false;
}
