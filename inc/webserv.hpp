/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 11:11:11 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/17 10:16:26 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

/* execve dup dup2 pipe fork close read write access */
#include <unistd.h>
/* strerror  */
#include <string.h>
/* gai_strerror */
#include <netdb.h>
/* errno */
#include <errno.h>
/* htons htonl ntohs ntohl */
#include <arpa/inet.h>
/* select */
#include <sys/select.h>
/* poll */
#include <poll.h>
/* socket accept listen send recv bind connect getaddrinfo freeaddrinfo setsockopt getsockname */
#include <sys/socket.h>
/* getaddrinfo freeaddrinfo */
#include <sys/types.h>
/* getaddrinfo freeaddrinfo getprotobyname */
#include <netdb.h>
/* fcntl */
#include <fcntl.h>
/* waitpid */
#include <sys/wait.h>
/* kill signal */
#include <signal.h>
/* kqueue kevent */
#include <sys/event.h>
#include <sys/stat.h>



#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>


#include "Server.hpp"
#include "LocationParser.hpp"

#include "WebServer.hpp"
#include "ServerConfiguration.hpp"
#include "Error.hpp"
#include "Request.hpp"
#include "Response.hpp"


size_t nbrEqualChar(std::string str1, std::string str2);
size_t nbrEqualStr(std::vector<std::string> vecstr1, std::vector<std::string> vecstr2);
std::vector<std::string> split(std::string str, std::string c);

#endif
