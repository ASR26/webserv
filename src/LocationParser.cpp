/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationParser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 12:19:26 by gromero-          #+#    #+#             */
/*   Updated: 2024/01/08 11:40:35 by gromero-         ###   ########.fr       */
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
	int		j;

	i = 0;
	while (conf[i] != '{')
		i++;
	location = conf.substr(9, i - 9);
	conf.erase(0, ++i);

	n = conf.find("allow_methods");
	if (n != std::string::npos)
	{
		conf.erase(n, 13);
		n = conf.find("GET");
		if (n != std::string::npos)
		{	
			methods.push_back(conf.substr(n, 3));
			conf.erase(n, 3);
		}
		n = conf.find("POST");
		if (n != std::string::npos)
		{
			methods.push_back(conf.substr(n, 4));
			conf.erase(n, 4);
		}
		n = conf.find("DELETE");
		if (n != std::string::npos)
		{	
			methods.push_back(conf.substr(n, 5));
			conf.erase(n, 5);
		}
	}

	n = conf.find("root");
	if (n == std::string::npos)
		root = "";
	else
	{
		i = n + 5;
		n = conf.find("\n", n + 1);
		root = conf.substr(i, n - i);
		if (root[root.length() - 1] == '/')
			throw std::exception();
		else if (root[0] != '/')
			throw std::exception();
		conf.erase(i - 5, n - i + 5);
	}

	n = conf.find("client_max_body_size");
	if (n == std::string::npos)
		c_size = 1;
	else
	{
		i = n;
		while (conf[n] != ' ')
			n++;
		c_size = std::atoi(&conf[n]);
		conf.erase(i, conf.find("\n", n) - i);
	}

	n = conf.find("autoindex");
	if (n == std::string::npos)
		auto_index = false;
	else
	{
		i = n;
		n = conf.find("off", n + 1);
		if (n != std::string::npos)
		{
			auto_index = false;
			conf.erase(i, n - i + 3);
		}
		n = conf.find("on", i + 1);
		if (n != std::string::npos)
		{
			auto_index = true;
			conf.erase(i, n - i + 2);
		}
	}

	n = conf.find("index");
	while (n != std::string::npos && isalpha(conf[n - 1]))
		n = conf.find("index", ++n);
	if (n == std::string::npos)
		index = "index.html";
	else
	{
		index = conf.substr(n + 6, conf.find("\n", n + 1) - (n + 6));
		conf.erase(n, conf.find("\n", n) - n);
	}
	n = conf.find("upload");
	if (n == std::string::npos)
		upload = "";
	else
	{
		upload = conf.substr(n + 7, conf.find("\n", n) - (n + 7));
		conf.erase(n, conf.find("\n", n) - n);
	}

	n = conf.find("return");
	if (n == std::string::npos)
	{
		redirec.first = "";
		redirec.second = "";
	}
	else
	{
		i = n;
		redirec.first = conf.substr(n + 7, conf.find(" ", n + 7) - n - 7);
		n = conf.find(" ", n + 8);
		redirec.second = conf.substr(n + 1, conf.find("\n", n + 1) - n - 1);
		conf.erase(i, conf.find("\n", n) - i);
	}
	
	n = conf.find("cgi_pass");
	if (n == std::string::npos)
		cgi_pass = "";
	else
	{
		cgi_pass = conf.substr(n + 9, conf.find("\n", n + 1) - n - 9);
		conf.erase(n, conf.find("\n", n) - n);
	}

	n = conf.find("error_page");
	if (n != std::string::npos)
	{
		while (n != std::string::npos)
		{
			i = n;
			while (conf[i] != ';')
				i++;
			j = i;
			while (conf[j] != ' ')
				j--;
			while (conf[n] != ' ')
				n++;
			while (std::atoi(&conf[n]))
			{
				error[std::atoi(&conf[n])] = conf.substr(j, i - j);
				n += 1 + std::to_string(std::atoi(&conf[n])).length();
			}
			conf.erase(conf.find("error_page"), conf.find("\n", conf.find("error_page")) - conf.find("error_page"));
			n = conf.find("error_page", n);
		}
	}

	i = -1;
	while (conf[++i])
		if (conf[i] != ' ' && conf[i] != '\n' && conf[i] != '}' && conf[i] != '\t')
			throw std::exception();
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
	std::map<int, std::string>::iterator me_it = error.begin();
	while (me_it != error.end())
	{
		std::cout << "Error page : " << me_it->first << " " << me_it->second;
		++me_it;
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
