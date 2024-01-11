/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:21:26 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/12 12:23:30 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

Server::Server()
{
	//remove all this later
	port.push_back("8080");
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	auto_index = false;
	location.push_back(LocationParser());
}

Server::Server(std::string conf)
{
	std::string::size_type	n;
	int						i = 0;
	int						j = 0;
	//struct addrinfo default_addrinfo;
	//struct addrinfo *returned_sockaddr;

	conf.erase(0, 6);
	n = conf.find("location");
	if (n != std::string::npos)
	{
		while (n != std::string::npos)
		{
			i = n + 8;
			while (conf[i] && conf[i] != '{')
				i++;
			j = 1;
			while (conf[++i] && j != 0)
				if (conf[i] == '{')
					j++;
				else if (conf[i] == '}')
					j--;  
			location.push_back(LocationParser(conf.substr(n, i - n)));
			if (conf.find("*.", n) < (unsigned long)i && conf.find("cgi_pass", n) < (unsigned long)i)
			{
				std::string cgi_file_type = trimSpaces(conf.substr(conf.find("*.", n) + 2, conf.find("{", conf.find("*.", n) + 2) - (conf.find("*.", n) + 2)));
				//std::cout << "CGI name:" << trimSpaces(conf.substr(conf.find("*.", n) + 2, conf.find("{", conf.find("*.", n) + 2) - (conf.find("*.", n) + 2))) << "<-" << std::endl;
				cgi[cgi_file_type] = location.back().getCGI();
			}
			conf.erase(n, i - n);
			n = conf.find("location");
		}
	}

	n = conf.find("listen");
	if (n == std::string::npos)
		port.push_back("80");
	else
	{
		i = n + 6;
		while (conf[i] == ' ' && conf[i])
			i++;
		n = i;
		while (conf[i] && conf[i - 1] != '\n')
		{
			if (conf[i] == ' ' || conf[i] == '\n')
			{
				port.push_back(trimSpaces(conf.substr(n, i - n)));
				n = i + 1;
			}
			i++;
		}
		conf.erase(conf.find("listen"), conf.find("\n", conf.find("listen")) - conf.find("listen"));
	}

	n = conf.find("server_name");
	if (n != std::string::npos)
	{
		i = n + 11;
		while (conf[i] == ' ' && conf[i])
			i++;
		n = i;
		while (conf[i] && conf[i - 1] != '\n')
		{
			if (conf[i] == ' ' || conf[i] == '\n')
			{
				s_name.push_back(trimSpaces(conf.substr(n, i - n)));
				n = i + 1;
			}
			i++;
		}
		conf.erase(conf.find("server_name"), conf.find("\n", conf.find("server_name")) - conf.find("server_name"));
	}

	n = conf.find("error_page");
	//if (n == std::string::npos)
	//	error[404] = "default_location";//no hace falta
	//else
	//{
		while (n != std::string::npos)
		{
			i = n;
			while (conf[i] != '\n')
				i++;
			j = i;
			while (conf[j] != ' ')
				j--;
			while (conf[n] != ' ')
				n++;
			while (std::atoi(&conf[n]))
			{
				std::string s;
				error[std::atoi(&conf[n])] = trimSpaces(conf.substr(j, i - j));
				//n += 1 + std::to_string(std::atoi(&conf[n])).length();//to_str es c++ 11!!!!!!!!!!!!!!!!!!!
				s = intToStr(std::atoi(&conf[n]));
				n += 1 + s.size();// esto NO es c++11, devuelve lo mismo que la línea comentada (debería estar bien)
			}
			conf.erase(conf.find("error_page"), conf.find("\n", conf.find("error_page")) - conf.find("error_page"));
			n = conf.find("error_page", n);
		}
	//}

	n = conf.find("client_max_body_size");
	if (n == std::string::npos)
		c_size = 1;
	else
	{
		while (conf[n] != ' ')
			n++;
		c_size = std::atoi(&conf[n]);
		conf.erase(conf.find("client_max_body_size"), conf.find("\n", conf.find("client_max_body_size")) - conf.find("client_max_body_size"));
	}

	n = conf.find("autoindex");
	if (n == std::string::npos)
		auto_index = false;
	else
	{
		i = n;
		n = conf.find("off", n + 1);
		if (n != std::string::npos)
			auto_index = false;
		n = conf.find("on", i + 1);
		if (n != std::string::npos)
			auto_index = true;
		conf.erase(conf.find("autoindex"), conf.find("\n", conf.find("autoindex")) - conf.find("autoindex"));	
	}

	n = conf.find("index");
	while (n != std::string::npos && isalpha(conf[n - 1]))
		n = conf.find("index", ++n);
	if (n == std::string::npos)
		index = "index.html";
	else
	{
		index = trimSpaces(conf.substr(n + 6, conf.find("\n", n + 1) - (n + 6)));
		conf.erase(n, conf.find("\n", n + 1) - n);
	}

	n = conf.find("allow_methods");
	if (n == std::string::npos)
		methods.push_back("GET");
	else
	{
		conf.erase(n, 13);
		n = conf.find("GET");
		if (n != std::string::npos)
		{
			methods.push_back(trimSpaces(conf.substr(n, 3)));
			conf.erase(n, 3);
		}
		n = conf.find("POST");
		if (n != std::string::npos)
		{
			methods.push_back(trimSpaces(conf.substr(n, 4)));
			conf.erase(n, 4);
		}
		n = conf.find("DELETE");
		if (n != std::string::npos)
		{
			methods.push_back(trimSpaces(conf.substr(n, 6)));
			conf.erase(n, 6);
		}
		std::vector<LocationParser>::iterator	it = location.begin();
		while (it != location.end())
		{
			if(it->getMethodsSize() == 0)
				it->setMethods(methods);
			++it;
		}
	}

	n = conf.find("root");
	if (n == std::string::npos)
		root = "";
	else
	{
		i = n + 5;
		n = conf.find("\n", n + 1);
		root = trimSpaces(conf.substr(i, n - i));
		if (root[root.length() - 1] == '/')
			throw std::runtime_error("Error: 2");
		else if (root[0] != '/')
			throw std::exception();
		conf.erase(conf.find("root"), conf.find("\n", conf.find("root")) - conf.find("root"));
	}

	n = conf.find("upload");
	if (n == std::string::npos)
		upload = "";
	else
	{
		upload = trimSpaces(conf.substr(n + 7, conf.find("\n", n + 1) - n - 7));
		if (upload[0] != '/')
			throw std::exception();
		conf.erase(conf.find("upload"), conf.find("\n", conf.find("upload")) - conf.find("upload"));
	}

	i = -1;
	while (conf[++i])
		if (conf[i] != ' ' && conf[i] != '\n' && conf[i] != '}' && conf[i] != '\t' && conf[i] != '{')
			throw std::runtime_error("Error: 3");
	
	getInfo();

	/*memset(&default_addrinfo, 0, sizeof(struct addrinfo));
	default_addrinfo.ai_family = AF_INET;
	default_addrinfo.ai_socktype = SOCK_STREAM;
	default_addrinfo.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, "8080", &default_addrinfo, &returned_sockaddr) != 0)
	{
		//error
	}
	i = 0;
	struct addrinfo *cpy = returned_sockaddr;
	while (cpy)
	{
		cpy = cpy->ai_next;
		i++;
	}
	std::cout << "returned_sockaddr has " << i << " node(s)" << std::endl;

	struct sockaddr_in sockaddr_conf;

	memset(&sockaddr_conf, 0, sizeof(struct sockaddr_in));
	sockaddr_conf.sin_family = AF_INET;
	sockaddr_conf.sin_port = htons(8080);
	sockaddr_conf.sin_addr.s_addr = inet_addr("0.0.0.0"); 

	if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		throw std::runtime_error("Error: socket");
	}
	//setsockopt
	if (bind(serverSocket, returned_sockaddr->ai_addr, returned_sockaddr->ai_addrlen) == -1)
	{
		throw std::runtime_error(strerror(errno));
	}
	if (listen(serverSocket, 1000) == -1)
	{
		throw std::runtime_error("Error: listen");
	}
	//freeaddrinfo(returned_sockaddr);
	//std::cout << "So far so good..." << std::endl;
	return ;*/
}

