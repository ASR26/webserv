/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 12:15:48 by ysmeding          #+#    #+#             */
/*   Updated: 2023/11/20 12:40:26 by ysmeding         ###   ########.fr       */
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

void WebServer::addServer(class ServerConfiguration & server)
{
	servers.push_back(server);
}

void WebServer::runWebserv()
{
	int events;
	
	acceptedaddrinfo_size = sizeof(struct sockaddr_storage);
	while (1)
	{
		if ((serverSocket_acc = accept(servers[0].getServerSocket(), (struct sockaddr *)&acceptedaddrinfo, &acceptedaddrinfo_size)) == -1)
		{
			throw std::runtime_error("Error: accept");
		}
		std::cout << "Connection accepted" << std::endl;
	}

	/* while (true)
	{
		events = poll();

		if (events < 0)
			//error
		else if (events == 0)
			continue;
		else
		{
			
		}
	} */
}