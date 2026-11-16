#pragma once

/* HPP headers */
#include "../headers/Request.hpp"
#include "../headers/Response.hpp"
#include "../headers/Config.hpp"

class RequestHandler
{
    public:
        /* Canonical orthodox form */
        RequestHandler();
        RequestHandler(const RequestHandler &requestHandler);
        RequestHandler &operator=(const RequestHandler &requestHandler);
        ~RequestHandler();

        /* Public methods */
        Response process(const Request &request, const Config &config);
        Response processError(int status);

    private:
        /* Private methods */
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
