/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 12:13:01 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/12 13:37:05 by ysmeding         ###   ########.fr       */
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
	public:
		WebServer();
		~WebServer();

		void addServer(std::string);
		void runWebserv();
};

#endif
