
#include "request.hpp"

Request::Request() : status(0) {}

Request::~Request() {}

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
