#pragma once

/* HPP headers */
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"

class RequestHandler
{
    public:
        RequestHandler();
        ~RequestHandler();

        Response process(const Request &request, const Config &config);
        Response processError(int status);

    private:
        Response handleGet(const Request &request, const Config &config);
        Response handlePost(const Request &request, const Config &config);
        Response handleDelete(const Request &request, const Config &config);
        Response handleMethodNotAllowed();
        Response handlePayloadTooLarge();
        Response handleBadRequest();
        Response handleForbidden();
        Response handleInternalServerError();
        Response handleNotFound();
        Response handleNotImplemented();
};
