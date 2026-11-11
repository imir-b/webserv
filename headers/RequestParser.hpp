#pragma once

/* C++ includes */
#include <string>
#include <map>
#include <cstddef>

/* HPP headers */
#include "Request.hpp"

using namespace std;

/* Parsing status */
enum ParseResult
{
    INCOMPLETE_REQUEST,
    COMPLETE_REQUEST,
    INVALID_REQUEST
};

class RequestParser
{
    public:
        /* Constructor */
        RequestParser();
        ~RequestParser();
        /* Main function for parsing */
        int parse(const string& buffer, Request& request, size_t& consumed);

    private:
        /* Extract methods */
        int parseRequestLine(const string& line, Request& request);
        int parseHeaders(const string& buffer, size_t start, size_t end, Request& request);
        int validateHeaders(Request& request);
        int parseContentLength(const string& value, size_t& length);

        /* Utils */
        static string trim(const string& str);
        static string toLower(const string& str);
        static bool isValidToken(const string& str);
};