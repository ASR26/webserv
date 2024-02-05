/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 13:59:41 by ysmeding          #+#    #+#             */
/*   Updated: 2024/02/05 18:53:02 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include <iostream>
#include <fstream>
#include <string>



Request::Request(): fd(-1), body_size(0), request(""), header(""), body(""), method(""), \
response(""), loc_index(-1), done_read(false), done_write(false), error(false)
{
	return ;
}

Request::Request(int fd_r): fd(fd_r), body_size(0), request(""), header(""), body(""), \
method(""), response(""), loc_index(-1), done_read(false), done_write(false), error(false)
{
	this->fd = fd_r;
}

Request::Request(const Request& req): fd(req.fd), body_size(req.body_size), content_type(req.content_type), request(req.request), header(req.header), \
body(req.body), body_raw(req.body_raw), method(req.method), response(req.response), original_request(req.original_request), server(req.server), host(req.host), \
request_file(req.request_file), request_file_path(req.request_file_path), loc_index(req.loc_index), query_string(req.query_string), done_read(req.done_read), done_write(req.done_write), error(false)
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
		this->fd = req.fd;
		this->body_size = req.body_size;
		this->content_type = req.content_type;
		this->request = req.request;
		this->header = req.header;
		this->body = req.body;
		this->method = req.method;
		this->response = req.response;
		this->original_request = req.original_request;
		this->server = req.server;
		this->request_file = req.request_file;
		this->request_file_path = req.request_file_path;
		this->loc_index = req.loc_index;
		this->done_read = req.done_read;
		this->done_write = req.done_write;
		this->host = req.host;
		this->body_raw = req.body_raw;
		this->query_string = req.query_string;
		this->error = req.error;
	}
	return *this;
}

Server Request::returnServerOfRequest(std::vector<class Server> servers)
{
	std::vector<int> index;
	
	unsigned long pos_start = header.find("Host: ");
	unsigned long pos_end = header.substr(pos_start, std::string::npos).find("\r\n");
	this->host = (header.substr(pos_start + std::strlen("Host: "), pos_end - std::strlen("Host: ")));
	if (host.find(":") == std::string::npos)
		throw std::runtime_error("No server found");
	std::string port = host.substr(host.find(":") + 1, std::string::npos);
	for (unsigned long i = 0; i < servers.size(); i++)
	{
		if (port == servers[i].getPort())
			index.push_back(i);
	}
	std::string server_name;
	pos_end = host.rfind(":");
	if (!std::isdigit(host[pos_end - 1]))
	{
		pos_end = host.find(".localhost");
		if (pos_end != std::string::npos)
			server_name = host.substr(0, pos_end);
		else
			server_name = "";
	}
	else
	{
		int n = 1;
		int point_count = 0;
		while (pos_end - n >= 0 && (std::isdigit(host[pos_end - n]) || host[pos_end - n] == '.') && point_count < 4)
		{
			if (host[pos_end - n] == '.')
				point_count++;
			n++;
		}
		if (pos_end - n < 0)
			server_name = "";
		else
			server_name = host.substr(0, pos_end - n);
	}
	if (server_name.empty())
		return servers[index[0]];
	else
	{
		int found = 0;
		for (unsigned long i = 0; i < index.size(); i++)
		{
			for (unsigned long j = 0; j < servers[index[i]].getServerNames().size(); j++)
			{
				if (servers[index[i]].getServerNames()[j] == server_name)
				{
					found = 1;
					return servers[index[i]];
				}
			}
		}
		if (found == 0)
			throw std::runtime_error("No server found");
		return servers[0];
	}
	
}

void Request::processHeader(std::vector<class Server> servers, int r, char *buf, unsigned long pos)
{
	this->header = request.substr(0, pos);
	if ((pos = header.find("Content-Type: ")) != std::string::npos)
		this->content_type = header.substr(pos + 14, header.find_first_of(";\n", pos + 14) - pos - 14);
	if ((pos = header.find("Content-Length: ")) != std::string::npos)
	{
		this->body_size = std::atoi(header.substr(pos + std::strlen("Content-Length: "), header.substr(pos, std::string::npos).find("\n") - (pos + std::strlen("Content-Length: "))).c_str());
		try
		{
			Server srv = returnServerOfRequest(servers);
			unsigned long pos_space = header.substr(header.find(" ") + 1, std::string::npos).find(" ");
			std::string file = header.substr(header.find(" ") + 1, pos_space);
			int index_for_location = returnLocationIndex(file, srv);
			if (index_for_location >= 0)
			{
				if (this->body_size > (unsigned int)srv.getLocations()[index_for_location].getCSize())
				{
					done_read = true;
					formErrorResponse(413);
					return ;
				}
			}
			else
			{
				if (this->body_size > (unsigned int)srv.getCSize())
				{
					done_read = true;
					formErrorResponse(413);
					return ;
				}
			}
		}
		catch(const std::exception& e)
		{
		}
	}
	if (header.find("Expect: 100-continue") != std::string::npos)
		write(this->fd, "HTTP/1.1 100 CONTINUE\n", 22);
	if ((unsigned int)r > header.size() + 4)
	{
		body = request.substr(header.size() + 4, std::string::npos);
		for (int i = (int)header.size() + 4; i < r; i++)
			body_raw.push_back(buf[i]);
	}
	request.clear();
}

void Request::readRequest(std::vector<class Server> servers)
{
	int r;
	char buf[10001];
	unsigned long pos;

	r = read(this->fd, buf, 10000);
	if (r <= 0)
	{
		error = true;
		return ;
	}
	buf[r] = 0;
	request += std::string(buf);
	if (header.empty() && (pos = request.find("\r\n\r\n")) != std::string::npos)
	{
		processHeader(servers, r, buf, pos);
		if (done_read)
			return;
	}
	else if (!header.empty())
	{
		body += request;
		for (int i = 0; i < r; i++)
			body_raw.push_back(buf[i]);
		request.clear();
	}
	if (!header.empty() && (body.size() == this->body_size || body_raw.size() == this->body_size))
	{
		done_read = true;
		this->setMethod();
	}
	return ;
}

int Request::returnLocationIndex(std::string file, Server srv)
{
	std::vector<std::string> directories = split(file, "/");
	int index_for_location = -1;
	size_t max_equal = 0;
	size_t equal;
	for (size_t i = 0; i < srv.getLocations().size(); i++)
	{
		equal = nbrEqualStr(directories, split(srv.getLocations()[i].getLocation(), "/"));
		if (equal >= split(srv.getLocations()[i].getLocation(), "/").size() && equal > max_equal)
		{
			max_equal = equal;
			index_for_location = i;
		}
	}
	return index_for_location;
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

void Request::formErrorResponse(int error_code)
{
	std::string error_file = "";
	if (loc_index < 0 && server.getError().find(error_code) != server.getError().end())
	{
		error_file = "." + server.getRoot() + server.getError().find(error_code)->second;
		if (access(error_file.c_str(), F_OK))
			error_file = "";
		else
		{
			struct stat buf;
			stat(request_file_path.c_str(), &buf);
			if (S_ISDIR(buf.st_mode))
				error_file = "";
		}
	}
	if (loc_index >= 0 && server.getLocations()[loc_index].getError().find(error_code) != server.getLocations()[loc_index].getError().end())
	{
		error_file = "." + server.getLocations()[loc_index].getRoot() + server.getLocations()[loc_index].getError().find(error_code)->second;
		if (access(error_file.c_str(), F_OK))
			error_file = "";
		else
		{
			struct stat buf;
			stat(request_file_path.c_str(), &buf);
			if (S_ISDIR(buf.st_mode))
				error_file = "";
		}
	}
	switch (error_code)
	{
		case 400:
			response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: ";
			if (error_file.empty())
				error_file = "./example_resources/def/400";
			break;
		case 403:
			response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: ";
			if (error_file.empty())
				error_file = "./example_resources/def/403";
			break;
		case 404:
			response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
			if (error_file.empty())
				error_file = "./example_resources/def/404";
			break;
		case 405:
			response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: ";
			if (error_file.empty())
				error_file = "./example_resources/def/405";
			break;
		case 413:
			response = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\nContent-Length: ";
			if (error_file.empty())
				error_file = "./example_resources/def/413";
			break;
		case 500:
			response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: ";
			if (error_file.empty())
				error_file = "./example_resources/def/500";
			break;
		case 2:
			response = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ");
			if (error_file.empty())
				error_file = "./example_resources/def/delete";
			break;
		case 600:
			response = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ");
			if (error_file.empty())
				error_file = "./example_resources/def/noindex";
			break;
		default:
			std::cout << "This server does not support error " << error_code << std::endl;
			break;
	}
	std::string resp = fileToStr(error_file.c_str());
	response += intToStr(resp.size()) + "\r\n\r\n" + resp;
	
}

void Request::executeGetRequest()
{
	if (request_file_path.find("?") != std::string::npos)
	{
		query_string = request_file_path.substr(request_file_path.find("?") + 1, std::string::npos);
		request_file_path = request_file_path.substr(0, request_file_path.find("?"));
	}
	switch (checkAccess(request_file_path, R_OK))
	{
		case -1:
			formErrorResponse(404);
			return ;
		case 1:
			formErrorResponse(403);
			return ;
		default:
			break;
	}
	struct stat buf;
	stat(request_file_path.c_str(), &buf);
	if (!S_ISDIR(buf.st_mode))
	{
		for (std::map<std::string, std::string>::iterator it = server.getCGI().begin(); it != server.getCGI().end(); it++)
		{
			if (request_file_path.find("." + it->first) != std::string::npos && request_file_path.rfind("." + it->first) == request_file_path.size() - it->first.size() - 1)
			{
				executeCGI(it->first);
				return;
			}
		}
		std::ifstream file(request_file_path);
		std::string str, content;
		if (file.bad() || file.fail())
		{
			formErrorResponse(403);
			return ;
		}
		while (!file.eof())
		{
			getline(file, str);
			content += str;
			content.push_back('\n');
		}
		file.close();
		int i = content.size();
		std::ostringstream s;
		s << i;
		std::string content_len(s.str());
		if (Request::file_types.find(request_file_path.substr(request_file_path.rfind("."), std::string::npos)) != Request::file_types.end())
			response = std::string("HTTP/1.1 200 OK\r\nContent-Type: " + Request::file_types.find(request_file_path.substr(request_file_path.rfind("."), std::string::npos))->second + "\r\nContent-Length: ") + content_len + std::string("\r\n\r\n") + content;
		else
			response = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ") + content_len + std::string("\r\n\r\n") + content;
	}
	else
	{
		std::string index_path_file;
		if (loc_index >= 0)
			index_path_file = "." + server.getLocations()[loc_index].getRoot() + "/" + server.getLocations()[loc_index].getIndex();
		else
			index_path_file = "." + server.getRoot() + "/" + server.getIndex();
		if (checkAccess(index_path_file, R_OK))
		{
			if ((loc_index >= 0 && server.getLocations()[loc_index].getAutoIndex()) || (loc_index < 0 && server.getAutoIndex()))
			{
				if (original_request.back() != '/')
					original_request.push_back('/');
				std::string list_file = "<html><body>";
				DIR *dir_fd = opendir(request_file_path.c_str());
				if (!dir_fd)
				{
					formErrorResponse(403);
					return ;
				}
				struct dirent *dir_entry;
				dir_entry = readdir(dir_fd);
				list_file += "<a href=\"" + original_request + "\">" + std::string(dir_entry->d_name) + "</a></br>";
				dir_entry = readdir(dir_fd);
				std::string prev = original_request.substr(0, original_request.substr(0, original_request.size() - 1).rfind("/"));
				if (prev.empty())
					prev = "/";
				list_file += "<a href=\"" + prev + "\">" + std::string(dir_entry->d_name) + "</a></br>";
				while ((dir_entry = readdir(dir_fd)) != NULL)
				{
					if (request_file_path.back() != '/')
						request_file_path += "/";
					list_file += "<a href=" + std::string("\"") + original_request + std::string(dir_entry->d_name) + std::string("\"") + ">" + std::string(dir_entry->d_name) + "</a></br>";
				}
				closedir(dir_fd);
				list_file += "</body></html>";
				response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(list_file.size()) + "\r\n\r\n" + list_file;
				return ;
			}
			else
			{
				switch (checkAccess(index_path_file, R_OK))
				{
					case -1:
						formErrorResponse(600);
						return ;
					case 1:
						formErrorResponse(403);
						return ;
					default:
						break;
				}
			}
		}
		else
		{
			struct stat buf2;
			stat(index_path_file.c_str(), &buf2);
			if (S_ISDIR(buf2.st_mode))
			{
				formErrorResponse(403);
				return ;
			}
			std::ifstream file(index_path_file);
			std::string str, content;
			if (file.bad() || file.fail())
			{
				formErrorResponse(403);
				return ;
			}
			while (!file.eof())
			{
				getline(file, str);
				content += str;
				content.push_back('\n');
			}
			file.close();
			int i = content.size();
			std::ostringstream s;
			s << i;
			std::string content_len(s.str());
			response = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ") + content_len + std::string("\r\n\r\n") + content;
		}
	}
}

void Request::createPostFile(std::string name)
{
	std::string ext = "";

	if (request_file_path.back() != '/')
		request_file_path += "/";
	if (name.empty())
		name = "default_name";
	else
	{
		if (name.find(".") != std::string::npos)
		{
			ext = name.substr(name.find("."), name.size() - name.find("."));
			name = name.substr(0, name.find("."));
		}
	}
	switch (checkAccess(request_file_path, W_OK))
	{
		case -1:
			formErrorResponse(404);
			return ;
		case 1:
			formErrorResponse(403);
			return ;
		default:
			break;
	}
	std::string complete_path = request_file_path;
	if (loc_index >= 0 && !server.getLocations()[loc_index].getUpload().empty())
	{
		complete_path = "." + server.getLocations()[loc_index].getRoot() + server.getLocations()[loc_index].getUpload();
	}
	else if (( loc_index < 0 || (loc_index >= 0 && server.getLocations()[loc_index].getRoot().empty())) && !server.getUpload().empty())
	{
		complete_path = "." + server.getRoot() + server.getUpload();
	}
	if (complete_path.back() != '/')
		complete_path += "/";
	std::string complete_path_ext = complete_path + name + ext;
	int count = 0;
	if (!access(complete_path_ext.c_str(), F_OK))
	{
		complete_path_ext = complete_path + name + "_" + intToStr(count) + ext;
		while (!access(complete_path_ext.c_str(), F_OK))
		{
			count++;
			complete_path_ext = complete_path + name + "_" + intToStr(count) + ext;
		}
	}
	complete_path = complete_path_ext;
	std::ofstream new_file;
	new_file.open(complete_path);
	for (unsigned int i = 0; i < body_raw.size(); i++)
		new_file << body_raw[i];
	response = "HTTP/1.1 201 Created\r\nContent-Length: 0\r\nLocation: " + complete_path + "\r\n\r\n";
	return ;
}

void Request::executePostRequest()
{
	struct stat buf;
	std::string filename;

	if (access(request_file_path.c_str(), F_OK))
	{
		if (!access(request_file_path.substr(0, request_file_path.rfind("/")).c_str(), F_OK))
		{
			stat(request_file_path.substr(0, request_file_path.rfind("/")).c_str(), &buf);
			if (S_ISDIR(buf.st_mode))
			{
				filename = request_file_path.substr(request_file_path.rfind("/") + 1, std::string::npos);
				request_file_path = request_file_path.substr(0, request_file_path.rfind("/")) + "/";
				createPostFile(filename);
				return ;
			}
		}
		switch (checkAccess(request_file_path, W_OK))
		{
			case -1:
				formErrorResponse(404);
				return ;
			case 1:
				formErrorResponse(403);
				return ;
			default:
				break;
		}
		return ;
	}
	else
	{
		stat(request_file_path.c_str(), &buf);
		if (S_ISDIR(buf.st_mode))
			createPostFile("");
		else
		{
			for (std::map<std::string, std::string>::iterator it = server.getCGI().begin(); it != server.getCGI().end(); it++)
			{
				if (request_file_path.find("." + it->first) != std::string::npos && request_file_path.rfind("." + it->first) == request_file_path.size() - it->first.size() - 1)
				{
					query_string = body;
					executeCGI(it->first);
					return ;
				}
			}
			filename = request_file_path.substr(request_file_path.rfind("/") + 1, request_file_path.size() - (request_file_path.rfind("/") + 1));
			request_file_path = request_file_path.substr(0, request_file_path.rfind("/")) + "/";
			createPostFile(filename);
		}
	}
}

void Request::executeDeleteRequest()
{
	switch (checkAccess(request_file_path, W_OK))
	{
		case -1:
			formErrorResponse(404);
			return ;
		case 1:
			formErrorResponse(403);
			return ;
		default:
			break;
	}
	if (std::remove(request_file_path.c_str()))
	{
		formErrorResponse(403);
		return ;
	}
	/* std::string resp = fileToStr("def/delete");
	response = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ") + intToStr(resp.size()) + "\r\n\r\n" + resp; */
	formErrorResponse(2);
}

