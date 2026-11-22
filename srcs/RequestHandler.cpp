
/* HPP headers*/
#include "../headers/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "../headers/RequestHandler.hpp"
#include <climits>
#include <cstdlib>
#include <sys/stat.h>

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
	response.setHeader("Content-Type", "application/octet-stream"); // Default MIME type
}

/* Check if the candidate path is inside the root directory */
bool RequestHandler::isInsideRoot(const std::string& root, const std::string& candidatePath)
{
    char realRoot[PATH_MAX];
    char realCand[PATH_MAX];

    if (realpath(root.c_str(), realRoot) == NULL)
        return false;
    if (realpath(candidatePath.c_str(), realCand) == NULL)
        return false;

    std::string rRoot(realRoot);
    std::string rCand(realCand);

    if (rCand.compare(0, rRoot.size(), rRoot) != 0)
        return false;

    return (rCand.size() == rRoot.size()) || (rCand[rRoot.size()] == '/');
}

/* find the longest matching location for the given URI */
static const LocationConfig *findLocation(const Config &config, const std::string &uri)
{
    const std::vector<LocationConfig> &locs = config.getLocations();
    const LocationConfig *best = NULL;

    for (size_t i = 0; i < locs.size(); ++i)
    {
        const std::string &p = locs[i].getPath();
        if (uri.compare(0, p.size(), p) == 0)
        {
            if (p == "/" || uri.size() == p.size() || uri[p.size()] == '/')
            {
                if (best == NULL || p.size() > best->getPath().size())
                    best = &locs[i];
            }
        }
    }
    return best;
}

/* Check if the request method is allowed for the given location */
static bool methodAllowed(const LocationConfig *loc, const std::string &method)
{
    if (!loc)
        return false;

    const std::vector<std::string> &methods = loc->getAllowedMethods();
    for (size_t i = 0; i < methods.size(); ++i)
    {
        if (methods[i] == method)
            return true;
    }
    return false;
}

/* Process the client's request and prepare the HTTP response */
Response RequestHandler::process(const Request &request, const Config &config)
{
	Response response;

	const LocationConfig *loc = findLocation(config, request.getUri());

	if (!methodAllowed(loc, request.getMethod()))
		return handleMethodNotAllowed(response);
	// std::cout << request.getUri() << std::endl;
	std::string filePath =  config.getRoot() + request.getUri();
	setMimeType(response, request.getUri());

    if (request.getMethod() == "GET")
        return handleGet(response, filePath, config);

    if (request.getMethod() == "POST")
        return handlePost(response, request, filePath, config);

    if (request.getMethod() == "DELETE")
        return handleDelete(response, filePath, config);

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

    response.setBody("405 Method Not Allowed\n");

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
Response RequestHandler::handleGet(Response response, std::string filePath, const Config &config)
{
	if (!isInsideRoot(config.getRoot(), filePath))
	{
		response.setStatusCode(HTTP_FORBIDDEN);
		response.setHeader("Content-Type", "text/plain");
		response.setHeader("Connection", "close");
		response.setBody("403 Forbidden\n");
		return response;
	}
	std::cout << filePath << std::endl; // <<<--- debug
	if (!filePath.empty() && filePath[filePath.length() - 1] == '/') {
		filePath += "index.html"; // Serve index.html for directory requests
		response.setHeader("Content-Type", "text/html");
	}

	std::ifstream file(filePath.c_str());
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
Response RequestHandler::handlePost(Response response, const Request &request, std::string filePath, const Config &config)
{
    (void) filePath;

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
Response RequestHandler::handleDelete(Response response, std::string filePath, const Config &config)
{
    struct stat st;

    if (!isInsideRoot(config.getRoot(), filePath) || S_ISDIR(st.st_mode))
	{
		response.setStatusCode(HTTP_FORBIDDEN);
		response.setHeader("Content-Type", "text/plain");
		response.setHeader("Connection", "close");
		response.setBody("403 Forbidden\n");
		return response;
    }

    if (stat(filePath.c_str(), &st) != 0)
	{
		response.setStatusCode(HTTP_NOT_FOUND);
		response.setHeader("Content-Type", "text/plain");
		response.setHeader("Connection", "close");
		response.setBody("404 Not Found\n");
		return response;
    }

    if (std::remove(filePath.c_str()) != 0)
	{
		response.setStatusCode(HTTP_INTERNAL_SERVER_ERROR);
		response.setHeader("Content-Type", "text/plain");
		response.setHeader("Connection", "close");
		response.setBody("500 Internal Server Error\n");
		return response;
    }

	response.setStatusCode(HTTP_OK);
	response.setHeader("Content-Type", "text/plain");
	response.setHeader("Connection", "close");
	response.setBody("File deleted successfully\n");

    return response;
}
