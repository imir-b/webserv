
/* HPP headers*/
#include "Config.hpp"
#include "RequestHandler.hpp"

/* Constructor */

RequestHandler::RequestHandler() {}

RequestHandler::~RequestHandler() {}

/* Process the client's request and prepare the HTTP response */
Response RequestHandler::process(const Request &request, const Config &config)
{
    if (request.getMethod() == "GET")
        return handleGet(request, config);

    if (request.getMethod() == "POST")
        return handlePost(request, config);

    if (request.getMethod() == "DELETE")
        return handleDelete(request, config);

    return handleMethodNotAllowed();
}

/* Handle unsupported HTTP method */
Response RequestHandler::handleMethodNotAllowed(void)
{
    Response response;

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
Response RequestHandler::handleGet(const Request &request, const Config &config)
{
    Response response;
    (void) config;

    if (request.getUri() == "/") {
        response.setStatusCode(HTTP_OK);
        response.setBody("Hello World!\n");
    }
    else {
        response.setStatusCode(HTTP_NOT_FOUND);
        response.setBody("Not Found\n");
    }

    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");

    return response;
}

/* Handle POST request */
Response RequestHandler::handlePost(const Request &request, const Config &config)
{
    Response response;
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

    return response;
}

/* Handle DELETE request */
Response RequestHandler::handleDelete(const Request &request, const Config &config)
{
    Response response;
    (void)request;
    (void)config;

    response.setStatusCode(HTTP_NOT_IMPLEMENTED);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");

    response.setBody("DELETE is not implemented yet\n");

    return response;
}
