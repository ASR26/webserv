/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:19:04 by gromero-          #+#    #+#             */
/*   Updated: 2023/11/20 16:53:22 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef SERVERPARSER_HPP
# define SERVERPARSER_HPP

# include "webserv.hpp"
# include <fstream>

class	ServerParser
{
	private:
		std::vector<std::string>		file;
		std::vector<std::string>	server_name;
	public:
		ServerParser();
		ServerParser(std::string);
		~ServerParser();
};

#endif
