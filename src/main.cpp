/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 11:23:25 by ysmeding          #+#    #+#             */
/*   Updated: 2024/01/09 14:32:48 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

int main(int argc, char **argv)
{
	(void)argv;
	if (argc > 2)
	{
		std::cout << "Error" << std::endl;
		std::exit (EXIT_FAILURE);
	}
	try
	{
		/*if (argc == 2)
			getConfig(argv[1]);
		else
			Server();*/
		WebServer mywebserver = WebServer();
		//ServerConfiguration servconf = ServerConfiguration();
		mywebserver.addServer(argv[1]);
		mywebserver.checkServerSpecification();
		mywebserver.configureServer();
		//mywebserver.addTestServer();
		mywebserver.runWebserv();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
}

//add listening socket to waiting list
//in while loop check if socket is listening socket, if it is use accept and add new socket to waiting list
