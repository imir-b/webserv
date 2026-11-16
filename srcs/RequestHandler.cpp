
/* HPP headers*/
#include "Config.hpp"
#include "RequestHandler.hpp"

/* Canonical orthodox form */

RequestHandler::RequestHandler(void) {
}

RequestHandler::RequestHandler(const RequestHandler &requestHandler) {
    (void) requestHandler;
}

RequestHandler &RequestHandler::operator=(const RequestHandler &requestHandler) {
    (void) requestHandler;
    return *this;
}

RequestHandler::~RequestHandler(void) {
}

/* Process the client's request and prepare the HTTP response */
Response RequestHandler::process(const Request &request, const Config &config) {
    if (request.getMethod() == "GET")
        return handleGet(request, config);

    if (request.getMethod() == "POST")
        return handlePost(request, config);

    if (request.getMethod() == "DELETE")
        return handleDelete(request, config);

    return handleMethodNotAllowed();
}

/* Handle errors and prepare the HTTP response */
Response RequestHandler::processError(int status) {
    switch (status) {
        case HTTP_BAD_REQUEST:
            return handleBadRequest();

        case HTTP_FORBIDDEN:
            return handleForbidden();

        case HTTP_NOT_FOUND:
            return handleNotFound();

        case HTTP_METHOD_NOT_ALLOWED:
            return handleMethodNotAllowed();

        case HTTP_PAYLOAD_TOO_LARGE:
            return handlePayloadTooLarge();

        case HTTP_INTERNAL_SERVER_ERROR:
            return handleInternalServerError();

        case HTTP_NOT_IMPLEMENTED:
            return handleNotImplemented();

        default:
            return handleInternalServerError();
    }
}

Response RequestHandler::handleBadRequest(void) {
    Response response;

    response.setStatusCode(HTTP_BAD_REQUEST);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");
    response.setBody("Bad Request\n");

    return response;
}

/* Handle unsupported HTTP method */
Response RequestHandler::handleMethodNotAllowed(void) {
    Response response;

    response.setStatusCode(HTTP_METHOD_NOT_ALLOWED);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");
    response.setHeader("Allow", "GET, POST, DELETE");

    response.setBody("Method Not Allowed\n");

    return response;
}

/* Handle 413 Content Too Large error */
Response RequestHandler::handlePayloadTooLarge(void) {
    Response response;

    response.setStatusCode(HTTP_PAYLOAD_TOO_LARGE);
    response.setBody("Content Too Large\n");
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");

    return response;
}

/* Handle 403 Forbidden error */
Response RequestHandler::handleForbidden(void) {
    Response response;

    response.setStatusCode(HTTP_FORBIDDEN);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");
    response.setBody("Forbidden\n");

    return response;
}

/* Handle 404 Not Found error */
Response RequestHandler::handleNotFound(void) {
    Response response;

    response.setStatusCode(HTTP_NOT_FOUND);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");
    response.setBody("Not Found\n");

    return response;
}

/* Handle 500 Internal Server Error */
Response RequestHandler::handleInternalServerError(void) {
    Response response;

    response.setStatusCode(HTTP_INTERNAL_SERVER_ERROR);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");
    response.setBody("Internal Server Error\n");

    return response;
}

/* Handle 501 Not Implemented error */
Response RequestHandler::handleNotImplemented(void) {
    Response response;

    response.setStatusCode(HTTP_NOT_IMPLEMENTED);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");
    response.setBody("Not Implemented\n");

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
    (void)config;

    if (request.getUri() == "/") {
        Response response;

        response.setStatusCode(HTTP_OK);
        response.setHeader("Content-Type", "text/plain");
        response.setHeader("Connection", "close");
        response.setBody("Hello World!\n");

        return response;
    }

    return processError(HTTP_NOT_FOUND);
}

/* Handle POST request */
Response RequestHandler::handlePost(const Request &request, const Config &config) {
    if (request.getBody().size() > config.getClientMaxBodySize())
        return processError(HTTP_PAYLOAD_TOO_LARGE);

    Response response;
    response.setStatusCode(HTTP_OK);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");
    response.setBody("POST received:\n" + request.getBody());

    return response;
}

/* Handle DELETE request */
Response RequestHandler::handleDelete(const Request &request, const Config &config) {
    Response response;
    (void)request;
    (void)config;

    response.setStatusCode(HTTP_NOT_IMPLEMENTED);
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");

    response.setBody("DELETE is not implemented yet\n");

    return response;
}
