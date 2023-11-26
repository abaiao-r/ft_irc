# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gacorrei <gacorrei@student.42lisboa.com>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/01 11:14:25 by gacorrei          #+#    #+#              #
#    Updated: 2023/11/26 11:58:37 by gacorrei         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= ircserv
SRC		= main.cpp Server.cpp ServerConnection.cpp ServerCommands.cpp ServerUtils.cpp \
		Client.cpp Channel.cpp utils.cpp Bot.cpp
CC		= @c++
CFLAGS	= -Wall -Wextra -Werror -pedantic -Wshadow -g -std=c++98
# FOR DEBUGGER TO SHOW STD::STRING
CFLAGSD	= -Wall -Wextra -Werror -pedantic -Wshadow -g -fno-limit-debug-info -std=c++98 -fsanitize=address,undefined


all:		$(NAME)

$(NAME):	$(SRC)
			$(CC) $(CFLAGS) $(^) -o $(@)
clean:
			@rm -f $(OBJS)

fclean:		clean
			@rm -f $(NAME)

re:			fclean all

debug:		$(SRC)
			$(CC) $(CFLAGSD) $(^) -o $(NAME)

valgrind:	$(NAME)
			valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$(NAME) 6697 pass

.PHONY:		all clean re fclean