# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dgessner <dgessner@student.42heilbronn.de> +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/19 12:00:50 by dgessner          #+#    #+#              #
#    Updated: 2025/05/24 17:38:56 by dgessner         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Name der Ausgabedatei
NAME = pipex

# Compiler und Flags
CC = cc
CFLAGS = -Wall -Werror -Wextra -I$(LIBFT_DIR)/includes

# Libft-Konfiguration
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# Quellcodedateien
SRC_DIR = src
SRCS = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/pipex.c \
	$(SRC_DIR)/helpers.c

# Objektdateien
OBJS = $(SRCS:.c=.o)

# Ziel 'all' – baut libft und das Hauptprogramm
all: $(NAME)

# Regel zum Erstellen der ausführbaren Datei
$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft -o $@
	chmod +x $@

# Regel zum Erstellen der libft.a
$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

# Regel zur Kompilierung der Objektdateien
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Ziel 'clean' – löscht Objektdateien
clean:
	rm -f $(OBJS)
	@$(MAKE) -C $(LIBFT_DIR) clean

# Ziel 'fclean' – löscht alles
fclean: clean
	rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

# Ziel 're' – neu bauen
re: fclean all

# Pseudotargets
.PHONY: all clean fclean re
