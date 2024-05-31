NAME = ircserv
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc

CPP = $(wildcard $(SRC_DIR)/*.cpp)
OOO = $(CPP:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

FLAG = -Wall -Wextra -Werror -std=c++98 -g -I$(INC_DIR)

all: $(NAME)

$(NAME): $(OOO)
	c++ $(OOO) $(FLAG) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	c++ -c $< $(FLAG) -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all
