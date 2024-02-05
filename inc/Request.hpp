/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 13:55:59 by ysmeding          #+#    #+#             */
/*   Updated: 2024/02/05 08:42:27 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include "Server.hpp"

class Request
{
	private:
		int					fd;
		size_t				body_size;
		std::string			content_type;
		std::string			request;
		std::string			header;
		std::string			body;
		std::vector<char>	body_raw;
		std::string			method;
		std::string			response;
		std::string			original_request;
		class Server		server;
		std::string			host;
		std::string			request_file;
		std::string			request_file_path;
		int					loc_index;
		std::string			query_string;
		
		static std::map<std::string, std::string> file_types;
		void setMethod();
	public:
		Request();
		Request(int fd);
		Request(const Request& req);
		~Request();

		Request& operator=(const Request& req);

		Server	returnServerOfRequest(std::vector<class Server> servers);
		int		returnLocationIndex(std::string file, Server srv);
		void	readRequest(std::vector<class Server> servers);
		void	sendResponse();
		void	formResponse();
		void	selectLocation();
		bool	isAllowedMethod();
		void	createPostFile(std::string);
		void 	executeGetRequest();
		void 	executePostRequest();
		void 	executeDeleteRequest();
		void 	formErrorResponse(int);
		void 	executeCGI(std::string type);
		void	processHeader(std::vector<class Server> servers, int r, char *buf, unsigned long pos);
		int		checkAccess(std::string path, int mode);
		
		std::string	getHeader() const;
		std::string	getHost() const;

		void	setServer(class Server&);
		void	setHost(std::string);
		void	setResponse(std::string);

		bool done_read;
		bool done_write;
		bool error;

		static std::map<std::string, std::string> initializeFileTypes();
		static int count;
};
#endif