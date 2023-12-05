/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 15:21:33 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/04 17:44:44 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIGURATION_HPP
# define SERVERCONFIGURATION_HPP

# include <string>
# include <map>
# include <vector>
# include "Request.hpp"

class ServerConfiguration
{
	private:
		std::vector<int> listen_ports;
		std::vector<std::string> server_names;
		std::map<int, std::string> error_pages;
		int client_max_body_size;

		int serverSocket;
		std::vector<int> clientSockets;

		std::map<int, class Request> requestQueue;
	public:
		ServerConfiguration();
		~ServerConfiguration();

		int getServerSocket() const;
		void addRequest(int fd);
		void formResponse();
};

#endif