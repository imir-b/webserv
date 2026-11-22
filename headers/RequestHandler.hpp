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
        Response handleGet(Response responce, std::string filePath, const Config &config);
        Response handlePost(Response responce, const Request &request, std::string filePath, const Config &config);
        Response handleDelete(Response responce, std::string filePath, const Config &config);
        Response handleMethodNotAllowed(Response responce);

		void setMimeType(Response &response, const string &uri);
		bool isInsideRoot(const std::string& root, const std::string& candidatePath);
		bool isPostTargetValid(const std::string &root, const std::string &filePath);

		std::map<std::string, std::string> mimeTypesTable;
};
