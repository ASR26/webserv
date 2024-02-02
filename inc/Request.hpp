/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 13:55:59 by ysmeding          #+#    #+#             */
/*   Updated: 2024/01/29 12:21:51 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include "Server.hpp"

class Request
{
	private:
		int fd;
		size_t body_size;
		//std::string boundary;
		//std::string boundary_end;
		//bool multipart;
		std::string content_type;
		std::string request;
		std::string header;
		std::string body;
		std::vector<char> body_raw;
		//int body_raw_size;
		std::string method;
		std::string response;
		std::string original_request;
		void setMethod();

		class Server server;

		std::string host;
		std::string request_file;
		std::string request_file_path;
		int loc_index;

		std::string query_string;
		static std::map<std::string, std::string> file_types;
	public:
		Request();
		Request(int fd);
		Request(const Request& req);
		~Request();

		bool done_read;
		bool done_write;
		bool error;
		Request& operator=(const Request& req);
		Server returnServerOfRequest(std::vector<class Server> servers);
		int returnLocationIndex(std::string file, Server srv);
		void readRequest(std::vector<class Server> servers);
		void sendResponse();
		void formResponse();
		//void responseGet();
		//void responsePost();
		//void responseDelete();
		void selectLocation();
		bool isAllowedMethod();
		void createPostFile(std::string);

		void executeGetRequest();
		void executePostRequest();
		void executeDeleteRequest();
		

		std::string getHeader() const;
		void setServer(class Server&);
		void setHost(std::string);
		std::string getHost() const;
		void setResponse(std::string);
		void formErrorResponse(int);
		void executeCGI(std::string type);

		static std::map<std::string, std::string> initializeFileTypes();
		static int count;

};
#endif