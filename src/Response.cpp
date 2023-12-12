/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 14:26:59 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/12 09:57:59 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

Response::Response()
{
	return ;
}

Response::Response(const Response& res): header(res.header), body(res.body)
{
	return ;
}

Response::~Response()
{
	return ;
}

Response &Response::operator=(const Response& res)
{
	if (this != &res)
	{
		this->header = res.header;
		this->body = res.body;
	}
	return (*this);
}