/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 11:11:11 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/11 11:19:22 by gromero-         ###   ########.fr       */
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



#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <iostream>
#include <stdexcept>


#include "ServerParser.hpp"
#include "LocationParser.hpp"

#include "WebServer.hpp"
#include "ServerConfiguration.hpp"
//#include "Error.hpp"
#include "Request.hpp"
#include "Response.hpp"

#endif
