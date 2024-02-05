/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 12:15:48 by ysmeding          #+#    #+#             */
/*   Updated: 2024/02/05 08:20:31 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

WebServer::WebServer()
{
	return ;
}

WebServer::WebServer(const WebServer& webser): servers(webser.servers), acceptedaddrinfo(webser.acceptedaddrinfo),\
serverSocket_acc(webser.serverSocket_acc), acceptedaddrinfo_size(webser.acceptedaddrinfo_size), \
requestQueue(webser.requestQueue), eventCount(webser.eventCount),\
finishedEvents(webser.finishedEvents)
{
	return ;
}

WebServer &WebServer::operator=(const WebServer & webser)
{
	if (this != &webser)
	{
		this->servers = webser.servers;
		this->acceptedaddrinfo = webser.acceptedaddrinfo;
		this->serverSocket_acc = webser.serverSocket_acc;
		this->acceptedaddrinfo_size = webser.acceptedaddrinfo_size;
		this->requestQueue = webser.requestQueue;
		this->eventCount = webser.eventCount;
		this->finishedEvents = webser.finishedEvents;
	}
	return *this;
}

WebServer::~WebServer()
{
	return ;
}

void WebServer::addRequest(int fd)
{
	requestQueue.insert(std::pair<int, class Request>(fd, Request(fd)));
	return ;
}

int WebServer::existRequest(int fd)
{
	if (requestQueue.find(fd) == requestQueue.end())
		return 0;
	return 1;
}

void WebServer::addServer(std::string conf)
{
	std::string::size_type n;
	std::fstream fd;
	std::string str;
	std::string file;

	n = conf.find(".conf");
	if (std::string::npos == n)
		throw std::runtime_error("\033[1;31mError\033[0m: Configuration file must be a .conf file");
	fd.open (conf, std::fstream::in);
	if (fd.is_open())
		while (std::getline(fd, str))
		{
			file += str;
			file.push_back('\n');
		}
	else
		throw std::runtime_error("\033[1;31mError\033[0m: Configuration file could not be opened.");
	n = file.find("server");
	while (n != std::string::npos)
	{
		int	i = n + 5;
		while (file[++i] && file[i] != '{')
			if (file[i] != ' ' && file[i] != '\t')
				throw std::runtime_error("\033[1;31mError\033[0m: Invalid configuration file.");
		i = n;
		while (file[i] && file[i] != '{')
			i++;
		int cont = 1;
		while (file[++i] && cont != 0)
			if (file[i] == '{')
				cont++;
			else if (file[i] == '}')
				cont--;
		servers.push_back(Server(file.substr(n, i - n)));
		n = file.find("server", i);
	}
	fd.close();
}

void WebServer::checkServerSpecification()
{
	std::string full_root;
	std::string full_upload;
	struct stat buf;
	for (unsigned int i = 0; i < servers.size(); i++)
	{
		if (!servers[i].getRoot().empty())
		{
			full_root = "." + servers[i].getRoot();
			if (access(full_root.c_str(), F_OK))
				throw std::runtime_error("\033[1;31mError\033[0m: Inaccessible root " + full_root);
			stat(full_root.c_str(), &buf);
			if (!S_ISDIR(buf.st_mode))
				throw std::runtime_error("\033[1;31mError\033[0m: Root not a directory");
		}
		if (!servers[i].getUpload().empty())
		{
			full_upload = "." + servers[i].getRoot() + servers[i].getUpload();
			if (access(full_upload.c_str(), F_OK))
				throw std::runtime_error("\033[1;31mError\033[0m: Inaccessible upload " + full_upload);
			stat(full_upload.c_str(), &buf);
			if (!S_ISDIR(buf.st_mode))
				throw std::runtime_error("\033[1;31mError\033[0m: Upload not a directory");
		}
		for (unsigned int j = 0; j < servers[i].getLocations().size(); j++)
		{
			servers[i].getLocations()[j].completeRoot(servers[i].getRoot());
			if (!servers[i].getLocations()[j].getRoot().empty())
			{
				full_root = "." + servers[i].getLocations()[j].getRoot();
				if (access(full_root.c_str(), F_OK))
					throw std::runtime_error("\033[1;31mError\033[0m: Inaccessible root " + full_root);
				stat(full_root.c_str(), &buf);
				if (!S_ISDIR(buf.st_mode))
					throw std::runtime_error("\033[1;31mError\033[0m: Root not a directory");
			}
			if (!servers[i].getLocations()[j].getUpload().empty())
			{
				full_upload = "." + servers[i].getLocations()[j].getRoot() + servers[i].getLocations()[j].getUpload();
				if (access(full_upload.c_str(), F_OK))
					throw std::runtime_error("\033[1;31mError\033[0m: Inaccessible upload " + full_upload);
				stat(full_upload.c_str(), &buf);
				if (!S_ISDIR(buf.st_mode))
					throw std::runtime_error("\033[1;31mError\033[0m: Upload not a directory");
			}
			if (!servers[i].getLocations()[j].getRedirpath().empty())
			{
				if (servers[i].getLocations()[j].getRedircode() != "301" && servers[i].getLocations()[j].getRedircode() != "303" \
				&& servers[i].getLocations()[j].getRedircode() != "307")
					throw std::runtime_error("\033[1;31mError\033[0m: Invalid return code");
			}
		}
	}

}

void WebServer::configureServer()
{
	int server_vec_size = servers.size();
	int nbr_port;
	int n;
	Server tmp;
	std::string tmp_port;
	int tmp_index;
	std::map<std::string, int> port_sock;
	std::map<std::string, int>::iterator it;
	n = 0;
	for (int i = 0; i < server_vec_size; i++)
	{
		nbr_port = servers[i + n].getPortVec().size();
		if (nbr_port > 1)
		{
			tmp_port = servers[i + n].getPort();
			tmp_index = i + n;
			for (int j = 1; j < nbr_port; j++)
			{
				tmp = Server(servers[tmp_index]);
				tmp.clearPort();
				tmp.setPort(servers[tmp_index].getPortVec()[j]);
				servers.insert(servers.begin() + tmp_index + j, tmp);
				n++;
			}
			servers[tmp_index].clearPort();
			servers[tmp_index].setPort(tmp_port);
		}
	}
	for (int i = 0; i < (int)servers.size(); i++)
	{
		it = port_sock.find(servers[i].getPort());
		if (it != port_sock.end())
		{
			servers[i].setServerSocket(it->second);
		}
		else
		{
			try
			{
				servers[i].openServerSocket();
				serverSocketFD.push_back(servers[i].getServerSocket());
			}
			catch(const std::exception& e)
			{
				for (std::map<std::string, int>::iterator its = port_sock.begin(); its != port_sock.end(); its++)
					close(its->second);
				throw std::runtime_error(e.what());
			}
			port_sock[servers[i].getPort()] = servers[i].getServerSocket();
		}
		servers[i].messageOpenServer();
	}
	
}

void WebServer::assignServerToRequest(class Request &req)
{
	std::vector<int> index;
	
	std::string header = req.getHeader();
	unsigned long pos_start = header.find("Host: ");
	unsigned long pos_end = header.substr(pos_start, std::string::npos).find("\r\n");
	req.setHost(header.substr(pos_start + std::strlen("Host: "), pos_end - std::strlen("Host: ")));
	if (req.getHost().find(":") == std::string::npos)
	{
		req.formErrorResponse(400);
		return ;
	}
	std::string port = req.getHost().substr(req.getHost().find(":") + 1, std::string::npos);
	for (unsigned long i = 0; i < servers.size(); i++)
	{
		if (port == servers[i].getPort())
			index.push_back(i);
	}
	std::string server_name;
	pos_end = req.getHost().rfind(":");
	if (!std::isdigit(req.getHost()[pos_end - 1]))
	{
		pos_end = req.getHost().find(".localhost");
		if (pos_end != std::string::npos)
			server_name = req.getHost().substr(0, pos_end);
		else
			server_name = "";
	}
	else
	{
		int n = 1;
		int point_count = 0;
		while (pos_end - n >= 0 && (std::isdigit(req.getHost()[pos_end - n]) || req.getHost()[pos_end - n] == '.') && point_count < 4)
		{
			if (req.getHost()[pos_end - n] == '.')
				point_count++;
			n++;
		}
		if (pos_end - n < 0)
			server_name = "";
		else
			server_name = req.getHost().substr(0, pos_end - n);
	}
	if (server_name.empty())
		req.setServer(servers[index[0]]);
	else
	{
		int found = 0;
		for (unsigned long i = 0; i < index.size(); i++)
		{
			for (unsigned long j = 0; j < servers[index[i]].getServerNames().size(); j++)
			{
				if (servers[index[i]].getServerNames()[j] == server_name)
				{
					req.setServer(servers[index[i]]);
					found = 1;
				}
			}
		}
		if (found == 0)
			req.formErrorResponse(400);
	}
}

bool WebServer::isServerSocket(int fd)
{
	for (int i = 0; i < (int) servers.size(); i++)
	{
		if (fd == servers[i].getServerSocket())
			return true;
	}
	return false;
}

void WebServer::acceptConnection(int fd)
{
	if ((serverSocket_acc = accept(fd, NULL, NULL)) == -1)
		throw std::runtime_error("\033[1;31mError\033[0m: server did not accept client socket");
	int option_val = 1;
	if (setsockopt(serverSocket_acc, SOL_SOCKET, SO_REUSEADDR, &option_val, sizeof(option_val)) == -1)
	{
		std::cerr << "\033[1;31mError\033[0m: setting socket options for client socket" << std::endl;
		if (close(fd) == -1)
			std::cerr << "\033[1;31mError\033[0m: closing client socket" << std::endl;
	}
	fcntl(serverSocket_acc, F_SETFD , O_NONBLOCK, FD_CLOEXEC);
	addSocket(serverSocket_acc);
	eventCount++;
}

void WebServer::readFromSocket(int fd, int i)
{
	if (!existRequest(fd))
		this->addRequest(fd);
	requestQueue.find(fd)->second.readRequest(this->servers);
	if (requestQueue.find(fd)->second.error)
	{
		requestQueue.erase(requestQueue.find(fd));
		sockets[i].fd = -1;
		if (close(fd) == -1)
			std::cerr << "\033[1;31mError\033[0m: closing client socket" << std::endl;
	}
	if (requestQueue.find(fd)->second.done_read)
	{
		assignServerToRequest(requestQueue.find(fd)->second);
		sockets[i].events = POLLOUT;
	}
	eventCount++;
}

void WebServer::writeToSocket(int fd, int i)
{
	requestQueue.find(fd)->second.sendResponse();
	if (requestQueue.find(fd)->second.done_write || requestQueue.find(fd)->second.error)
	{
		requestQueue.erase(requestQueue.find(fd));
		sockets[i].fd = -1;
		if (close(fd) == -1)
			std::cerr << "\033[1;31mError\033[0m: closing client socket" << std::endl;
	}
	eventCount++;
}

void WebServer::addSocket(int fd)
{
	struct pollfd sock;
	sock.fd = fd;
	sock.events = POLLIN;
	sockets.push_back(sock);
	return ;
}

bool WebServer::existFinishedSocket()
{
	for (unsigned int i = 0; i < sockets.size(); i++)
	{
		if (sockets[i].fd < 0)
			return true;
	}
	return false;
}

void WebServer::clearFinishedSockets()
{
	while (existFinishedSocket())
	{
		for (unsigned int i = 0; i < sockets.size(); i++)
		{
			if (sockets[i].fd < 0)
			{
				sockets.erase(sockets.begin() + i);
				break ;
			}
		}
	}
}

void WebServer::runWebserv()
{
	unsigned int i = 0;
	eventCount = 0;
	finishedEvents = 0;

	for (int j = 0; j < (int)serverSocketFD.size(); j++)
 	{
 		addSocket(serverSocketFD[j]);
 	}
	while (1)
	{
		finishedEvents = poll(&sockets[0], sockets.size(), 10);
		if (finishedEvents < 0)
			throw std::runtime_error("\033[1;31mError\033[0m: poll");
		else if (finishedEvents == 0)
			continue;
		else
		{
			eventCount = 0;
			i = 0;
			while (i < sockets.size())
			{
				/* if (!isServerSocket(sockets[i].fd) && time(NULL) > timeoutPerSocket[sockets[i].fd] + 30)
				{
					std::cout << "TIMEOUT!! " << timeoutPerSocket[sockets[i].fd] << " " << time(NULL) << std::endl;
					requestQueue.find(sockets[i].fd)->second.setResponse("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: 0");
					sockets[i].events = POLLOUT;
				} */
				if (sockets[i].fd >= 0 && isServerSocket(sockets[i].fd) && sockets[i].revents == POLLIN)
					acceptConnection(sockets[i].fd);
				if (sockets[i].fd >= 0 && !isServerSocket(sockets[i].fd) && sockets[i].revents == POLLIN)
					readFromSocket(sockets[i].fd, i);
				if (sockets[i].fd >= 0 && sockets[i].revents == POLLOUT)
					writeToSocket(sockets[i].fd, i);
				i++;
			}
			clearFinishedSockets();
		}
	}
}

void WebServer::addTestServer()
{
	servers.push_back(Server());
	servers[0].openServerSocket();
}