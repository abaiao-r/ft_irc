# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: joao-per <joao-per@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/18 14:23:18 by abaiao-r          #+#    #+#              #
#    Updated: 2023/10/25 16:14:20 by joao-per         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Target (Name of the executable)
NAME	= ircserv

# Compiler settings
CXX	= c++
CXXFLAGS = -Wall -Wextra -Werror -Wshadow -std=c++98 -I includes -g #-fsanitize=address

# Directories
SRCDIR = src
OBJDIR = objs

# Source files
SRCS		= 	$(SRCDIR)/main.cpp \
				$(SRCDIR)/Commands.cpp \
				$(SRCDIR)/Handle_user.cpp \
				$(SRCDIR)/User.cpp \
				$(SRCDIR)/utils.cpp \
				$(SRCDIR)/Client.cpp

# Object files
OBJS	= $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Commands
RM		= rm -rf

# Colors

DEFAULT = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m
CURSIVE	= \e[33;3m


all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(MAGENTA)- Compiling $(NAME)... $(DEFAULT)"
	@$(CXX) $(OBJS) $(CXXFLAGS) -o  $(NAME)
	@printf "$(GREEN)- ircserv Compiled!$(DEFAULT)"

%.o: %.cpp
	@echo "$(CURSIVE)$(YELLOW)- Compiling $<... $(DEFAULT)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJDIR)
	@echo "$(RED)- OBJS Removed.$(DEFAULT)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)- ircserv Removed.$(DEFAULT)"

re: fclean all

run: fclean
		@./$(NAME)

valgrind: fclean all
		@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)

.PHONY: all clean fclean re run test valgrind