# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/16 14:13:39 by gromero-          #+#    #+#              #
#    Updated: 2024/01/08 12:41:51 by gromero-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC			=	src/main.cpp src/WebServer.cpp src/LocationParser.cpp \
				src/Server.cpp src/ServerConfiguration.cpp \
				src/Error.cpp src/Request.cpp src/Response.cpp \
				src/miscellaneous.cpp

OBJ_SRC		=	 $(SRC:.cpp=.o)

EXEC		=	webserv

CC			=	c++  -std=c++98
CPPFLAGS	=	-Wall -Werror -Wextra

all:		$(EXEC)

$(NAME):	all

$(EXEC):	$(OBJ_SRC)
	$(CC)	$(CPPFLAGS)	$(OBJ_SRC)	-o	$(EXEC)

clean:
	rm -f $(OBJ_SRC)

fclean:
	make clean
	rm -f $(EXEC)

re:	fclean all

.PHONY: all clean fclean re
