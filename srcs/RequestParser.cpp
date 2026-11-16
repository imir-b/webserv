
/* C++ includes */
#include <string>
#include <cctype>
#include <limits>

/* HPP headers */
#include "../headers/RequestParser.hpp"

using namespace std;

/* Canonical orthodox form */

RequestParser::RequestParser(void) {
}

RequestParser::RequestParser(const RequestParser &requestParser) {
    (void)requestParser;
}

RequestParser &RequestParser::operator=(const RequestParser &requestHandler) {
    (void) requestHandler;
    return *this;
}

RequestParser::~RequestParser(void) {
}

/**
 * Parsing utility functions
 */

string RequestParser::trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");

    if (start == string::npos)
        return "";

    size_t end = str.find_last_not_of(" \t");

    return str.substr(start, end - start + 1);
}

string RequestParser::toLower(const string& str) {
    string result = str;

    for (size_t i = 0; i < result.size(); ++i)
    {
        result[i] = static_cast<char>(
            tolower(static_cast<unsigned char>(result[i]))
        );
    }

    return result;
}

bool RequestParser::isValidToken(const string& str) {
    const string separators = "()<>@,;:\\\"/[]?={} \t";

    if (str.empty())
        return false;

    for (size_t i = 0; i < str.size(); ++i)
    {
        unsigned char c = str[i];

        if (c <= 32 || c >= 127 ||
            separators.find(str[i]) != string::npos)
            return false;
    }

    return true;
}

/**
 * Main parsing functions
 */

/* Parse the first line of the request */
int RequestParser::parseRequestLine(const string& line, Request& request) {
    size_t firstSpace = line.find(' ');
    size_t secondSpace = line.find(' ', firstSpace + 1);

    if (firstSpace == string::npos ||
        secondSpace == string::npos ||
        line.find(' ', secondSpace + 1) != string::npos)
        return INVALID_REQUEST;

    string method = line.substr(0, firstSpace);
    string uri = line.substr(
        firstSpace + 1,
        secondSpace - firstSpace - 1
    );
    string version = line.substr(secondSpace + 1);

    /* Validate the method */
    if (!isValidToken(method))
        return INVALID_REQUEST;

    /* Validate the URI */
    if (uri.empty())
        return INVALID_REQUEST;

    for (size_t i = 0; i < uri.size(); ++i)
    {
        unsigned char c = uri[i];

        if (c <= 32 || c == 127)
            return INVALID_REQUEST;
    }

    /* Validate the HTTP version */
    if (version != "HTTP/1.1" && version != "HTTP/1.0")
        return INVALID_REQUEST;

    request.setMethod(method);
    request.setUri(uri);
    request.setHttpVersion(version);

    return COMPLETE_REQUEST;
}

/* Validate the request headers */
int RequestParser::validateHeaders(Request& request) {
    /* HTTP/1.1 requires a Host header */
    if (request.getHttpVersion() == "HTTP/1.1") {
        if (!request.hasHeader("host") ||
            request.getHeader("host").empty())
            return INVALID_REQUEST;
    }

    bool hasTransferEncoding = request.hasHeader("transfer-encoding");
    bool hasContentLength = request.hasHeader("content-length");

    /* Reject requests containing both headers */
    if (hasTransferEncoding && hasContentLength)
        return INVALID_REQUEST;

    if (hasTransferEncoding) {
        string encoding = toLower(trim(
            request.getHeader("transfer-encoding")
        ));

        /* Only chunked encoding is supported */
        if (encoding != "chunked")
            return NOT_IMPLEMENTED;

        request.setIsChunked(true);
    }

    return COMPLETE_REQUEST;
}

/* Parse and validate the Content-Length header */
int RequestParser::parseContentLength(const string& value, size_t& length) {
    if (value.empty())
        return INVALID_REQUEST;

    length = 0;

    for (size_t i = 0; i < value.size(); ++i)
    {
        if (!isdigit(static_cast<unsigned char>(value[i])))
            return INVALID_REQUEST;

        size_t digit = value[i] - '0'; // extraction chirurgicale de content-length

        if (length > (numeric_limits<size_t>::max() - digit) / 10)
            return INVALID_REQUEST;

        length = length * 10 + digit;
    }

    return COMPLETE_REQUEST;
}

