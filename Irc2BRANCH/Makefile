# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gacorrei <gacorrei@student.42lisboa.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/01 11:14:25 by gacorrei          #+#    #+#              #
#    Updated: 2023/11/10 14:57:43 by gacorrei         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= ircserv
SRC		= main.cpp Server.cpp Client.cpp Channel.cpp utils.cpp
CC		= @c++
# CFLAGS	= -Wall -Wextra -Werror -pedantic -Wshadow -g -std=c++98
CFLAGS	= -Wall -Wextra -Werror -pedantic -Wshadow -g -std=c++98 -fsanitize=address,undefined
# FOR DEBUGGER TO SHOW STD::STRING
# CFLAGS	= -Wall -Wextra -Werror -pedantic -Wshadow -g -fno-limit-debug-info -std=c++98 -fsanitize=address,undefined

all:		$(NAME)

$(NAME):	$(SRC)
			$(CC) $(CFLAGS) $(^) -o $(@)
clean:
			@rm -f $(OBJS)

fclean:		clean
			@rm -f $(NAME)

re:			fclean all

.PHONY:		all clean re fclean