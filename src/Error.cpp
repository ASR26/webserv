/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 13:11:05 by ysmeding          #+#    #+#             */
/*   Updated: 2023/11/23 15:32:30 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

void Error::functionError()
{
	std::cout << std::strerror(errno) << std::endl;
	return ;
}

void Error::functionError(std::string errstr)
{
	std::cout << errstr << std::endl;
	return ;
}

std::string Error::responseCode(int err)
{
	switch (err)
	{
		case 200:
			return "200 OK";
		case 404:
			return "404 Not Found";
		case 405:
			return "405 Method Not Allowed";
		default:
			return "...";
	}
}