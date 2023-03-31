#include "Controller.hpp"
#include "Config.hpp"
#include "Utils/Colors.hpp"
#include "Utils/Structs.hpp"
#include "Utils/File.hpp"
#include "Utils/Strings.hpp"
#include "Utils/Dirs.hpp"

Controller::Controller(std::vector<Server> servers, Request req, Response res) : _req(req), _res(res)
{
	try
	{
		std::cout << GRN "CONTROLLER\n" RESET;
		std::cout << req << std::endl;
		int index = findServer(servers);
		_server = servers[index];
		_server.setActivePath(_req.getPath());
		if (!_server.isMethodAllow(req.getMethod()))
		{
			_res.notAllowed();
			return;
		}

		// Comprobar si hay rewrite
		std::string rewrite = _server.getRewrite();
		std::cout << BLU "redir: " << rewrite << "\n" RESET;

		if (!rewrite.empty())
		{
			res.redirect(rewrite);
			return;
		}

		// Comprobar si excede el limite de body 413
		long maxBodySize = _server.getBodyMaxSize();
		if (maxBodySize > 0)
		{
			try
			{
				int limit = std::stoi(_req.getHeaders().at("Content-Length"));
				// TODO OBTENER PAGINAS DE ERROR
				if (limit > maxBodySize)
				{
					res.limitExced();
					return;
				}
			}
			catch (const std::exception &e)
			{
			}
		}

		// Comprobar si hay un file
		if (_req.getFiles().size() > 0)
		{
			std::string pathUpload = _server.getUploadPath();
			std::map<std::string, fileStruct> files = _req.getFiles();
			std::map<std::string, fileStruct>::iterator it = files.begin();
			for (; it != files.end(); it++)
			{
				File tmpFile((*it).second.name);
				std::cout << (*it).second.name << std::endl;
				std::cout << pathUpload + "/" + (*it).second.name << std::endl;
				tmpFile.move(pathUpload + "/" + (*it).second.name);
			}

			// MOVER EL ARCHIVO A OTRO SITIO
		}

		std::string path = _server.getPathFolder(_req.getPath());
		std::string ext = File::getExtension(path);
		if (ext.empty())
		{
			// Comprobar si tiene autoindex
			if (_server.haveAutoindex())
			{
				try
				{
					_res.autoindex(path, _req.getPath());
					return;
				}
				catch (const std::exception &e)
				{
					// NOT FOUND
					std::string errorPath = Strings::trim(_server.getErrorPageByStatus(404), "/");
					std::cout << RED "ERROR: " << errorPath << std::endl;
					_res.notFound(errorPath);
					return;
				}
			}
			// Comprobar si no tiene extension buscar en el index index
			std::vector<std::string> index = _server.findIndex();
			std::cout << GRN "INDEX\n";
			for (size_t i = 0; i < index.size(); i++)
			{
				std::cout << path << "/" << index[i] << std::endl;
				if (Dirs::existDir(path + "/" + index[i]))
				{
					path += "/" + index[i];
					break;
				}
			}
		}
		ext = File::getExtension(path);

		// Si definitivamente no tiene extension (COPIADO DE ARRIBA REFACTORIZAR)
		std::string errorPath = Strings::trim(_server.getErrorPageByStatus(404), "/");
		if (ext.empty())
		{
			_res.notFound(errorPath);
			return;
		}

		// Comprobar que exista el archivo
		if (!Dirs::existDir(path))
		{
			_res.notFound(errorPath);
			return;
		}

		// Comprobar si la extension es la de algun cgi

		// Enviar el file si es html
		if (ext == "html" || ext == "png" || ext == "css")
			_res.sendFile(path);
		// Atachment si es otro tipo
		else
		{
			_res.attachment(path);
		}
	}
	catch (const std::exception &e)
	{
		// SERVER ERROR
		std::cerr << e.what() << '\n';
	}
}

Controller::~Controller()
{
}

int Controller::findServer(std::vector<Server> servers)
{
	std::vector<Server>::iterator it = servers.begin();

	for (; it != servers.end(); it++)
	{
		std::vector<int> ports = (*it).getPorts();
		for (size_t i = 0; i < ports.size(); i++)
		{
			if (ports[i] == _req.getPort())
				return i;
		}
	}
	throw std::invalid_argument("AddPositiveIntegers arguments must be positive");
	// return -1;
}

const char *Controller::ServerException::what() const throw()
{
	return "Server error";
}