Server::Server(const Server& ser)
{
	this->location = ser.location;
	this->port = ser.port;
	this->s_name = ser.s_name;
	this->error = ser.error;
	this->c_size = ser.c_size;
	this->auto_index = ser.auto_index;
	this->index = ser.index;
	this->methods = ser.methods;
	this->root = ser.root;
	this->upload = ser.upload;
	this->redirec = ser.redirec;
	this->cgi = ser.cgi;
	this->serverSocket = ser.serverSocket;
}

Server::~Server()
{

}

Server &Server::operator=(const Server & ser)
{
	if (this != &ser)
	{
		this->location = ser.location;
		this->port = ser.port;
		this->s_name = ser.s_name;
		this->error = ser.error;
		this->c_size = ser.c_size;
		this->auto_index = ser.auto_index;
		this->index = ser.index;
		this->methods = ser.methods;
		this->root = ser.root;
		this->upload = ser.upload;
		this->redirec = ser.redirec;
		this->cgi = ser.cgi;
		this->serverSocket = ser.serverSocket;
	}
	return *this;
}

void	Server::getInfo()
{
	std::cout << "Server" << std::endl;
	std::vector<std::string>::iterator	p_it = port.begin();
	std:: cout << "Port : ";
	while (p_it != port.end())
	{
		std:: cout << *p_it << " ";
		++p_it;
	}
	std::cout << std::endl;
	std::vector<std::string>::iterator v_it = s_name.begin();
	while (v_it != s_name.end())
	{	
		std::cout << "Server name : " << *v_it << std::endl;
		++v_it;
	}
	std::map<int, std::string>::iterator m_it = error.begin();
	while (m_it != error.end())
	{
		std::cout << "Error page : " << m_it->first << " " << m_it->second << std::endl;
		++m_it;
	}
	std::vector<std::string>::iterator	meth_it = methods.begin();
	std::cout << "Allow methods : ";
	while (meth_it != methods.end())
	{
		std::cout << *meth_it << " ";
		++meth_it;
	}
	std::cout << std::endl;
	std::cout << "Client size : " << c_size << std::endl;
	std::cout << "Auto index : " << auto_index << std::endl;
	std::cout << "Index : " << index << std::endl;
	std::cout << "Root : " << root << std::endl;
	std::cout << "Upload : " << upload << std::endl;
	std::cout << "Redirec : " << redirec.first << " " << redirec.second << std::endl;
	std::map<std::string, std::string>::iterator cgi_it = cgi.begin();
	std::cout << "CGI : " << std::endl;
	while (cgi_it != cgi.end())
	{
		std::cout << cgi_it->first << " -> " << cgi_it->second << std::endl;
		cgi_it++;
	}
	std::cout << std::endl;
}


