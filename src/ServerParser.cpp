/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:21:26 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/11 11:28:46 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


ServerParser::ServerParser()
{

}

ServerParser::ServerParser(std::string conf)
{
	std::string::size_type	n;
	int						i = 0;
	int						j = 0;

	n = conf.find("location");
	if (n == std::string::npos)
		throw std::exception();
	else
	{
		while (n != std::string::npos)
		{
			i = n + 8;
			while (conf[i] && conf[i] != '{')
				i++;
			j = 1;
			while (conf[++i] && j != 0)
				if (conf[i] == '{')
					j++;
				else if (conf[i] == '}')
					j--;
			location.push_back(LocationParser(conf.substr(n, i - n)));
			n = conf.find("location", i);
		}
	}
	n = conf.find("listen");
	if (n == std::string::npos)
		port = 80;
	else
		while (conf[++n] && !isdigit(conf[n]))
			port = std::atoi(&conf[n]);
	n = conf.find("server_name");
	if (n == std::string::npos)
		s_name.push_back("host_name");
	else
	{
		i = n + 11;
		while (conf[i] == ' ' && conf[i])
			i++;
		n = i;
		while (conf[i] && conf[i] != '\n')
		{
			if (conf[i] == ' ' || conf[i] == ';')
			{
				s_name.push_back(conf.substr(n, i - n));
				n = i + 1;
			}
			i++;
		}
	}
	n = conf.find("error_page");
	if (n == std::string::npos)
		error[404] = "default_location";
	else
	{
		while (n != std::string::npos)
		{
			i = n;
			while (conf[i] != ';')
				i++;
			j = i;
			while (conf[j] != ' ')
				j--;
			while (conf[n] != ' ')
				n++;
			while (std::atoi(&conf[n]))
			{
				error[std::atoi(&conf[n])] = conf.substr(j, i - j);
				n += 1 + std::to_string(std::atoi(&conf[n])).length();
			}
			n = conf.find("error_page", n);
		}
	}
	n = conf.find("client_max_body_size");
	if (n == std::string::npos)
		c_size = 1;
	else
	{
		while (conf[n] != ' ')
			n++;
		c_size = std::atoi(&conf[n]);
	}
	//getInfo();
}

ServerParser::~ServerParser()
{

}

void	ServerParser::getInfo()
{
	std::cout << "Server" << std::endl << "Port : " << port << std::endl;
	std::vector<std::string>::iterator v_it = s_name.begin();
	while (v_it != s_name.end())
	{	
		std::cout << "Server name : " << *v_it << std::endl;
		++v_it;
	}
	std::map<int, std::string>::iterator m_it = error.begin();
	while (m_it != error.end())
	{
		std::cout << "Error page : " << m_it->first << " " << m_it->second << std::endl;
		++m_it;
	}
	std::cout << "Client size : " << c_size << std::endl;
	std::cout << std::endl;
}
int ServerConfiguration::getServerSocket() const
{
	return this->serverSocket;
}

void ServerConfiguration::addRequest(int fd)
{
	requestQueue.insert(std::pair<int, class Request>(fd, Request(fd)));
	return ;
}

