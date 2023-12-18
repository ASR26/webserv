/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 11:23:25 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/18 10:55:47 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

int main(int argc, char **argv)
{
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
		//mywebserver.runWebserv();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
}

//add listening socket to waiting list
//in while loop check if socket is listening socket, if it is use accept and add new socket to waiting list
