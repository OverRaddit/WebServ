# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jinyoo <jinyoo@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/30 18:07:40 by gshim             #+#    #+#              #
#    Updated: 2022/09/17 20:40:50 by jinyoo           ###   ########.fr        #
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
SRC_LIST	=	src/main.cpp			\
				src/Client/Client.cpp	\
				src/Request/Request.cpp \
				src/Server/Server.cpp	\
				src/Server/Callback.cpp	\
				src/Response/Response.cpp
#				src/webserv.cpp
#				src/util.cpp

SRCS		=	$(addprefix $(SRCS_DIR), $(SRC_LIST))
OBJS		=	$(SRCS:.cpp=.o)

# =============================================================================
# Target Generating
# =============================================================================
$(NAME)			:	$(OBJS)
	@echo $(GREEN) "Source files are compiled!\n" $(EOC)
	@echo $(WHITE) "Building $(NAME) for" $(YELLOW) "Mandatory" $(WHITE) "..." $(EOC)
	@$(CXX) $(CFLAGS) $^ -o $@
	@echo $(GREEN) "$(NAME) is created!\n" $(EOC)

$(SRCS_DIR)/%.o	:	$(SRCS_DIR)/%.cpp
	@echo $(YELLOW) "Compiling...\t" $< $(EOC) $(LINE_CLEAR)
	@$(CXX) $(CFLAGS) -c $< -o $@

# =============================================================================
# Rules
# =============================================================================
all			: $(NAME)

clean		:
				@echo $(YELLOW) "Cleaning object files..." $(EOC)
				@rm -rf $(OBJS)
				@echo $(RED) "Object files are cleaned! 🧹 🧹\n" $(EOC)

fclean		:
				@echo $(YELLOW) "Removing $(NAME)..." $(EOC)
				@rm -rf $(NAME) $(OBJS)
				@echo $(RED) "$(NAME) is removed! 🗑 🗑\n" $(EOC)

re			: fclean all

.PHONY		: all clean fclean re