int Server::getServerSocket() const
{
	return this->serverSocket;
}

void Server::openServerSocket()
{
	struct addrinfo default_addrinfo;
	struct addrinfo *returned_sockaddr;

	memset(&default_addrinfo, 0, sizeof(struct addrinfo));
	default_addrinfo.ai_family = AF_INET;
	default_addrinfo.ai_socktype = SOCK_STREAM;
	default_addrinfo.ai_flags = AI_PASSIVE;
	//this->port_str = "8080";//remove this later
	if (getaddrinfo(NULL, port[0].c_str(), &default_addrinfo, &returned_sockaddr) != 0)
	{
		throw std::runtime_error("Error: getaddrinfo");
	}
	
	if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		throw std::runtime_error("Error: socket");
	}
	//setsockopt
	
	if (bind(serverSocket, returned_sockaddr->ai_addr, returned_sockaddr->ai_addrlen) == -1)
	{
		throw std::runtime_error(strerror(errno));
	}
	if (listen(serverSocket, 1000) == -1)
	{
		throw std::runtime_error("Error: listen");
	}
	//freeaddrinfo(returned_sockaddr);
	//std::cout << "So far so good..." << std::endl;
	return ;
}

std::vector<std::string> Server::getPortVec() const
{
	return this->port;
}

std::vector<std::string> Server::getServerNames() const
{
	return this->s_name;
}

std::vector<LocationParser> &Server::getLocations()
{
	return this->location;
}

std::string Server::getRoot() const
{
	return this->root;
}

bool Server::isAllowedMethod(std::string meth)
{
	if (std::find(methods.begin(), methods.end(), meth) == methods.end())
		return false;
	return true;
}

std::string Server::getRedirpath() const
{
	return this->redirec.second;
}

bool Server::getAutoIndex() const
{
	return this->auto_index;
}

std::string Server::getIndex() const
{
	return this->index;
}

void Server::clearPort()
{
	port.clear();
}

void Server::setPort(std::string p)
{
	port.push_back(p);
}

std::string Server::getPort() const
{
	return this->port[0];
}

void Server::setServerSocket(int fd)
{
	this->serverSocket = fd;
}

std::string Server::getUpload() const
{
	return this->upload;
}

int Server::getCSize() const
{
	return this->c_size;
}

std::map<int, std::string> &Server::getError()
{
	return this->error;
}