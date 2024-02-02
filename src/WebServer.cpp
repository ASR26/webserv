/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 12:15:48 by ysmeding          #+#    #+#             */
/*   Updated: 2024/02/02 07:26:05 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

WebServer::WebServer()
{
	return ;
}

WebServer::WebServer(const WebServer& webser): servers(webser.servers), acceptedaddrinfo(webser.acceptedaddrinfo),\
serverSocket_acc(webser.serverSocket_acc), acceptedaddrinfo_size(webser.acceptedaddrinfo_size), \
requestQueue(webser.requestQueue), socketCount(webser.socketCount), eventCount(webser.eventCount),\
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
		this->socketCount = webser.socketCount;
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
	//std::cout << "Insert request for fd: " << fd << std::endl;
	//std::cout << requestQueue.find(fd)->second.fd << std::endl;
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
		throw std::runtime_error("Error: Configuration file must be a .conf file");
	fd.open (conf, std::fstream::in);
	if (fd.is_open())
		while (std::getline(fd, str))
		{
			file += str;
			file.push_back('\n');
		}
	else
		throw std::runtime_error("Error: Configuration file could not be opened.");
	n = file.find("server");
	while (n != std::string::npos)
	{
		int	i = n + 5;
		while (file[++i] && file[i] != '{')
			if (file[i] != ' ' && file[i] != '\t')
				throw std::runtime_error("Error: Invalid configuration file.");
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
			{
				throw std::runtime_error("Error: Non existent root " + full_root);
			}
			stat(full_root.c_str(), &buf);
			if (!S_ISDIR(buf.st_mode))
			{
				throw std::runtime_error("Error: Root not a directory");
			}
		}
		if (!servers[i].getUpload().empty())
		{
			full_upload = "." + servers[i].getRoot() + servers[i].getUpload();
			if (access(full_upload.c_str(), F_OK))
			{
				throw std::runtime_error("Error: Non existent upload " + full_upload);
			}
			stat(full_upload.c_str(), &buf);
			if (!S_ISDIR(buf.st_mode))
			{
				throw std::runtime_error("Error: Upload not a directory");
			}
		}
		for (unsigned int j = 0; j < servers[i].getLocations().size(); j++)
		{
			servers[i].getLocations()[j].completeRoot(servers[i].getRoot());
			if (!servers[i].getLocations()[j].getRoot().empty())
			{
				full_root = "." + servers[i].getLocations()[j].getRoot();
				if (access(full_root.c_str(), F_OK))
				{
					throw std::runtime_error("Error: Non existent root " + full_root);
				}
				stat(full_root.c_str(), &buf);
				if (!S_ISDIR(buf.st_mode))
				{
					throw std::runtime_error("Error: Root not a directory");
				}
			}
			if (!servers[i].getLocations()[j].getUpload().empty())
			{
				full_upload = "." + servers[i].getLocations()[j].getRoot() + servers[i].getLocations()[j].getUpload();
				if (access(full_upload.c_str(), F_OK))
				{
					throw std::runtime_error("Error: Non existent upload " + full_upload);
				}
				stat(full_upload.c_str(), &buf);
				if (!S_ISDIR(buf.st_mode))
				{
					throw std::runtime_error("Error: Upload not a directory");
				}
			}
			if (!servers[i].getLocations()[j].getRedirpath().empty())
			{
				if (servers[i].getLocations()[j].getRedircode() != "301" && servers[i].getLocations()[j].getRedircode() != "303" && servers[i].getLocations()[j].getRedircode() != "307")
				{
					throw std::runtime_error("Error: Invalid return code");
				}
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
				{
					close(its->second);
				}
				throw std::runtime_error(e.what());
			}
			port_sock[servers[i].getPort()] = servers[i].getServerSocket();
		}
	}
	
}

void WebServer::assignServerToRequest(class Request &req)
{
	//function needs to be check with properly configured WebServ::servers
	std::vector<int> index;
	
	std::string header = req.getHeader();
	unsigned long pos_start = header.find("Host: ");
	unsigned long pos_end = header.substr(pos_start, std::string::npos).find("\r\n");
	req.setHost(header.substr(pos_start + std::strlen("Host: "), pos_end - std::strlen("Host: ")));
	//this->host = header.substr(pos_start + std::strlen("Host: "), pos_end - std::strlen("Host: "));
	if (req.getHost().find(":") == std::string::npos)
	{
		req.formErrorResponse(400);
		return ;
	}
	std::string port = req.getHost().substr(req.getHost().find(":") + 1, std::string::npos);
	for (unsigned long i = 0; i < servers.size(); i++)
	{
		//std::cout << "port is: " << port << " server port is: " << servers[i].getPort() << std::endl;
		if (port == servers[i].getPort())
			index.push_back(i);
	}
	//std::cout << index.size() << std::endl;
	std::string server_name;
	pos_end = req.getHost().rfind(":");
	//std::cout << "host: " << req.getHost()[pos_end - 1] << std::endl;
	if (!std::isdigit(req.getHost()[pos_end - 1]))
	{
		pos_end = req.getHost().find(".localhost");
		if (pos_end != std::string::npos)
		//std::cout << "pos end -> " << pos_end << std::endl;
		//std::cout << req.getHost() << std::endl;
			server_name = req.getHost().substr(0, pos_end);
		//std::cout << server_name << std::endl;
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
	//std::cout << server_name << std::endl;
	if (server_name.empty())
	{
		req.setServer(servers[index[0]]);
	}
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
		{
			req.formErrorResponse(400);
		}
	}
	
}

/* void WebServer::assignServerToRequest(class Request &req)
{
	std::vector<int> index;
	std::string server_name;
	std::string header = req.getHeader();
	unsigned long pos_start = header.find("Host: ");
	unsigned long pos_end = header.substr(pos_start, std::string::npos).find("\r\n");
	req.setHost(header.substr(pos_start + std::strlen("Host: "), pos_end - std::strlen("Host: ")));
	if (req.getHost().find(":") == std::string::npos)
	{
		if (req.getHost().find(".") == std::string::npos)
			server_name = req.getHost();
		else
		{
			pos_end = req.getHost().find(".");
			if (pos_end != std::string::npos)
				server_name = req.getHost().substr(0, pos_end);
		}
		for (unsigned long i = 0; i < servers.size(); i++)
		{
			if (std::find(servers[i].getSName().begin(), servers[i].getSName().end(), server_name) != servers[i].getSName().end())
			{
				req.setServer(servers[i]);
				return ;
			}
		}
		req.formErrorResponse(400);
		return ;
	}
	std::string port = req.getHost().substr(req.getHost().find(":") + 1, std::string::npos);
	for (unsigned long i = 0; i < servers.size(); i++)
	{
		if (port == servers[i].getPort())
			index.push_back(i);
	}
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
	
} */

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
		throw std::runtime_error("Error: server did not accept client socket");
	std::cout << "client socket: " << serverSocket_acc << std::endl;
	int option_val = 1;
	if (setsockopt(serverSocket_acc, SOL_SOCKET, SO_REUSEADDR, &option_val, sizeof(option_val)) == -1)
	{
		throw std::runtime_error("Error: socket option");
	}
	fcntl(serverSocket_acc, F_SETFD , O_NONBLOCK, FD_CLOEXEC);
	int i = 0;
	while (sockets[i].fd >= 0)
		i++;
	sockets[i].fd = serverSocket_acc;
	sockets[i].events = POLLIN;
	eventCount++;
	socketCount++;
	if (socketCount == 1000)
	{
		throw std::runtime_error("Too many requests...");
	}
}

