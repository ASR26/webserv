/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 12:15:48 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/12 13:35:39 by ysmeding         ###   ########.fr       */
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

void WebServer::addServer(std::string conf)
{
	std::string::size_type n;
	std::fstream fd;
	std::string str;
	std::string file;

	n = conf.find(".conf");
	if (std::string::npos == n)
		throw std::exception();
	fd.open (conf, std::fstream::in);
	if (fd.is_open())
		while (std::getline(fd, str))
		{
			file += str;
			file.push_back('\n');
		}
	else
		throw std::exception();
	n = file.find("server");
	while (n != std::string::npos)
	{
		int	i = n + 5;
		while (file[++i] && file[i] != '{')
			if (file[i] != ' ' && file[i] != '\t')
				throw std::exception();
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

void WebServer::runWebserv()
{
	//int c;
	int k;
	int r;
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
	std::cout << servers.size() << std::endl;
	EV_SET(&new_event, servers[0].getServerSocket(), EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
	k = kevent(kq, &new_event, 1, NULL, 0, NULL);
	/*if (k == -1)
		Error::functionError();*/
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
			std::cout << std::endl << "closing connection" << std::endl;
			close(finished_event.ident);
		}
		else if (finished_event.ident == (unsigned long)servers[0].getServerSocket())
		{
			std::cout << std::endl << "accept connection" << std::endl;
			if ((serverSocket_acc = accept(servers[0].getServerSocket(), (struct sockaddr *)&acceptedaddrinfo, &acceptedaddrinfo_size)) == -1)
				Error::functionError();
			EV_SET(&new_event, serverSocket_acc, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			kevent(kq, &new_event, 1, NULL, 0, NULL);
		}
		else if (finished_event.filter == EVFILT_READ)
		{
			std::cout << std::endl << "receiving request" << std::endl;
			servers[0].addRequest(finished_event.ident);
			EV_SET(&new_event, finished_event.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
			kevent(kq, &new_event, 1, NULL, 0, NULL);
			EV_SET(&finished_event, serverSocket_acc,  EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, NULL);
			kevent(kq, &finished_event, 1, NULL, 0, NULL);
		}
		else if (finished_event.filter == EVFILT_WRITE)
		{
			std::cout << std::endl << "sending response" << std::endl;
			
			int fd = open("tab.html", O_RDONLY);
			char buff2[10000];
			r = read(fd, buff2, 10000);
			close(fd);
			write(finished_event.ident, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 449\r\n\r\n", strlen( "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 449\r\n\r\n"));
			write(finished_event.ident, buff2, r);

			EV_SET(&finished_event, finished_event.ident,  EVFILT_READ, EV_DELETE, 0, 0, NULL);
			kevent(kq, &finished_event, 1, NULL, 0, NULL);
			EV_SET(&finished_event, finished_event.ident,  EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			kevent(kq, &finished_event, 1, NULL, 0, NULL);
			close(finished_event.ident);
		}
	}
}
