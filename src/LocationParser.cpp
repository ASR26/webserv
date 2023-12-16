/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationParser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 12:19:26 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/16 10:42:22 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

LocationParser::LocationParser()
{
	location = "/lazy/test/what";
	root = "/abc/123/xyz/bye";
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
}

LocationParser::LocationParser(std::string conf)
{
	std::string::size_type	n;
	int		i;

	i = 0;
	while (conf[i] != '{')
		i++;
	location = conf.substr(9, i - 9);
	n = conf.find("allow_methods");
	if (n == std::string::npos)
		methods.push_back("NONE");
	else
	{
		n = conf.find("GET");
		if (n != std::string::npos)
			methods.push_back(conf.substr(n, 3));
		n = conf.find("POST");
		if (n != std::string::npos)
			methods.push_back(conf.substr(n, 4));
		n = conf.find("DELETE");
		if (n != std::string::npos)
			methods.push_back(conf.substr(n, 5));
	}
	n = conf.find("root");
	if (n == std::string::npos)
		root = "DEFAULT";
	else
	{
		i = n + 5;
		n = conf.find("\n", n + 1);
		root = conf.substr(i, n - i);
	}
	n = conf.find("client_max_body_size");
	if (n == std::string::npos)
		c_size = 1;
	else
	{
		while (conf[n] != ' ')
			n++;
		c_size = std::atoi(&conf[n]);
	}
	//getInfo();
}

LocationParser::~LocationParser()
{

}

void	LocationParser::getInfo(void)
{
	std::cout << "Location : " << location << std::endl;
	std::vector<std::string>::iterator	m_it = methods.begin();
	std::cout << "Allow methods : ";
	while (m_it != methods.end())
	{
		std::cout << *m_it << " ";
		++m_it;
	}
	std::cout << std::endl;
	std::cout << "Root : " << root << std::endl;
	std::cout << "Client size : " << c_size << std::endl;
	std::cout << std::endl;
}

std::string LocationParser::getLocation() const
{
	return this->location;
}

bool LocationParser::isAllowedMethod(std::string meth)
{
	if (std::find(methods.begin(), methods.end(), meth) == methods.end())
		return false;
	return true;
}

std::string LocationParser::getRoot() const
{
	return this->root;
}
