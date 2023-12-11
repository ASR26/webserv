/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 14:25:59 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/01 15:12:21 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>

class Response
{
	private:
		std::string header;
		std::string body;
	public:
		Response();
		Response(const Response& res);
		~Response();

		Response &operator=(const Response& res);
};


#endif