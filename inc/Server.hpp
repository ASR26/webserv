/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:19:04 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/12 10:28:39 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include "LocationParser.hpp"
# include <fstream>

class	Server
{
	private:
		std::vector<LocationParser> location;
		int							port;//should be vector
		std::string					port_str;//should be vector
		std::vector<std::string>	s_name;
		std::map<int, std::string>	error;
		int							c_size;
		std::string					index;
		bool						auto_index;
		std::vector<std::string>	methods;
		std::string					root;
		std::string					redir_path;

		int serverSocket;

	public:
		Server();
		Server(std::string);
		Server(const Server&);
		~Server();

		Server &operator=(const Server&);
		void getInfo(void);
		int getServerSocket() const;
		void openServerSocket();
		std::string getPort() const;
		std::vector<std::string> getServerNames() const;
		std::vector<LocationParser> getLocations() const;
		std::string getRoot() const;
		bool isAllowedMethod(std::string);
		std::string getRedirpath() const;
		bool getAutoIndex() const;
		std::string getIndex() const;
};

#endif
