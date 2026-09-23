NAME = ircserv
CC = c++
CFLAGS = -Wall -Wextra -Werror -I./includes -std=c++98
SRC =	src/main.cpp \
		src/server/Server.cpp \
		src/client/Client.cpp \
		src/Channel/Channel.cpp \
		src/parser.cpp \
		src/command/Command.cpp
		
OBJ_DIR = obj/
OBJ = $(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))

all: $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
	
$(NAME): $(OBJ)
	@$(CC) $(OBJ) $(CFLAGS) -o $(NAME)
	@echo "✅ $(NAME) ready !"

clean:
	@rm -rf $(OBJ) $(OBJ_DIR)
	@echo "✅ Removed object files"

fclean: clean
	@rm -f $(NAME)
	@echo "✅ Removed executable $(NAME)"

re: fclean all

.PHONY: all clean fclean re