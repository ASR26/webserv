/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationParser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 12:19:26 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/18 15:24:22 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

LocationParser::LocationParser()
{

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
	if (n != std::string::npos)
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
		root = "";
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
	n = conf.find("autoindex");
	if (n == std::string::npos)
		auto_index = false;
	else
	{
		i = n;
		n = conf.find("off", n + 1);
		if (n != std::string::npos)
			auto_index = false;
		n = conf.find("on", i + 1);
		if (n != std::string::npos)
			auto_index = true;
	}
	n = conf.find("index");
	while (n != std::string::npos && isalpha(conf[n - 1]))
		n = conf.find("index", ++n);
	if (n == std::string::npos)
		index = "index.html";
	else
		index = conf.substr(n + 6, conf.find("\n", n + 1) - (n + 6));
	n = conf.find("upload");
	if (n == std::string::npos)
		upload = "";
	else
		upload = conf.substr(n + 7, conf.find("\n", n) - (n + 7));
	n = conf.find("return");
	if (n == std::string::npos)
	{
		redirec.first = "";
		redirec.second = "";
	}
	else
	{
		redirec.first = conf.substr(n + 7, conf.find(" ", n + 7) - n - 7);
		n = conf.find(" ", n + 8);
		redirec.second = conf.substr(n + 1, conf.find("\n", n + 1) - n - 1);
	}
	n = conf.find("cgi_pass");
	if (n == std::string::npos)
		cgi_pass = "";
	else
	{
		cgi_pass = conf.substr(n + 9, conf.find("\n", n + 1) - n - 9);
	}
	getInfo();
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
	std::cout << "Auto index : " << auto_index << std::endl;
	std::cout << "Index : " << index << std::endl;
	std::cout << "Upload : " << upload << std::endl;
	std::cout << "Redirec : " << redirec.first << " " << redirec.second << std::endl;
	std::cout << std::endl;
}

size_t	LocationParser::getMethodsSize() const
{
	return (methods.size());
}

void	LocationParser::setMethods(std::vector<std::string> cpy)
{
	methods = cpy;
}

std::string	LocationParser::getCGI() const
{
	return (cgi_pass);
}
