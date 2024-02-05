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
		std::vector<LocationParser> 		location;
		std::vector<std::string>			port;
		std::vector<std::string>			s_name;
		std::map<int, std::string>			error;
		int									c_size;
		bool								auto_index;
		std::string 						index;
		std::vector<std::string>			methods;
		std::string							root;
		std::string							upload;
		std::pair<std::string, std::string>	redirec;
		std::map<std::string, std::string>	cgi;
		int 								serverSocket;

	public:
		Server();
		Server(std::string);
		Server(const Server&);
		~Server();

		Server &operator=(const Server&);

		void openServerSocket();
		bool isAllowedMethod(std::string);
		void clearPort();
		void messageOpenServer();

		void								getInfo(void);
		int									getServerSocket() const;
		std::vector<std::string>			getPortVec() const;
		std::vector<std::string>			getServerNames() const;
		std::vector<LocationParser> 		&getLocations();
		std::string 						getRoot() const;
		std::string 						getRedirpath() const;
		bool 								getAutoIndex() const;
		std::string 						getIndex() const;
		std::string 						getPort() const;
		std::string 						getUpload() const;
		int 								getCSize() const;
		std::map<int, std::string> 			&getError();
		std::map<std::string, std::string>	&getCGI();
		std::vector<std::string> 			&getSName();

		void setPort(std::string);
		void setServerSocket(int);
};

#endif
