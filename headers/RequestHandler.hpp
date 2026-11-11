#pragma once

#include "Request.hpp"
#include "Response.hpp"

class RequestHandler
{
    public:
        RequestHandler();
        ~RequestHandler();

        Response process(const Request& request);

    private:
        Response handleGet(const Request& request);
        Response handlePost(const Request& request);
        Response handleDelete(const Request& request);
        Response handleMethodNotAllowed();
};
