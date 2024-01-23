/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 12:15:48 by ysmeding          #+#    #+#             */
/*   Updated: 2024/01/23 11:14:12 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

WebServer::WebServer()
{
	return ;
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
			req.setResponse("HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 11\r\n\r\nbad request");//change to error400.html
		}
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

void WebServer::runWebserv()
{
	//int c;
	int k;
	//int r;
	struct kevent new_event;
	struct kevent finished_event;
	int events;
	int kq = kqueue();
	
	if (kq == -1)
	{
		std::cout << std::strerror(errno) << std::endl;
		//throw std::runtime_error("Error: kqueue"); 
	}

	/* Do this in for loop for all servers */
	//std::cout << servers.size() << std::endl;
	for (int i = 0; i < (int)servers.size(); i++)
	{
		EV_SET(&new_event, servers[i].getServerSocket(), EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
		k = kevent(kq, &new_event, 1, NULL, 0, NULL);
		/*if (k == -1)
			Error::functionError();*/
	}
	acceptedaddrinfo_size = sizeof(struct sockaddr_storage);
	while (1)
	{
		events = kevent(kq, NULL, 0, &finished_event, 1, NULL);
		/*if (events == -1)
			Error::functionError();*/
		if (events == 0)
			continue;
		if (finished_event.flags == EV_EOF)
		{
			//std::cout << std::endl << "closing connection" << std::endl;
			close(finished_event.ident);
		}
		else if (isServerSocket(finished_event.ident))
		{
			//std::cout << std::endl << "accept connection" << std::endl;
			if ((serverSocket_acc = accept(finished_event.ident, (struct sockaddr *)&acceptedaddrinfo, &acceptedaddrinfo_size)) == -1)
				Error::functionError();
			fcntl(serverSocket_acc, F_SETFD , O_NONBLOCK, FD_CLOEXEC);
			EV_SET(&new_event, serverSocket_acc, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			kevent(kq, &new_event, 1, NULL, 0, NULL);
		}
		else if (finished_event.filter == EVFILT_READ)
		{
			//std::cout << std::endl << "receiving request" << std::endl;
			if (!existRequest(finished_event.ident))
				this->addRequest(finished_event.ident);
			requestQueue.find(finished_event.ident)->second.readRequest(this->servers);
			if (requestQueue.find(finished_event.ident)->second.done_read)
			{
				assignServerToRequest(requestQueue.find(finished_event.ident)->second);
				EV_SET(&new_event, finished_event.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
				kevent(kq, &new_event, 1, NULL, 0, NULL);
				EV_SET(&finished_event, serverSocket_acc,  EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, NULL);
				kevent(kq, &finished_event, 1, NULL, 0, NULL);
			}
		}
		else if (finished_event.filter == EVFILT_WRITE)
		{
			//std::cout << std::endl << "sending response" << std::endl;
			
			requestQueue.find(finished_event.ident)->second.sendResponse();
			if (requestQueue.find(finished_event.ident)->second.done_write)
			{
				requestQueue.erase(requestQueue.find(finished_event.ident));
				EV_SET(&finished_event, finished_event.ident,  EVFILT_READ, EV_DELETE, 0, 0, NULL);
				kevent(kq, &finished_event, 1, NULL, 0, NULL);
				EV_SET(&finished_event, finished_event.ident,  EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
				kevent(kq, &finished_event, 1, NULL, 0, NULL);
				close(finished_event.ident);
			}
		}
	}
}


/* int fd = open("tab.html", O_RDONLY);
char buff2[10000];
r = read(fd, buff2, 10000);
close(fd);
write(finished_event.ident, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 449\r\n\r\n", strlen( "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 449\r\n\r\n"));
write(finished_event.ident, buff2, r); */


void WebServer::addTestServer()
{
	servers.push_back(Server());
	servers[0].openServerSocket();
}