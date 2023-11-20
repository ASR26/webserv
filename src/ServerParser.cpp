/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:21:26 by gromero-          #+#    #+#             */
/*   Updated: 2023/11/20 17:11:05 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../inc/ServerParser.hpp"

ServerParser::ServerParser()
{

}

ServerParser::ServerParser(std::string conf)
{
	std::string::size_type n;
	n = conf.find(".conf");
	if (std::string::npos == n)
		throw std::exception();
	
	std::fstream fd;
	std::string str;
	std::vector<std::string>::iterator	it;

	fd.open (conf, std::fstream::in);
	if (fd.is_open())
		while (std::getline(fd, str))
			file.push_back(str + '\n');
	else
		throw std::exception();
	it = file.begin(); 
	while (it != file.end())
	{
		std::cout << *it;
		it++;
	}
	fd.close();
}

ServerParser::~ServerParser()
{

}