/* Parse a chunked request body */
int RequestParser::parseChunkedBody(const string& buffer, size_t bodyStart, Request& request, size_t& consumed) {
    size_t pos = bodyStart;
    string body;

    while (true) {
        /* Find the end of the chunk size line */
        size_t lineEnd = buffer.find("\r\n", pos);

        if (lineEnd == string::npos)
            return INCOMPLETE_REQUEST;

        string sizeLine = buffer.substr(pos, lineEnd - pos);

        /* Ignore chunk extensions for now */
        size_t extension = sizeLine.find(';');

        if (extension != string::npos)
            sizeLine.erase(extension);

        if (sizeLine.empty())
            return INVALID_REQUEST;

        /* Parse the chunk size in hexadecimal */
        size_t chunkSize = 0;

        for (size_t i = 0; i < sizeLine.size(); ++i) {
            unsigned char c = sizeLine[i];
            size_t digit;

            if (c >= '0' && c <= '9')
                digit = c - '0';
            else if (c >= 'a' && c <= 'f')
                digit = c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                digit = c - 'A' + 10;
            else
                return INVALID_REQUEST;

            /* Check hexadecimal overflow */
            if (chunkSize > (numeric_limits<size_t>::max() - digit) / 16)
                return INVALID_REQUEST;

            chunkSize = chunkSize * 16 + digit;
        }

        /* Move past the chunk size line */
        pos = lineEnd + 2;

        /* The last chunk has a size of zero */
        if (chunkSize == 0) {
            /* Read trailer headers until the empty line */
            while (true) {
                size_t trailerEnd = buffer.find("\r\n", pos);

                if (trailerEnd == string::npos)
                    return INCOMPLETE_REQUEST;

                /* Empty line marks the end of the request */
                if (trailerEnd == pos) {
                    pos += 2;
                    request.setBody(body);
                    consumed = pos;
                    return COMPLETE_REQUEST;
                }

                string trailer = buffer.substr(pos, trailerEnd - pos);

                /* Validate trailer syntax */
                size_t colon = trailer.find(':');

                if (colon == string::npos ||
                    !isValidToken(trailer.substr(0, colon)))
                    return INVALID_REQUEST;

                string name = toLower(trailer.substr(0, colon));

                /* Framing headers are not allowed in trailers */
                if (name == "content-length" ||
                    name == "transfer-encoding" ||
                    name == "host")
                    return INVALID_REQUEST;

                pos = trailerEnd + 2;
            }
        }

        /* Check whether the complete chunk has been received */
        if (chunkSize > buffer.size() - pos)
            return INCOMPLETE_REQUEST;

        /* Append the chunk data */
        body.append(buffer, pos, chunkSize);
        pos += chunkSize;

        /* Each chunk must end with CRLF */
        if (buffer.size() - pos < 2)
            return INCOMPLETE_REQUEST;

        if (buffer.compare(pos, 2, "\r\n") != 0)
            return INVALID_REQUEST;

        pos += 2;
    }
}

/* Parse the request headers */
int RequestParser::parseHeaders(const string& buffer, size_t start, size_t end, Request& request) {
    size_t pos = start;

    while (pos < end)
    {
        size_t lineEnd = buffer.find("\r\n", pos);

        if (lineEnd == string::npos || lineEnd > end)
            return INVALID_REQUEST;

        string line = buffer.substr(pos, lineEnd - pos);

        /* Reject empty lines and obsolete line folding */
        if (line.empty() || line[0] == ' ' || line[0] == '\t')
            return INVALID_REQUEST;

        /* Find the header name/value separator */
        size_t colon = line.find(':');

        if (colon == string::npos)
            return INVALID_REQUEST;

        string name = line.substr(0, colon);
        string value = trim(line.substr(colon + 1));

        /* Validate the header name */
        if (!isValidToken(name))
            return INVALID_REQUEST;

        /* Validate the header value characters */
        for (size_t i = 0; i < value.size(); ++i)
        {
            unsigned char c = value[i];

            if ((c < 32 && c != '\t') || c == 127)
                return INVALID_REQUEST;
        }

        /* Normalize the header name to lowercase */
        name = toLower(name);

        /* Handle duplicate headers */
        if (request.hasHeader(name))
        {
            /* Host and Content-Length must not be duplicated */
            if (name == "host" || name == "content-length")
                return INVALID_REQUEST;

            /* Transfer-Encoding is not supported yet */
            if (name == "transfer-encoding")
                return INVALID_REQUEST;

            /* Cookie values are separated with semicolons */
            if (name == "cookie")
                value = request.getHeader(name) + "; " + value;

            /* Other supported list headers are separated with commas */
            else if (name == "accept" ||
                    name == "accept-encoding" ||
                    name == "connection")
                value = request.getHeader(name) + ", " + value;

            else
                return INVALID_REQUEST;

            request.setHeader(name, value);
        }
        else
        {
            request.addHeader(name, value);
        }

        request.addHeader(name, value);

        pos = lineEnd + 2;
    }

    return COMPLETE_REQUEST;
}

/* Main function to parse the request and validate its grammar */
int RequestParser::parse(const string& buffer, Request& request, size_t& consumed) {
    consumed = 0;

    /* Find the end of the headers */
    size_t headerEnd = buffer.find("\r\n\r\n");

    if (headerEnd == string::npos)  
        return INCOMPLETE_REQUEST;

    /* Find the request line */
    size_t lineEnd = buffer.find("\r\n");

    if (lineEnd == string::npos || lineEnd > headerEnd)
        return INVALID_REQUEST;

    request.reset();

    /* Parse the request line */
    string requestLine = buffer.substr(0, lineEnd);

    if (parseRequestLine(requestLine, request) != COMPLETE_REQUEST)
        return INVALID_REQUEST;

    /* Parse the headers */
    if (parseHeaders(
        buffer,
        lineEnd + 2,
        headerEnd,
        request) != COMPLETE_REQUEST)
        return INVALID_REQUEST;

    /* Validate the headers */
    if (validateHeaders(request) != COMPLETE_REQUEST)
        return INVALID_REQUEST;

    /* Find the beginning of the request body */
    size_t bodyStart = headerEnd + 4;

    /* Parse chunked body */
    if (request.getIsChunked())
        return parseChunkedBody(buffer, bodyStart, request, consumed);

    /* Retrieve Content-Length */
    size_t contentLength = 0;

    if (request.hasHeader("content-length")) {
        if (parseContentLength(
            request.getHeader("content-length"),
            contentLength) != COMPLETE_REQUEST)
            return INVALID_REQUEST;
    }

    /* Check whether the complete body has been received */
    if (contentLength > buffer.size() - bodyStart)
        return INCOMPLETE_REQUEST;

    /* Store the request body */
    request.setBody(buffer.substr(bodyStart, contentLength));

    /* Calculate the number of consumed bytes */
    consumed = bodyStart + contentLength;

    return COMPLETE_REQUEST;
}
