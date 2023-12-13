/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 13:55:59 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/13 13:58:03 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>

class Request
{
	private:
		int fd;
		size_t body_size;
		std::string request;
		std::string header;
		std::string body;
		std::string method;
		std::string response;
		void setMethod();
	public:
		Request();
		Request(int fd);
		Request(const Request& req);
		~Request();

		bool done_read;
		bool done_write;
		Request& operator=(const Request& req);
		void readRequest();
		void sendResponse();
		void formResponse();
};

#endif