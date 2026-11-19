
/* HPP headers*/
#include "../headers/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "../headers/RequestHandler.hpp"

/* Constructor */

RequestHandler::RequestHandler()
{
	std::ifstream file("mime.types");

	if (!file.is_open())
		std::cerr << "Failed to open MIME types file" << std::endl;

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
			continue;
		std::stringstream ss(line);
		std::string extension;
		std::string mimeType;
		if (ss >> extension >> mimeType)
		{
			mimeTypesTable[extension] = mimeType;
		}
	}
	file.close();
}

RequestHandler::~RequestHandler() {}

/* Set MIME type based on the file extension */
void RequestHandler::setMimeType(Response &response, const string &uri)
{
	size_t dotPos = uri.find_last_of('.');
	if (dotPos != string::npos)
	{
		std::string extension = uri.substr(dotPos + 1);
		std::map<std::string, std::string>::iterator it = mimeTypesTable.find(extension);

		if (it != mimeTypesTable.end())
		{
			response.setHeader("Content-Type", it->second);
			return;
		}
	}
	else
	{
		response.setHeader("Content-Type", "application/octet-stream"); // Default MIME type
	}
}

/* Process the client's request and prepare the HTTP response */
Response RequestHandler::process(const Request &request, const Config &config)
{
	Response response;

	setMimeType(response, request.getUri());

    if (request.getMethod() == "GET")
        return handleGet(response, request, config);

    if (request.getMethod() == "POST")
        return handlePost(response, request, config);

    if (request.getMethod() == "DELETE")
        return handleDelete(response, request, config);

    return handleMethodNotAllowed(response);
}

/* Handle unsupported HTTP method */
Response RequestHandler::handleMethodNotAllowed(Response response)
{
	(void)response;

    response.setStatusCode(HTTP_METHOD_NOT_ALLOWED);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");
    response.setHeader("Allow", "GET, POST, DELETE");

    response.setBody("Method Not Allowed\n");

    return response;
}

// Code	Situation
// 200 OK	Fichier trouvé et envoyé
// 301 Moved Permanently	Redirection permanente
// 302 Found	Redirection temporaire
// 304 Not Modified	Ressource non modifiée
// 400 Bad Request	Requête invalide
// 403 Forbidden	Ressource interdite
// 404 Not Found	Fichier inexistant
// 405 Method Not Allowed	GET non autorisé sur la location
// 414 URI Too Long	URI trop longue
// 500 Internal Server Error	Erreur interne

/* Handle GET request */
Response RequestHandler::handleGet(Response response, const Request &request, const Config &config)
{
    (void) config;

	std::string filePath = "www" + request.getUri();
	if (filePath[filePath.length() - 1] == '/') {
		filePath += "index.html"; // Serve index.html for directory requests
		response.setHeader("Content-Type", "text/html");
	}
	// std::cout << "Requested file path: " << filePath << std::endl; // <<<--- debug

	std::ifstream file(static_cast<const char *>(filePath.c_str()));
	if (file.is_open()) {
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		response.setBody(content);
		response.setStatusCode(HTTP_OK);
		file.close();
	}
	else {
		response.setStatusCode(HTTP_NOT_FOUND);
		response.setHeader("Content-Type", "text/plain");
		response.setBody("404 Not Found\n");
	}
    response.setHeader("Connection", "close");
	// std::cout << "RESPONSE: " << response.serialize() << std::endl; // <<<--- debug
	return response;
}

/* Handle POST request */
Response RequestHandler::handlePost(Response response, const Request &request, const Config &config)
{
    (void) config;

    if (request.getBody().size() > config.getClientMaxBodySize()) {
        response.setStatusCode(HTTP_PAYLOAD_TOO_LARGE);
        response.setBody("413 Payload Too Large\n");
    }
    else {
        response.setStatusCode(HTTP_OK);
        response.setBody("POST received:\n" + request.getBody());
    }

    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");

	std::cout << "POST A GERER\n";

    return response;
}

/* Handle DELETE request */
Response RequestHandler::handleDelete(Response response, const Request &request, const Config &config)
{
    (void)request;
    (void)config;

    response.setStatusCode(HTTP_NOT_IMPLEMENTED);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");

    response.setBody("DELETE is not implemented yet\n");

	std::cout << "DELETE A GERER\n";

    return response;
}
