/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/15 12:13:01 by ysmeding          #+#    #+#             */
/*   Updated: 2023/11/28 12:33:24 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "ServerConfiguration.hpp"

class WebServer
{
	private:
		std::vector<class ServerConfiguration> servers;
		struct sockaddr_storage acceptedaddrinfo;
		int serverSocket_acc;
		socklen_t acceptedaddrinfo_size;
	public:
		WebServer();
		~WebServer();

		void addServer(class ServerConfiguration & server);
		void runWebserv();
};

#endif
