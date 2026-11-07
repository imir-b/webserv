
# --- shell ---
SHELL       = /bin/bash

# --- exe ---
NAME        = server

# --- compiler & flags ---
CXX         = c++
CXXFLAGS    = -Wall -Werror -Wextra -std=c++98

# --- directories ---
SRC_DIR     = srcs
OBJ_DIR     = objs

# --- files ---
SRC         = main.cpp client.cpp server.cpp
OBJ         = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
HPP         = $(wildcard $(SRC_DIR)/*.hpp)

# --- rules ---
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HPP) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
