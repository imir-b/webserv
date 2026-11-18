
/* HPP headers*/
#include "../headers/Config.hpp"
#include <iostream>
#include <fstream>
#include "../headers/RequestHandler.hpp"

/* Constructor */

RequestHandler::RequestHandler() {}

RequestHandler::~RequestHandler() {}

/* Set MIME type based on the file extension */
void RequestHandler::setMimeType(Response &response, const string &uri)
{
	size_t dotPos = uri.find_last_of('.');
	if (dotPos != string::npos)
	{
		string extension = uri.substr(dotPos + 1);
		if (extension == "html" || extension == "htm")
			response.setHeader("Content-Type", "text/html");
		else if (extension == "css")
			response.setHeader("Content-Type", "text/css");
		else if (extension == "js")
			response.setHeader("Content-Type", "application/javascript");
		else if (extension == "json")
			response.setHeader("Content-Type", "application/json");
		else if (extension == "png")
			response.setHeader("Content-Type", "image/png");
		else if (extension == "jpg" || extension == "jpeg")
			response.setHeader("Content-Type", "image/jpeg");
		else if (extension == "gif")
			response.setHeader("Content-Type", "image/gif");
		else if (extension == "txt")
			response.setHeader("Content-Type", "text/plain");
		else
			response.setHeader("Content-Type", "application/octet-stream"); // Default MIME type
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

    if (request.getUri() == "/") {
        response.setStatusCode(HTTP_OK);
        // response.setBody("Hello World!\n");

		std::ifstream file("www/index.html");
		if (file.is_open()) {
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			response.setBody(content);
			// std::cout << content << "\n"; // <<<--- debug

			file.close();
		}
		// response.setHeader("Content-Type", "text/html"); // <<<--- car index.html est de ce type mais plus tard il faut gerer en parsant l extension du fichier: c est ca le MIME type

    }
	else if (request.getUri() == "/test") {
		response.setStatusCode(HTTP_OK);
		response.setBody("Test page\n");
		response.setHeader("Content-Type", "text/plain");
	}
	else if (request.getUri() == "/eminem") {
		response.setStatusCode(HTTP_OK);
		std::ifstream file("www/eminem.png");
		if (file.is_open()) {
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			response.setBody(content);
			file.close();
		}
		response.setHeader("Content-Type", "image/png");
	}
    else {
        response.setStatusCode(HTTP_NOT_FOUND);
        response.setBody("Not Found\n");
    }

    response.setHeader("Connection", "close");

	std::cout << "GET A GERER\n";
	// pour l instant on check les uri et on renvoie un fichier ou un message d erreur, mais plus tard il faudra gerer les redirections, les fichiers statiques, les fichiers dynamiques, etc.

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
