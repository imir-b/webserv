
# --- shell ---
SHELL       = /bin/bash

# --- exe ---
NAME        = server

# --- compiler & flags ---
CXX         = c++
CXXFLAGS    = -Wall -Werror -Wextra -std=c++98
CPPFLAGS    = -I$(INC_DIR)
DEPFLAGS    = -MMD -MP

# --- directories ---
SRC_DIR     = srcs
OBJ_DIR     = objs
INC_DIR     = headers

# --- files ---
SRC			= Main.cpp Client.cpp Server.cpp Request.cpp RequestParser.cpp RequestHandler.cpp Response.cpp
OBJ         = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
DEP         = $(OBJ:.o=.d)

# --- rules ---
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

-include $(DEP)

.PHONY: all clean fclean re