void Request::executeCGI(std::string type)
{
	(void)type;
	std::vector<std::string> var_vec = split(query_string, "&");
	std::vector<std::string> values;
	std::string value;
	char c;
	std::string hex;
	std::vector<std::string> var_val;
	for (unsigned int i = 0; i < var_vec.size(); i++)
	{
		value.clear();
		var_val = split(var_vec[i], "=");
		if (var_val.size() == 1)
			var_val.push_back("");
		while (var_val[1].find("+") != std::string::npos)
			var_val[1].replace(var_val[1].find("+"), 1, " ");
		for (unsigned int j = 0; j < var_val[1].size(); j++)
		{
			if (var_val[1][j] == '%')
			{
				hex = var_val[1].substr(j + 1, 2);
				c = hexToDec(hex);
				value.push_back(c);
				j += 2;
			}
			else
				value.push_back(var_val[1][j]);
		}
		values.push_back(value);
	}
	
	char *args[values.size() + 3];
	args[0] = (char *)server.getCGI()[type].c_str();
	args[1] = (char *)request_file_path.c_str();
	unsigned int i = 2;
	while (i < values.size() + 3 - 1)
	{
		args[i] = (char *)values[i - 2].c_str();
		i++;
	}
	args[i] = NULL;
	i = 0;
	while(args[i])
	{
		i++; 
	}
	int pipe_fd[2];
	if (pipe(pipe_fd))
	{
		formErrorResponse(500);
		return ;
	}
	int status;
	int childpid = fork(); 
	if (childpid < 0)
	{
		formErrorResponse(500);
		return ;
	}
	else if (childpid == 0)
	{
		std::time_t start = std::time(nullptr);
		std::time_t current = start;
		while (current - start < 1)
			current = std::time(nullptr);
		dup2(pipe_fd[1], STDOUT_FILENO);
		dup2(pipe_fd[1], STDERR_FILENO);
		execve(args[0], args, NULL);
		exit(1);
	}
	else
	{
		std::time_t start = std::time(nullptr);
		std::time_t current = start;			
		while (!waitpid(childpid, &status, WNOHANG) && current - start < 20)
			current = std::time(nullptr);
		close(pipe_fd[1]);
		if (kill(childpid, 0) == 0)
		{
			kill(childpid, SIGTERM);
			formErrorResponse(500);
			return ;
		}
		if (!WIFEXITED(status))
		{
			formErrorResponse(500);
			return ;
		}
		else
		{
			if (WEXITSTATUS(status) == 0)
			{
				char buff[101];
				int r;
				std::string output_cgi;
				while ((r = read(pipe_fd[0],buff, 100)))
				{
					buff[r] = 0;
					output_cgi += std::string(buff);
					for (int i = 0; i < 101; i++)
						buff[i] = 0;
				}
				close(pipe_fd[0]);
				if (output_cgi.find("<!DOCTYPE html>") != std::string::npos)
					response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(output_cgi.size()) + "\r\n\r\n" + output_cgi;
				else
					response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + intToStr(output_cgi.size()) + "\r\n\r\n" + output_cgi;
				return ;
			}
			else
			{
				close(pipe_fd[0]);
				formErrorResponse(500);
				return ;
			}
		}
	}
	return ;
}

