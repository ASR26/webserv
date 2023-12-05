/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:19:04 by gromero-          #+#    #+#             */
/*   Updated: 2023/11/28 12:31:22 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef SERVERPARSER_HPP
# define SERVERPARSER_HPP

# include "webserv.hpp"
# include "LocationParser.hpp"
# include <fstream>

class	ServerParser
{
	private:
		std::vector<LocationParser> location;
		int							port;
		std::vector<std::string>	s_name;
		std::map<int, std::string>	error;
		int							c_size;

	public:
		ServerParser();
		ServerParser(std::string);
		~ServerParser();
		void getInfo(void);
};

#endif
