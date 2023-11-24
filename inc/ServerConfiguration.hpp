/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 15:21:33 by ysmeding          #+#    #+#             */
/*   Updated: 2023/11/23 16:28:29 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIGURATION_HPP
# define SERVERCONFIGURATION_HPP

# include <string>
# include <map>
# include <vector>

class ServerConfiguration
{
	private:
		std::vector<int> listen_ports;
		std::vector<std::string> server_names;
		std::map<int, std::string> error_pages;
		int client_max_body_size;

		int serverSocket;
		std::vector<int> clientSockets;
	public:
		ServerConfiguration();
		~ServerConfiguration();

		int getServerSocket() const;
		
};

#endif