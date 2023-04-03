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
		std::cout << "1\n";
		std::cout << req << std::endl;
		int index = findServer(servers);
		_server = servers[index];
		_server.setActivePath(_req.getPath());
		std::cout << "2\n";
		// Metodo no permitido
		if (!_server.isMethodAllow(req.getMethod()))
		{
			// TODO buscar en el config el archivo de error
			std::string errorPath = Strings::trim(_server.getErrorPageByStatus(405), "/");
			_res.status(405).sendError(errorPath);
			// _res.notAllowed();
			std::cout << "3\n";
			return;
		}

		std::cout << "4\n";
		// Comprobar si hay rewrite
		std::string rewrite = _server.getRewrite();
		if (!rewrite.empty())
		{
			std::cout << "5\n";
			res.redirect(rewrite);
			return;
		}

		// Comprobar si excede el limite de body 413
		std::cout << "6\n";
		long maxBodySize = _server.getBodyMaxSize();
		if (maxBodySize > 0)
		{
			try
			{
				std::cout << "7\n";
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

		std::cout << "8\n";
		// Comprobar si hay un file
		if (_req.getFiles().size() > 0)
		{
			std::string pathUpload = _server.getUploadPath();
			std::map<std::string, fileStruct> files = _req.getFiles();
			std::map<std::string, fileStruct>::iterator it = files.begin();
			for (; it != files.end(); it++)
			{
				std::cout << "9\n";
				File tmpFile((*it).second.name);
				tmpFile.move(pathUpload + "/" + (*it).second.name);
			}

			// MOVER EL ARCHIVO A OTRO SITIO
		}

		std::cout << "10\n";
		std::string path =_server.getPathFolder(_req.getPath());

		std::cout << "FIRST: " << path << std::endl;

		std::string ext = File::getExtension(path);
		if (ext.empty())
		{
			std::cout << "11\n";
			// Comprobar si tiene autoindex
			if (_server.haveAutoindex())
			{
				try
				{
					_res.autoindex(path, _req.getPath());
					std::cout << "12\n";
					return;
				}
				catch (const std::exception &e)
				{
					// NOT FOUND
					std::cout << "NOT FPOUNDDD\n";
					std::string errorPath = Strings::trim(_server.getErrorPageByStatus(404), "/");
					_res.status(404).sendError(errorPath);

					// _res.notFound(errorPath);
					std::cout << "13\n";
					return;
				}
			}
			// Comprobar si no tiene extension buscar en el index index
			std::cout << "14\n";
			std::vector<std::string> index = _server.findIndex();
			for (size_t i = 0; i < index.size(); i++)
			{
				if (Dirs::existDir(path + "/" + index[i]))
				{
					path += "/" + index[i];
					std::cout << "15\n";
					break;
				}
			}
		}
		ext = File::getExtension(path);

		// Si definitivamente no tiene extension (COPIADO DE ARRIBA REFACTORIZAR)
		std::string errorPath = Strings::trim(_server.getErrorPageByStatus(404), "/");
		std::cout << "16\n";
		if (ext.empty())
		{
			std::cout << "EMPTY\n";
			_res.status(404).sendError(errorPath);
			// _res.notFound(errorPath);
			std::cout << "17\n";
			return;
		}

		// Comprobar que exista el archivo
		std::cout << "path is: " << path << "\n";
		if (!Dirs::existDir(path))
		{
			std::cout << "EL DIR NO EXISTE\n";
			_res.notFound(errorPath);
			return;
		}

		// Comprobar si la extension es la de algun cgi


			// TODO mirar si la extension coincide con algun cgi y si coincide, llamar a cgi


		// Enviar el file si es html
		std::cout << "PATH " << path << std::endl;
		std::cout << "20\n";
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

	std::cout << "22\n";
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
