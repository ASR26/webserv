/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 11:23:25 by ysmeding          #+#    #+#             */
/*   Updated: 2024/02/03 11:01:59 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

int main(int argc, char **argv)
{
	(void)argv;
	if (argc > 2)
	{
		std::cerr << "\033[1;31mError\033[0m: Too many arguments." << std::endl;
		std::exit (EXIT_FAILURE);
	}
	try
	{
		WebServer mywebserver = WebServer();
		if (argc == 2)
			mywebserver.addServer(argv[1]);
		else
			mywebserver.addServer("./conf/peps.conf");
		mywebserver.checkServerSpecification();
		mywebserver.configureServer();
		mywebserver.runWebserv();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
}