void WebServer::readFromSocket(int fd, int i)
{
	std::cout << std::endl << "receiving request from " << fd << std::endl;
	if (!existRequest(fd))
	{
		//std::cout << "NEW RQUEST ADDED TO QUEUE" << std::endl;
		this->addRequest(fd);
	}
	std::cout << "requestQueue size after adding: " << requestQueue.size() << std::endl;
	requestQueue.find(fd)->second.readRequest(this->servers);
	if (requestQueue.find(fd)->second.error)
	{
		throw std::runtime_error("Error: reading from client socket");
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
	if (requestQueue.find(fd)->second.error)
	{
		throw std::runtime_error("Error: writing to socket");
	}
	if (requestQueue.find(fd)->second.done_write)
	{
		requestQueue.erase(requestQueue.find(fd));
		sockets[i].fd = -1;
		sockets[i].events = POLLOUT;
		sockets[i].revents = 0;
		if (close(fd) == -1)
			throw std::runtime_error("Error: close");
		//std::cout << std::endl << "end of exchange" << std::endl;
		socketCount--;
	}
	eventCount++;
}

void WebServer::runWebserv()
{
	int i = 0;
	socketCount = 0;
	eventCount = 0;
	finishedEvents = 0;

	for (int j = 0; j < 1000; j++)
	{
		sockets[j].fd = -1;
		sockets[j].events = 0;
		sockets[j].revents = 0;
 	}
	for (int j = 0; j < (int)serverSocketFD.size(); j++)
 	{
 		sockets[j].fd = serverSocketFD[j];
		sockets[j].events = POLLIN;
		socketCount++; 
 	}
	while (1)
	{
		finishedEvents = poll(sockets, socketCount, 10);
		if (finishedEvents < 0)
			throw std::runtime_error("Error: poll");
		else if (finishedEvents == 0)
			continue;
		else
		{
			eventCount = 0;
			i = 0;
			while (i < 1000 && eventCount < finishedEvents)
			{
				if (sockets[i].fd >= 0 && isServerSocket(sockets[i].fd) && sockets[i].revents == POLLIN)
				{
					acceptConnection(sockets[i].fd);
				}
				else if (sockets[i].fd >= 0 && sockets[i].revents == POLLIN)
				{
					readFromSocket(sockets[i].fd, i);
				}
				else if (sockets[i].fd >= 0 && sockets[i].revents == POLLOUT)
				{
					writeToSocket(sockets[i].fd, i);
				}
				i++;
			}
		}
	}
}

void WebServer::addTestServer()
{
	servers.push_back(Server());
	servers[0].openServerSocket();
}