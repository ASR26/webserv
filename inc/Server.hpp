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
		int							port;
		std::vector<std::string>	s_name;
		std::map<int, std::string>	error;
		int							c_size;
		std::map<int, class Request> requestQueue;
		int serverSocket;

	public:
		Server();
		Server(std::string);
		~Server();
		void getInfo(void);
		int getServerSocket() const;
		void addRequest(int fd);
		void formResponse();
};

#endif
