/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 11:23:25 by ysmeding          #+#    #+#             */
/*   Updated: 2023/11/20 12:42:21 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

int main(int argc, char **argv)
{
	/* if (argc == 2)
		servconf(argv[1]);
	else if (argc == 1)
		ServerConfiguration servconf();
	else
		return 1; */
	try
	{
		WebServer mywebserver = WebServer();
		ServerConfiguration servconf = ServerConfiguration();
		mywebserver.addServer(servconf);
		mywebserver.runWebserv();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
}

//add listening socket to waiting list
//in while loop check if socket is listening socket, if it is use accept and add new socket to waiting list