void Request::formResponse()
{
	unsigned long pos_space = header.substr(header.find(" ") + 1, std::string::npos).find(" ");
	request_file = header.substr(header.find(" ") + 1, pos_space);
	original_request = request_file;
	selectLocation();
	if (!isAllowedMethod())
	{
		method = "GET";
		formErrorResponse(405);
	 	return ;
	}
	if (loc_index >= 0 && !server.getLocations()[loc_index].getRedirpath().empty())
	{
		request_file.replace(0, server.getLocations()[loc_index].getLocation().size(), server.getLocations()[loc_index].getRedirpath());
		if (server.getLocations()[loc_index].getRedircode() == "307")
			response = std::string("HTTP/1.1 307 Temporary Redirect\r\nContent-Length: 0\r\nLocation: ") + request_file + std::string("\r\n\r\n");
		else if (server.getLocations()[loc_index].getRedircode() == "303")
			response = std::string("HTTP/1.1 303 See Other\r\nContent-Length: 0\r\nLocation: ") + request_file + std::string("\r\n\r\n");
		else if (server.getLocations()[loc_index].getRedircode() == "301")
			response = std::string("HTTP/1.1 301 Moved Permanently\r\nContent-Length: 0\r\nLocation: ") + request_file + std::string("\r\n\r\n");
		return ;
	}
	else if (loc_index >= 0 && !server.getLocations()[loc_index].getRoot().empty())
	{
		request_file.replace(0, server.getLocations()[loc_index].getLocation().size(), server.getLocations()[loc_index].getRoot());
		request_file_path = std::string(".") + request_file;
	}
	else
		request_file_path = std::string(".") + server.getRoot() + request_file;
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
}

