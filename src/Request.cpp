/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 13:59:41 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/16 10:48:43 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include <iostream>
#include <fstream>
#include <string>

Request::Request(): fd(-1), body_size(0), request(""), header(""), body(""), method(""), \
response(""), done_read(false), done_write(false)
{
	//std::cout << "Created an empty request fd: " << this->fd << std::endl;
	return ;
}

Request::Request(int fd_r): fd(fd_r), body_size(0), request(""), header(""), body(""), \
method(""), response(""), done_read(false), done_write(false)
{
	this->fd = fd_r;
	//std::cout << "Created a request fd: " << this->fd << std::endl;
	/* char buff[101];
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
	if (pos == (int)std::string::npos)
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
	std::cout << method << std::endl; */
}

Request::Request(const Request& req): fd(req.fd), body_size(req.body_size), header(req.header), \
body(req.body), method(req.method), response(req.response), host(req.host), done_read(req.done_read), \
done_write(req.done_write)
{
	//std::cout << "Created a copy request fd: " << this->fd << std::endl;
	return ;
}

Request::~Request()
{
	//std::cout << "destructing request for fd: " << this->fd << std::endl;
	return ;
}

Request& Request::operator=(const Request& req)
{
	if (this != &req)
	{
		this->fd = req.fd;
		this->body_size = req.body_size;
		this->header = req.header;
		this->body = req.body;
		this->method = req.method;
		this->response = req.response;
		this->done_read = req.done_read;
		this->done_write = req.done_write;
		this->host = req.host;
	}
	return *this;
}

void Request::readRequest()
{
	int r;
	char buf[10001];
	unsigned long pos;

	r = read(this->fd, buf, 10000);
	if (r <= 0)
	{
		//for -1 write error/send error code?
		return ;
	}
	buf[r] = 0;
	request += std::string(buf);
	if (header.empty() && (pos = request.find("\r\n\r\n")) != std::string::npos)
	{
		this->header = request.substr(0, pos);
		//std::cout << this->header << std::endl;
		if ((pos = header.find("Content-Length: ")) != std::string::npos)
		{
			this->body_size = std::atoi(header.substr(pos + std::strlen("Content-Length: "), header.substr(pos, std::string::npos).find("\n") - (pos + std::strlen("Content-Length: "))).c_str());
			//std::cout << this->body_size << std::endl;
		}
		if (request.size() > header.size() + 4)
		{
			body = request.substr(header.size() + 4, std::string::npos);
			//std::cout << "body: " << this->body << std::endl;
		}
		request.clear();
	}
	else if (!header.empty())
	{
		body += request;
		request.clear();
	}
	if (!header.empty() && body.size() == this->body_size)
	{
		done_read = true;
		this->setMethod();
		//std::cout << method << std::endl;
		//std::cout << "done reading" << std::endl;
		//std::cout << "HEADER" << std::endl;
		//std::cout << this->header << std::endl;
		//std::cout << "BODY" << std::endl;
		//std::cout << this->body << std::endl;
	}
	return ;
}

void Request::selectLocation()
{
	std::vector<std::string> directories = split(request_file, "/");
	loc_index = -1;
	size_t max_equal = 0;
	size_t equal;
	for (size_t i = 0; i < server.getLocations().size(); i++)
	{
		equal = nbrEqualStr(directories, split(server.getLocations()[i].getLocation(), "/"));
		if (equal >= split(server.getLocations()[i].getLocation(), "/").size() && equal > max_equal)
		{
			max_equal = equal;
			loc_index = i;
		}
	}
}

bool Request::isAllowedMethod()
{
	if (loc_index == -1)
		return (server.isAllowedMethod(method));
	else
		return server.getLocations()[loc_index].isAllowedMethod(method);
}

void Request::executeGetRequest()
{
	if (access(request_file_path.c_str(), F_OK))
	{
		response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 9\r\n\r\nnot found";
		return ;
	}
	response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 14\r\n\r\nhello from get";
}

void Request::executePostRequest()
{
	response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 15\r\n\r\nhello from post";
}

void Request::executeDeleteRequest()
{
	response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 17\r\n\r\nhello from delete";
}

void Request::formResponse()
{
	unsigned long pos_space = header.substr(header.find(" ") + 1, std::string::npos).find(" ");
	request_file = header.substr(header.find(" ") + 1, pos_space);
	//request_file_path = server.getRoot() + request_file;
	//request_file_path = std::string(".") + request_file; //change to actual root!!!
	/* if (access(request_file_path.c_str(), F_OK))
	{
		response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 9\r\n\r\nnot found";
		return ;
	} */
	selectLocation();
	if (!isAllowedMethod())
	{
		response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length:11\r\n\r\nnot allowed";
		return ;
	}
	if (loc_index >= 0 && !server.getLocations()[loc_index].getRoot().empty())
	{
		request_file.replace(0, server.getLocations()[loc_index].getLocation().size(), server.getLocations()[loc_index].getRoot());
	}
	request_file_path = std::string(".") + server.getRoot() + request_file;
	//std::cout << request_file_path << std::endl;
	std::string meths[3] = {"GET", "POST", "DELETE"};
	int i = 0;
	for (; i < 3; i++)
		if (method == meths[i])
			break;
	switch (i)
	{
		case 0:
			executeGetRequest();
			break;
		case 1:
			executePostRequest();
			break;
		case 2:
			executeDeleteRequest();
			break;
	}
	return ;
	response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 5\r\n\r\nhello";
}

void Request::sendResponse()
{
	int w;

	if (this->response.empty())
		this->formResponse();
	w = write(this->fd, this->response.c_str(), this->response.size());
	if (w <= 0)
	{
		//put some error thing for -1
		return ;
	}
	if (response.size() < (size_t)w)
		response = response.substr(w, std::string::npos);
	else
		response = "";
	if (response.empty())
	{
		done_write = true;
	}
}

void Request::setMethod()
{
	//int pos;
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

std::string Request::getHeader() const
{
	return header;
}

void Request::setServer(class Server &ser)
{
	this->server = ser;
	return ;
}

void Request::setHost(std::string h)
{
	this->host = h;
}

std::string Request::getHost() const
{
	return this->host;
}

void Request::setResponse(std::string res)
{
	this->response = res;
}