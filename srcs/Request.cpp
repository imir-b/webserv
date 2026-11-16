
/* C++ includes */
#include <ostream>

/* HPP headers */
#include "../headers/Request.hpp"

/* Constructor */

Request::Request() : status(0) {}

Request::~Request() {}

ostream &operator<<(ostream &os, const Request &request)
{
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

const string& Request::getMethod() const
{
    return method;
}

const string& Request::getUri() const
{
    return uri;
}

const string& Request::getHttpVersion() const
{
    return httpVersion;
}

const string& Request::getBody() const
{
    return body;
}

const string& Request::getHeader(const string& name) const
{
    map<string, string>::const_iterator it = headers.find(name);

    if (it == headers.end())
    {
        static const string empty;
        return empty;
    }

    return it->second;
}

int Request::getStatus() const
{
    return status;
}

void Request::setMethod(const string& value)
{
    method = value;
}

void Request::setUri(const string& value)
{
    uri = value;
}

void Request::setHttpVersion(const string& version)
{
    httpVersion = version;
}

void Request::setBody(const string& value)
{
    body = value;
}

void Request::setStatus(int value)
{
    status = value;
}

void Request::addHeader(const string& key, const string& value)
{
    headers[key] = value;
}

bool Request::hasHeader(const string& name) const
{
    return headers.find(name) != headers.end();
}

void Request::reset()
{
    method.clear();
    uri.clear();
    httpVersion.clear();
    headers.clear();
    body.clear();
    status = 0;
}
