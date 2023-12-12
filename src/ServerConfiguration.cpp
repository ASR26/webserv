/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 12:17:57 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/12 15:55:59 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

ServerConfiguration::ServerConfiguration()
{
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
	int i = 0;
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
}

ServerConfiguration::~ServerConfiguration()
{
	return ;
}
/*
int ServerConfiguration::getServerSocket() const
{
	return this->serverSocket;
}

void ServerConfiguration::addRequest(int fd)
{
	requestQueue.insert(std::pair<int, class Request>(fd, Request(fd)));
	return ;
}*/
