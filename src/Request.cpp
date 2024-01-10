/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 13:59:41 by ysmeding          #+#    #+#             */
/*   Updated: 2024/01/10 11:47:35 by ysmeding         ###   ########.fr       */
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

Server Request::returnServerOfRequest(std::vector<class Server> servers)
{
	std::vector<int> index;
	
	unsigned long pos_start = header.find("Host: ");
	unsigned long pos_end = header.substr(pos_start, std::string::npos).find("\r\n");
	this->host = (header.substr(pos_start + std::strlen("Host: "), pos_end - std::strlen("Host: ")));
	//this->host = header.substr(pos_start + std::strlen("Host: "), pos_end - std::strlen("Host: "));
	std::string port = host.substr(host.find(":") + 1, std::string::npos);
	for (unsigned long i = 0; i < servers.size(); i++)
	{
		//std::cout << "port is: " << port << " server port is: " << servers[i].getPort() << std::endl;
		if (port == servers[i].getPort())
			index.push_back(i);
	}
	//std::cout << index.size() << std::endl;
	std::string server_name;
	pos_end = host.rfind(":");
	//std::cout << "host: " << req.getHost()[pos_end - 1] << std::endl;
	if (!std::isdigit(host[pos_end - 1]))
	{
		pos_end = host.find(".localhost");
		if (pos_end != std::string::npos)
		//std::cout << "pos end -> " << pos_end << std::endl;
		//std::cout << req.getHost() << std::endl;
			server_name = host.substr(0, pos_end);
		//std::cout << server_name << std::endl;
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
	//std::cout << server_name << std::endl;
	if (server_name.empty())
	{
		return servers[index[0]];
	}
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
		{
			throw std::runtime_error("No server found");
		}
		return servers[0];
	}
	
}

void Request::readRequest(std::vector<class Server> servers)
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
	std::cout << "chars: " << r << std::endl;
	std::cout << "READ: " << buf << std::endl;
	request += std::string(buf);
	if (header.empty() && (pos = request.find("\r\n\r\n")) != std::string::npos)
	{
		this->header = request.substr(0, pos);
		//std::cout << this->header << std::endl;
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
						response = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n"; 
						return ;
					}
				}
				else
				{
					if (this->body_size > (unsigned int)srv.getCSize())
					{
						done_read = true;
						response = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n"; 
						return ;
					}
				}
			}
			catch(const std::exception& e)
			{
			}
			
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

