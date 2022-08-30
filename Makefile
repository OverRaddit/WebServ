CC = c++
#CFLAGS = -Wall -Wextra -Werror -Iinclude
# -g3 -fsanitize=address
# -o2 or -o3
NAME = webserv
#B_NAME = cub3D_bonus

SRCS =	src/main.cpp src/Request/Request.cpp
#B_SRCS = 	src/main.c \

OBJS = $(SRCS:.cpp=.o)
#B_OBJS = $(B_SRCS:.c=.o)

# MLX_NAME = mlx
# MLX_DIR = include/minilibx_opengl_20191021

# GNL_NAME = gnl
# GNL_DIR = include/get_next_line

# LIBFT_NAME = ft
# LIBFT_DIR = include/libft

.PHONY : all clean fclean re

$(NAME) : $(OBJS)
#	$(MAKE) -C $(MLX_DIR) all
#	$(MAKE) -C $(GNL_DIR) all
#	$(MAKE) -C $(LIBFT_DIR) all
	$(CC) $(CFLAGS) $^ -o $@
#			-L$(MLX_DIR) -l$(MLX_NAME) \
#			-L$(GNL_DIR) -l$(GNL_NAME) \
#			-L$(LIBFT_DIR) -l$(LIBFT_NAME) \
			-framework OpenGL -framework AppKit $^ -o $@

# $(B_NAME) : $(B_OBJS)
# 	$(MAKE) -C $(MLX_DIR) all
# 	$(MAKE) -C $(GNL_DIR) all
# 	$(MAKE) -C $(LIBFT_DIR) all
# 	$(CC) $(CFLAGS) -L$(MLX_DIR) -l$(MLX_NAME) \
# 			-L$(GNL_DIR) -l$(GNL_NAME) \
# 			-L$(LIBFT_DIR) -l$(LIBFT_NAME) \
# 			-framework OpenGL -framework AppKit $^ -o $@

# $(SRCS_DIR)/%.o : $(SRCS_DIR)/%.c
# 	$(CC) $(CFLAGS) -I$(MLX_DIR) -I$(GNL_DIR) -I$(LIBFT_DIR) -c $< -o $@

all : $(NAME)

#bonus : $(B_NAME)

clean :
	@rm -rf $(OBJS)
#	$(MAKE) -C $(MLX_DIR) clean
#	$(MAKE) -C $(GNL_DIR) clean
#	$(MAKE) -C $(LIBFT_DIR) clean

fclean : clean
	@rm -rf $(NAME)
#	$(MAKE) -C $(MLX_DIR) clean
#	$(MAKE) -C $(GNL_DIR) fclean
#	$(MAKE) -C $(LIBFT_DIR) fclean

re :
	$(MAKE) fclean
	$(MAKE) all