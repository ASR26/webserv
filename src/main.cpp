/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/06 11:23:25 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/12 13:36:00 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

/*void	getConfig(std::string conf)
{
	std::string::size_type n;
	std::fstream fd;
	std::string str;
	std::string file;
	std::vector<Server> sv;

	n = conf.find(".conf");
	if (std::string::npos == n)
		throw std::exception();
	fd.open (conf, std::fstream::in);
	if (fd.is_open())
		while (std::getline(fd, str))
		{
			file += str;
			file.push_back('\n');
		}
	else
		throw std::exception();
	n = file.find("server");
	while (n != std::string::npos)
	{
		int	i = n + 5;
		while (file[++i] && file[i] != '{')
			if (file[i] != ' ' && file[i] != '\t')
				throw std::exception();
		i = n;
		while (file[i] && file[i] != '{')
			i++;
		int cont = 1;
		while (file[++i] && cont != 0)
			if (file[i] == '{')
				cont++;
			else if (file[i] == '}')
				cont--;
		sv.push_back(Server(file.substr(n, i - n)));
		n = file.find("server", i);
	}
	fd.close();
}*/

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
		mywebserver.runWebserv();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
}

//add listening socket to waiting list
//in while loop check if socket is listening socket, if it is use accept and add new socket to waiting list
