/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 13:55:59 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/01 11:29:51 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>

class Request
{
	private:
		std::string header;
		std::string body;
		std::string method;
		void setMethod();
	public:
		Request();
		Request(int fd);
		Request(const Request& req);
		~Request();

		Request& operator=(const Request& req);

};

#endif