void Request::executeGetRequest()
{
	if (request_file_path.find("?") != std::string::npos)
	{
		query_string = request_file_path.substr(request_file_path.find("?") + 1, std::string::npos);
		request_file_path = request_file_path.substr(0, request_file_path.find("?"));
		std::cout << query_string << std::endl;
		std::cout << request_file_path << std::endl;
		//separate query string
	}
	if (access(request_file_path.c_str(), F_OK))
	{
		std::string resp = fileToStr("def/404");
		response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
		return ;
	}
	struct stat buf;
	stat(request_file_path.c_str(), &buf);//check what happens if not all directories in path can be executed
	if (!S_ISDIR(buf.st_mode))
	{
		//check if need cgi
		std::ifstream file(request_file_path);
		std::string str, content;
		if (file.bad() || file.fail())
		{
			std::string resp = fileToStr("def/403");
			response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
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
	else
	{
		//std::cout << "IS DIRECTORY" << std::endl;
		if ((loc_index >= 0 && server.getLocations()[loc_index].getAutoIndex()) || server.getAutoIndex())
		{
			//std::cout << "LISTING DIRECTORY" << std::endl;
			//response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 51\r\n\r\nwhy are you asking for a directory? You get a list.";
			//list directory here
			std::string list_file = "<html><body>";
			DIR *dir_fd = opendir(request_file_path.c_str());
			if (!dir_fd)
			{
				std::string resp = fileToStr("def/403");
				response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
				return ;
			}
			struct dirent *dir_entry;
			dir_entry = readdir(dir_fd);
			std::cout << "<a href=" + host + request_file_path.substr(1, std::string::npos) + ">" + std::string(dir_entry->d_name) + "</a></br>" << std::endl;
			list_file += "<a href=" + host + request_file_path.substr(1, std::string::npos) + ">" + std::string(dir_entry->d_name) + "</a></br>";
			dir_entry = readdir(dir_fd);
			std::cout << "<a href=" + host + request_file_path.substr(1, request_file_path.rfind("/") - 1) + ">" + std::string(dir_entry->d_name) + "</a></br>" << std::endl;
			list_file += "<a href=" + host + request_file_path.substr(1, request_file_path.rfind("/") - 1) + ">" + std::string(dir_entry->d_name) + "</a></br>";
			while ((dir_entry = readdir(dir_fd)) != NULL)
			{
				//std::cout << dir_entry->d_name << std::endl;
				//std::cout << "<a href=" + host + request_file_path.substr(1, std::string::npos) + std::string(dir_entry->d_name) + ">" + std::string(dir_entry->d_name) + "</a>" << std::endl;ç
				if (request_file_path.back() != '/')
					request_file_path += "/";
				list_file += "<a href=" + std::string("\"") + host + request_file_path.substr(1, std::string::npos) + std::string(dir_entry->d_name) + std::string("\"") + ">" + std::string(dir_entry->d_name) + "</a></br>";
			}
			closedir(dir_fd);
			list_file += "</body></html>";
			response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(list_file.size()) + "\r\n\r\n" + list_file;
			return ;
		}
		else
		{
			if (loc_index >= 0 && !server.getLocations()[loc_index].getRoot().empty())
			{
				request_file_path = std::string(".") + server.getLocations()[loc_index].getRoot() + std::string("/") + server.getLocations()[loc_index].getIndex();
				if (access(request_file_path.c_str(), F_OK))
				{
					std::string resp = fileToStr("def/404");
					response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
					return ;
				}
				std::ifstream file(request_file_path);
				std::string str, content;
				if (file.bad() || file.fail())
				{
					std::string resp = fileToStr("def/403");
					response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
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
			else
			{
				request_file_path = std::string(".") + server.getRoot() + std::string("/") + server.getIndex();
				if (access(request_file_path.c_str(), F_OK))
				{
					std::string resp = fileToStr("def/404");
					response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
					return ;
				}
				std::ifstream file(request_file_path);
				std::string str, content;
				if (file.bad() || file.fail())
				{
					std::string resp = fileToStr("def/403");
					response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
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
		//response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 35\r\n\r\nwhy are you asking for a directory?";
	}
}

void Request::createPostFile(std::string name)
{
	//(void)name;
	//std::cout << "location:" << request_file_path << "<-" << std::endl; 
	//std::cout << "file:" << name << "<-" << std::endl;


	if ((loc_index >= 0 && body.size() > (unsigned int)(server.getLocations()[loc_index].getCSize() * 1024)) || (body.size() > (unsigned int)(server.getCSize() * 1024)))
	{
		response = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\nContent-Length: 15\r\n\r\nerror from post";
		return;
	}


	if (request_file_path.back() != '/')
		request_file_path += "/";
	if (name.empty())
		name = "default_name";
	std::string complete_path = request_file_path + name;
	if (loc_index >= 0 && !server.getLocations()[loc_index].getUpload().empty())
	{
		complete_path = "." + server.getLocations()[loc_index].getRoot() + server.getLocations()[loc_index].getUpload();
		std::cout << "case 1: " << complete_path << std::endl;
	}
	else if (loc_index >= 0 && server.getLocations()[loc_index].getRoot().empty() && !server.getUpload().empty())
	{
		complete_path = "." + server.getRoot() + server.getUpload();
		std::cout << "case 2: " << complete_path << std::endl;
	}
	else if (!server.getUpload().empty())
	{
		complete_path = "." + server.getRoot() + server.getUpload();
		std::cout << "case 3: " << complete_path << std::endl;
	}
	if (complete_path.back() != '/')
		complete_path += "/";
	complete_path += name;
	std::string complete_path_ext;
	int count = 0;
	if (!access(complete_path.c_str(), F_OK))
	{
		complete_path_ext = complete_path + "_" + intToStr(count);
		while (!access(complete_path_ext.c_str(), F_OK))
		{
			count++;
			complete_path_ext = complete_path + "_" + intToStr(count);
		}
		complete_path = complete_path_ext;
	}
	std::cout << "new file: " << complete_path << std::endl;
	std::cout << "body: " << body << std::endl;
	std::ofstream new_file;
	new_file.open(complete_path);
	new_file << body;
	response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 15\r\n\r\nhello from post";
	return ;
}

void Request::executePostRequest()
{
	struct stat buf;
	std::string filename;
	if (access(request_file_path.c_str(), F_OK))
	{
		std::cout << request_file_path.substr(0, request_file_path.rfind("/")).c_str() << std::endl;

		if (!access(request_file_path.substr(0, request_file_path.rfind("/")).c_str(), F_OK))
		{
			stat(request_file_path.substr(0, request_file_path.rfind("/")).c_str(), &buf);
			if (S_ISDIR(buf.st_mode))
			{
				std::cout << "POST with file and path exists" << std::endl;
				filename = request_file_path.substr(request_file_path.rfind("/") + 1, std::string::npos);
				request_file_path = request_file_path.substr(0, request_file_path.rfind("/")) + "/";
				createPostFile(filename);
				return ;
			}
		}
		std::cout << "POST with file and path does not exists" << std::endl;
		std::string resp = fileToStr("def/404");
		response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
		return ;
	}
	else
	{
		stat(request_file_path.c_str(), &buf);
		if (S_ISDIR(buf.st_mode))
		{
			std::cout << "POST without file and path exists" << std::endl;
			createPostFile("");
		}
		else
		{
			std::cout << "POST with existing file and path exists: " << request_file_path << std::endl;
			filename = request_file_path.substr(request_file_path.rfind("/") + 1, request_file_path.size() - (request_file_path.rfind("/") + 1));
			request_file_path = request_file_path.substr(0, request_file_path.rfind("/")) + "/";
			createPostFile(filename);
		}
	}
}

void Request::executeDeleteRequest()
{
	//response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 17\r\n\r\nhello from delete";
	if (access(request_file_path.c_str(), F_OK))
	{
		std::string resp = fileToStr("def/404");
		response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
		return ;
	}
	if (std::remove(request_file_path.c_str()))
	{
		std::string resp = fileToStr("def/403");
		response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
		return ;
	}
	std::string resp = fileToStr("def/delete");
	response = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ") + intToStr(resp.size()) + "\r\n\r\n" + resp;
}

void Request::formResponse()
{
	//server.getInfo();
	unsigned long pos_space = header.substr(header.find(" ") + 1, std::string::npos).find(" ");
	request_file = header.substr(header.find(" ") + 1, pos_space);
	// /* if (access(request_file_path.c_str(), F_OK))
	// {
	// 	response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 9\r\n\r\nnot found";
	// 	return ;
	// } */
	selectLocation();
	if (!isAllowedMethod())
	{
		std::string resp = fileToStr("def/405");
		method = "GET";
		response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: " + intToStr(resp.size()) + "\r\n\r\n" + resp;
	 	return ;
	}
	if (loc_index >= 0 && !server.getLocations()[loc_index].getRedirpath().empty())
	{
		request_file.replace(0, server.getLocations()[loc_index].getLocation().size(), server.getLocations()[loc_index].getRedirpath());
		//request_file_path = std::string(".") + request_file;
		//std::cout << request_file_path << std::endl;
		std::cout << request_file << std::endl;
		//std::cout << request_file_path << std::endl;
		if (server.getLocations()[loc_index].getRedircode() == "307")
			response = std::string("HTTP/1.1 307 Temporary Redirect\r\nContent-Length: 0\r\nLocation: ") + request_file + std::string("\r\n\r\n");
		else if (server.getLocations()[loc_index].getRedircode() == "303")
			response = std::string("HTTP/1.1 303 See Other\r\nContent-Length: 0\r\nLocation: ") + request_file + std::string("\r\n\r\n");
		else if (server.getLocations()[loc_index].getRedircode() == "301")
			response = std::string("HTTP/1.1 301 Moved Permanently\r\nContent-Length: 0\r\nLocation: ") + request_file + std::string("\r\n\r\n");
		
		// if (access(request_file.c_str(), F_OK))
		// {
		// 	if (method == "POST")
		// 		response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length:11\r\n\r\nbad request";
		// 	else
		// 		response = std::string("HTTP/1.1 307 Temporary Redirect\r\nContent-Length: 0\r\nLocation: ") + request_file + std::string("\r\n\r\n");
		// }
		// else
		// {
		// 	if (method == "GET" || method == "DELETE")
		// 		response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 9\r\n\r\nnot found";
		// 	else
		// 	{
		// 		if (access(request_file.substr(0, request_file.rfind("/")).c_str(), F_OK))
		// 			response = std::string("HTTP/1.1 307 Temporary Redirect\r\nContent-Length: 0\r\nLocation: ") + request_file.substr(0, request_file.rfind("/")) + std::string("\r\n\r\n");
		// 		else
		// 			response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 9\r\n\r\nnot found";
		// 	}
		// }
		return ;
	}
	// else if (!server.getRedirpath().empty())
	// {
	// 	request_file.replace(0, , server.getRedirpath());
	// 	//request_file_path = std::string(".") + request_file;
	// 	std::cout << request_file_path << std::endl;
	// 	response = std::string("HTTP/1.1 307 Temporary Redirect\r\nContent-Length: 0\r\nLocation: ") + request_file + std::string("\r\n\r\n");
	// 	/* if (access(request_file.c_str(), F_OK))
	// 	{
	// 		if (method == "POST")
	// 			response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length:11\r\n\r\nbad request";
	// 		else
	// 			response = std::string("HTTP/1.1 307 Temporary Redirect\r\nContent-Length: 0\r\nLocation: ") + request_file + std::string("\r\n\r\n");
	// 	}
	// 	else
	// 	{
	// 		if (method == "GET" || method == "DELETE")
	// 			response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 9\r\n\r\nnot found";
	// 		else
	// 		{
	// 			if (access(request_file.substr(0, request_file.rfind("/")).c_str(), F_OK))
	// 				response = std::string("HTTP/1.1 307 Temporary Redirect\r\nContent-Length: 0\r\nLocation: ") + request_file.substr(0, request_file.rfind("/")) + std::string("\r\n\r\n");
	// 			else
	// 				response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 9\r\n\r\nnot found";
	// 		}
	// 	} */
	// 	return ;
	// }
	else if (loc_index >= 0 && !server.getLocations()[loc_index].getRoot().empty())
	{
		request_file.replace(0, server.getLocations()[loc_index].getLocation().size(), server.getLocations()[loc_index].getRoot());
		request_file_path = std::string(".") + request_file;
	}
	else
		request_file_path = std::string(".") + server.getRoot() + request_file;
	// //std::cout << request_file_path << std::endl;
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