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
	if (n == std::string::npos)
		throw std::exception();
	else
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
				cgi[conf.substr(conf.find("*.", n) + 2, conf.find(" ", n + 9))] = location.back().getCGI();
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
				port.push_back(conf.substr(n, i - n));
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
				s_name.push_back(conf.substr(n, i - n));
				n = i + 1;
			}
			i++;
		}
		conf.erase(conf.find("server_name"), conf.find("\n", conf.find("server_name")) - conf.find("server_name"));
	}

	n = conf.find("error_page");
	if (n == std::string::npos)
		error[404] = "default_location";
	else
	{
		while (n != std::string::npos)
		{
			i = n;
			while (conf[i] != ';')
				i++;
			j = i;
			while (conf[j] != ' ')
				j--;
			while (conf[n] != ' ')
				n++;
			while (std::atoi(&conf[n]))
			{
				error[std::atoi(&conf[n])] = conf.substr(j, i - j);
				n += 1 + std::to_string(std::atoi(&conf[n])).length();
			}
			conf.erase(conf.find("error_page"), conf.find("\n", conf.find("error_page")) - conf.find("error_page"));
			n = conf.find("error_page", n);
		}
	}

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
		index = conf.substr(n + 6, conf.find("\n", n + 1) - (n + 6));
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
			methods.push_back(conf.substr(n, 3));
			conf.erase(n, 3);
		}
		n = conf.find("POST");
		if (n != std::string::npos)
		{
			methods.push_back(conf.substr(n, 4));
			conf.erase(n, 4);
		}
		n = conf.find("DELETE");
		if (n != std::string::npos)
		{
			methods.push_back(conf.substr(n, 5));
			conf.erase(n, 5);
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
		root = conf.substr(i, n - i);
		if (root[root.length() - 1] == '/')
			throw std::exception();
		else if (root[0] != '/')
			throw std::exception();
		conf.erase(conf.find("root"), conf.find("\n", conf.find("root")) - conf.find("root"));
	}

	n = conf.find("upload");
	if (n == std::string::npos)
		upload = "";
	else
	{
		upload = conf.substr(n + 7, conf.find("\n", n + 1) - n - 7);
		conf.erase(conf.find("upload"), conf.find("\n", conf.find("upload")) - conf.find("upload"));
	}

	n = conf.find("return");
	if (n == std::string::npos)
	{
		redirec.first = "";
		redirec.second = "";
	}
	else
	{
		redirec.first = conf.substr(n + 7, conf.find(" ", n + 7) - n - 7);
		n = conf.find(" ", n + 8);
		redirec.second = conf.substr(n + 1, conf.find("\n", n + 1) - n - 1);
		conf.erase(conf.find("return"), conf.find("\n", conf.find("return")) - conf.find("return"));
	}

	i = -1;
	while (conf[++i])
		if (conf[i] != ' ' && conf[i] != '\n' && conf[i] != '}' && conf[i] != '\t' && conf[i] != '{')
			throw std::exception();
	
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

Server::~Server()
{

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
	std::cout << std::endl;
}
int Server::getServerSocket() const
{
	return this->serverSocket;
}

void Server::addRequest(int fd)
{
	requestQueue.insert(std::pair<int, class Request>(fd, Request(fd)));
	return ;
}

