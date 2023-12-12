/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:21:26 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/12 10:30:10 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

ServerParser::ServerParser()
{

}

ServerParser::ServerParser(std::string conf)
{
	std::string::size_type	n;
	int						i = 0;
	int						j = 0;
	struct addrinfo default_addrinfo;
	struct addrinfo *returned_sockaddr;

	memset(&default_addrinfo, 0, sizeof(struct addrinfo));
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

	/* struct sockaddr_in sockaddr_conf;

	memset(&sockaddr_conf, 0, sizeof(struct sockaddr_in));
	sockaddr_conf.sin_family = AF_INET;
	sockaddr_conf.sin_port = htons(8080);
	sockaddr_conf.sin_addr.s_addr = inet_addr("0.0.0.0"); */

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
			n = conf.find("location", i);
		}
	}
	n = conf.find("listen");
	if (n == std::string::npos)
		port = 80;
	else
		while (conf[++n] && !isdigit(conf[n]))
			port = std::atoi(&conf[n]);
	n = conf.find("server_name");
	if (n == std::string::npos)
		s_name.push_back("host_name");
	else
	{
		i = n + 11;
		while (conf[i] == ' ' && conf[i])
			i++;
		n = i;
		while (conf[i] && conf[i] != '\n')
		{
			if (conf[i] == ' ' || conf[i] == ';')
			{
				s_name.push_back(conf.substr(n, i - n));
				n = i + 1;
			}
			i++;
		}
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
	}
	//getInfo();
}

ServerParser::~ServerParser()
{

}

void	ServerParser::getInfo()
{
	std::cout << "Server" << std::endl << "Port : " << port << std::endl;
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
	std::cout << "Client size : " << c_size << std::endl;
	std::cout << std::endl;
}
int ServerParser::getServerSocket() const
{
	return this->serverSocket;
}

void ServerParser::addRequest(int fd)
{
	requestQueue.insert(std::pair<int, class Request>(fd, Request(fd)));
	return ;
}

