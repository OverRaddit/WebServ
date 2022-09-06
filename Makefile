# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gshim <gshim@student.42seoul.kr>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/30 18:07:40 by gshim             #+#    #+#              #
#    Updated: 2022/09/01 10:42:47 by gshim            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# =============================================================================
# Color Variables
# =============================================================================
BLACK		= 	"\033[0;30m"
GRAY		= 	"\033[1;30m"
RED			=	"\033[0;31m"
GREEN		=	"\033[0;32m"
YELLOW		=	"\033[1;33m"
PURPLE		=	"\033[0;35m"
CYAN		=	"\033[0;36m"
WHITE		=	"\033[1;37m"
EOC			=	"\033[0;0m"
LINE_CLEAR	=	"\x1b[1A\x1b[M"

# =============================================================================
# Command Variables
# =============================================================================
CXX			=	c++
#CFLAGS		=	-Wall -Wextra -Werror -std=c++98
CDEBUG		=	-g -fsanitize=address

# =============================================================================
# File Variables
# =============================================================================
NAME		=	webserv
SRCS_DIR	=	./
SRC_LIST	=	src/webserv.cpp			\
				src/Client/Client.cpp	\
				src/Request/Request.cpp
#				src/Request.cpp
#				src/util.cpp

SRCS		=	$(addprefix $(SRCS_DIR), $(SRC_LIST))
OBJS		=	$(SRCS:.cpp=.o)

GNL_NAME = gnl
GNL_DIR = include/get_next_line

# =============================================================================
# Target Generating
# =============================================================================
$(NAME)			:	$(OBJS)
	@$(MAKE) -C $(GNL_DIR) all
	@echo $(GREEN) "Source files are compiled!\n" $(EOC)
	@echo $(WHITE) "Building $(NAME) for" $(YELLOW) "Mandatory" $(WHITE) "..." $(EOC)
	$(CXX) $(CFLAGS) -L$(GNL_DIR) -l$(GNL_NAME) $^ -o $@
	@echo $(GREEN) "$(NAME) is created!\n" $(EOC)

$(SRCS_DIR)/%.o	:	$(SRCS_DIR)/%.cpp
	@echo $(YELLOW) "Compiling...\t" $< $(EOC) $(LINE_CLEAR)
	$(CXX) $(CFLAGS) -I$(GNL_DIR) -c $< -o $@

# =============================================================================
# Rules
# =============================================================================
all			: $(NAME)

clean		:
				@echo $(YELLOW) "Cleaning object files..." $(EOC)
				@rm -rf $(OBJS)
				$(MAKE) -C $(GNL_DIR) clean
				@echo $(RED) "Object files are cleaned! 🧹 🧹\n" $(EOC)

fclean		:
				@echo $(YELLOW) "Removing $(NAME)..." $(EOC)
				@rm -rf $(NAME) $(OBJS)
				$(MAKE) -C $(GNL_DIR) fclean
				@echo $(RED) "$(NAME) is removed! 🗑 🗑\n" $(EOC)

re			: fclean all

.PHONY		: all clean fclean re