void Request::sendResponse()
{
	int w; 
	if (this->response.empty())
		this->formResponse();
	w = write(this->fd, this->response.c_str(), this->response.size());
	if (w <= 0)
	{
		error = true;
		return ;
	}
	//else if (w == 0)
		//throw std::runtime_error("Error: write response returned 0");
	// else
	// {
	// 	count++;
	// }
	if (response.size() < (size_t)w)
		response = response.substr(w, std::string::npos);
	else
		response = "";
	if (response.empty())
	{
		done_write = true;
	}
}

int Request::checkAccess(std::string path, int mode)
{
	std::vector<std::string> split_path = split(path, "/");
	std::string partial_path = split_path[0];
	for (unsigned int i = 0; i < split_path.size(); i++)
	{
		if (i != 0)
			partial_path += "/" + split_path[i];
		if (access(partial_path.c_str(), F_OK) == -1)
			return -1;
		switch (mode)
		{
			case R_OK:
				if (access(partial_path.c_str(), R_OK) == -1)
					return 1;
				break;
			case W_OK:
				if (access(partial_path.c_str(), W_OK) == -1)
					return 1;
				break;
			case X_OK:
				if (access(partial_path.c_str(), X_OK) == -1)
					return 1;
				break;
		}
	}
	return 0;
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

std::map<std::string, std::string> Request::initializeFileTypes()
{
	std::map<std::string, std::string> files;
	files[".png"] = "image/png";
	files[".jpeg"] = "image/jpeg";
	files[".html"] = "text/html";
	files[".pdf"] = "application/pdf";
	files[".php"] = "application/x-httpd-php";
	files[".txt"] = "text/plain";
	files[".gif"] = "image/gif";
	files[".css"] = "text/css";
	
	files[".aac	"] = "audio/aac";
	files[".abw	"] = "application/x-abiword";
	files[".apng"] = "image/apng";
	files[".arc"] = "application/x-freearc";
	files[".avif"] = "image/avif";
	files[".avi"] = "video/x-msvideo";
	files[".azw"] = "application/vnd.amazon.ebook";
	files[".bin"] = "application/octet-stream";
	files[".bmp"] = "image/bmp";
	files[".bz"] = "application/x-bzip";
	files[".bz2"] = "application/x-bzip2";
	files[".cda"] = "application/x-cdf";
	files[".csh"] = "application/x-csh";
	files[".csv"] = "text/csv";
	files[".doc"] = "application/msword";
	files[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	files[".eot"] = "application/vnd.ms-fontobject";
	files[".epub"] = "application/epub+zip";
	files[".gz"] = "application/gzip";
	files[".htm"] = "text/html";
	files[".ico"] = "image/vnd.microsoft.icon";
	files[".ics"] = "text/calendar";
	files[".jar"] = "application/java-archive";
	files[".jpg"] = "image/jpeg";
	files[".js"] = "text/javascript";
	files[".json"] = "application/json";
	files[".jsonld"] = "application/ld+json";
	files[".mid"] = "audio/midi";
	files[".midi"] = "audio/x-midi";
	files[".mjs"] = "text/javascript";
	files[".mp3"] = "audio/mpeg";
	files[".mp4"] = "video/mp4";
	files[".mpeg"] = "video/mpeg";
	files[".mpkg"] = "application/vnd.apple.installer+xml";
	files[".odp"] = "application/vnd.oasis.opendocument.presentation";
	files[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	files[".odt"] = "application/vnd.oasis.opendocument.text";
	files[".oga"] = "audio/ogg";
	files[".ogv"] = "video/ogg";
	files[".ogx"] = "application/ogg";
	files[".opus"] = "audio/opus";
	files[".otf"] = "font/otf";
	files[".png"] = "image/png";
	files[".ppt"] = "application/vnd.ms-powerpoint";
	files[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	files[".rar	"] = "application/vnd.rar";
	files[".rtf"] = "application/rtf";
	files[".sh"] = "application/x-sh";
	files[".svg"] = "image/svg+xml";
	files[".tar"] = "application/x-tar";
	files[".tif"] = "image/tiff";
	files[".tiff"] = "image/tiff";
	files[".ts"] = "video/mp2t";
	files[".ttf"] = "font/ttf";
	files[".vsd"] = "application/vnd.visio";
	files[".wav"] = "audio/wav";
	files[".weba"] = "audio/webm";
	files[".webm"] = "video/webm";
	files[".webp"] = "image/webp";
	files[".woff"] = "font/woff";
	files[".woff2"] = "font/woff2";
	files[".xhtml"] = "application/xhtml+xml";
	files[".xls"] = "application/vnd.ms-excel";
	files[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	files[".xml"] = "application/xml";
	files[".xul"] = "application/vnd.mozilla.xul+xml";
	files[".zip"] = "application/zip";
	files[".3gp"] = "video/3gpp";
	files[".3g2"] = "video/3gpp2";
	files[".7z"] = "application/x-7z-compressed";
	return files ;
}

std::map<std::string, std::string> Request::file_types = initializeFileTypes();
int Request::count = 0;