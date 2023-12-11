/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 13:59:41 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/04 17:19:14 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include <iostream>
#include <fstream>
#include <string>

Request::Request(): header(""), body("")
{
	return ;
}

Request::Request(int fd)
{
	char buff[101];
	int r;
	int pos;
	std::string str = "";
	int done = 0;
	while (!done)
	{
		r = read(fd, buff, 100);
		buff[r] = 0;
		if (r < 100)
			done = 1;
		str += buff;
	}
	pos = str.find("\r\n\r\n");
	if (pos == std::string::npos)
	{
		header = str;
		body = "";
	}
	else
	{
		this->header = str.substr(0, pos);
		body = str.substr(pos + 4, str.size() - (pos + 4));
	}
	this->setMethod();
	std::cout << method << std::endl;
}

Request::Request(const Request& req): header(req.header), body(req.body)
{
	return ;
}

Request::~Request()
{
	return ;
}

Request& Request::operator=(const Request& req)
{
	if (this != &req)
	{
		this->header = req.header;
		this->body = req.body;
	}
	return *this;
}

void Request::setMethod()
{
	int pos;
	int i = 0;
	std::string methods[3] = {"GET", "POST", "DELETE"};
	for (i = 0; i < 3; i++)
	{
		if (this->header.compare(0, methods[i].size(), methods[i]) == 0)
		{
			method = methods[i];
			break;
		}
	}
	if (i == 3)
		method = "UNKNOWN";
}