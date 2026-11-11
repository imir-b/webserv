#pragma once

/* C++ includes */
#include <map>
#include <string>

using namespace std;

/* Status code defines */
enum HttpStatusCode
{
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_BAD_REQUEST = 400,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_CONTENT_TOO_LARGE = 413,
    HTTP_INTERNAL_SERVER_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501
};

class Response
{
    private:
        int statusCode;
        map<string, string> headers;
        string body;

    public:
        /* Constructor */
        Response();
        ~Response();

        /* Setters */
        void setStatusCode(int code);
        void setHeader(const string& name, const string& value);
        void setBody(const string& body);

        /* Methods */
        string getReasonPhrase() const;
        string serialize() const;
};
