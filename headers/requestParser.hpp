#pragma once

#include <string>
#include <map>
#include <cstddef>

#include "request.hpp"

using namespace std;

enum ParseResult
{
    INCOMPLETE_REQUEST,
    COMPLETE_REQUEST,
    INVALID_REQUEST
};

class RequestParser
{
    public:
        RequestParser();
        ~RequestParser();

        int parse(
            const string& buffer,
            Request& request,
            size_t& consumed
        );

    private:
        int parseRequestLine(
            const string& line,
            Request& request
        );

        int parseHeaders(
            const string& buffer,
            size_t start,
            size_t end,
            Request& request
        );

        int validateHeaders(Request& request);

        int parseContentLength(
            const string& value,
            size_t& length
        );

        static string trim(const string& str);
        static string toLower(const string& str);
        static bool isValidToken(const string& str);
};