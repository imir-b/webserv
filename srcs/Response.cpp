
/* C++ includes */
#include <sstream>

/* HPP headers */
#include "Response.hpp"

/* Constructor */
Response::Response() : statusCode(HTTP_OK) {}

Response::~Response() {}

/* Set HTTP status code */
void Response::setStatusCode(int code)
{
    statusCode = code;
}

/* Set or update a response header */
void Response::setHeader(const string &name, const string &value)
{
    headers[name] = value;
}

/* Set response body */
void Response::setBody(const string &body)
{
    this->body = body;
}

/* Get HTTP reason phrase corresponding to status code */
string Response::getReasonPhrase(void) const
{
    switch (statusCode)
    {
        case HTTP_OK:
            return "OK";

        case HTTP_CREATED:
            return "Created";

        case HTTP_BAD_REQUEST:
            return "Bad Request";

        case HTTP_FORBIDDEN:
            return "Forbidden";

        case HTTP_NOT_FOUND:
            return "Not Found";

        case HTTP_METHOD_NOT_ALLOWED:
            return "Method Not Allowed";

        case HTTP_PAYLOAD_TOO_LARGE:
            return "Content Too Large";

        case HTTP_INTERNAL_SERVER_ERROR:
            return "Internal Server Error";

        case HTTP_NOT_IMPLEMENTED:
            return "Not Implemented";

        default:
            return "Unknown";
    }
}

/* Convert Response object into a raw HTTP response */
string Response::serialize(void) const
{
    ostringstream response;

    /* Status line */
    response << "HTTP/1.1 " << statusCode << " " << getReasonPhrase() << "\r\n";

    /* Response headers */

    for (map<string, string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }

    /* Automatically calculate Content-Length */
    response << "Content-Length: " << body.size() << "\r\n";

    /* End of headers */
    response << "\r\n";

    /* Response body */
    response << body;

    return response.str();
}
