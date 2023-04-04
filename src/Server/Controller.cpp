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
		std::cout << req << std::endl;
		int index = findServer(servers);
		_server = servers[index];
		_server.setActivePath(_req.getPath());
		// Metodo no permitido
		if (!_server.isMethodAllow(req.getMethod()))
		{
			// TODO buscar en el config el archivo de error
			std::string errorPath = Strings::trim(_server.getErrorPageByStatus(405), "/");
			_res.status(405).sendError(errorPath);
			// _res.notAllowed();
			return;
		}

		// Comprobar si hay rewrite
		std::string rewrite = _server.getRewrite();
		if (!rewrite.empty())
		{
			std::cout << "5\n";
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
					// getErrorPageByStatus 413
					std::string errorPath = Strings::trim(_server.getErrorPageByStatus(413), "/");
					_res.status(413).sendError(errorPath);
					// res.limitExced();
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
				tmpFile.move(pathUpload + "/" + (*it).second.name);
			}

			// MOVER EL ARCHIVO A OTRO SITIO
		}

		std::string path =_server.getPathFolder(_req.getPath());

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
					_res.status(404).sendError(errorPath);

					// _res.notFound(errorPath);
					return;
				}
			}
			// Comprobar si no tiene extension buscar en el index index
			std::vector<std::string> index = _server.findIndex();
			for (size_t i = 0; i < index.size(); i++)
			{
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
			_res.status(404).sendError(errorPath);
			// _res.notFound(errorPath);
			return;
		}

		// Comprobar que exista el archivo
		if (!Dirs::existDir(path))
		{
			_res.notFound(errorPath);
			return;
		}

		// Comprobar si la extension es la de algun cgi


			// TODO mirar si la extension coincide con algun cgi y si coincide, llamar a cgi


		// Enviar el file si es html
		if (ext == "html" || ext == "png" || ext == "css")
			_res.status(200).sendFile(path);
		// Atachment si es otro tipo
		else
		{
			std::cout << "21\n";
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
	throw std::invalid_argument("Server not found");
}

const char *Controller::ServerException::what() const throw()
{
	return "Server error";
}
