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

    private:
        Response handleGet(Response responce, const Request &request, const Config &config);
        Response handlePost(Response responce, const Request &request, const Config &config);
        Response handleDelete(Response responce, const Request &request, const Config &config);
        Response handleMethodNotAllowed(Response responce);
		void setMimeType(Response &response, const string &uri);
};
