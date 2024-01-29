/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 12:13:01 by ysmeding          #+#    #+#             */
/*   Updated: 2024/01/26 13:44:58 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "ServerConfiguration.hpp"

class WebServer
{
	private:
		std::vector<class Server> servers;
		struct sockaddr_storage acceptedaddrinfo;
		int serverSocket_acc;
		socklen_t acceptedaddrinfo_size;
		std::map<int, class Request> requestQueue;
	public:
		WebServer();
		WebServer(const WebServer &);
		WebServer& operator=(const WebServer &);
		~WebServer();

		void addServer(std::string);
		void runWebserv();
		void addRequest(int fd);
		int existRequest(int fd);
		void assignServerToRequest(class Request &req);
		void configureServer();

		void addTestServer();
		bool isServerSocket(int);
		void checkServerSpecification();
};

#endif
