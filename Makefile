# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: scarpent <scarpent@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/16 15:51:04 by llaigle           #+#    #+#              #
#    Updated: 2024/05/27 14:05:14 by scarpent         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
CPP = main.cpp\
	Clients.cpp\
	Server.cpp\
	Channel.cpp\
	Commands.cpp

OOO = $(CPP:.cpp=.o)

FLAG = -Wall -Wextra -Werror -std=c++98 -g

all: $(NAME)

$(NAME): $(OOO)
	c++ $(OOO) $(FLAG) -o $(NAME)
$(OOO): $(CPP)
	c++ -c $(CPP) $(FLAG)
clean:
	rm -f $(OOO)
fclean: clean
	rm -f $(NAME)
re: fclean all