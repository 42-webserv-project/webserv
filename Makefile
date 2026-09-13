NAME := webserv
INCLUDE := -I src
BUILD_DIR := obj
SRC_DIR := src

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++17 $(INCLUDE)


SRC := $(SRC_DIR)/http_parser/http_parser.cpp \
		$(SRC_DIR)/http_response/http_response_state.cpp \
		$(SRC_DIR)/http_response/src_http_response.cpp \
		$(SRC_DIR)/main.cpp

OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(NAME) successfully built."

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled $< successfully."

clean:
	@rm -fr $(BUILD_DIR)
	@echo "Object files cleaned."

fclean: clean
	@rm -f $(NAME)
	@echo "Project cleaning is finished."

re: fclean all

TEST_DIR := tests
TEST_BUILD_DIR := $(BUILD_DIR)/tests

test: $(TEST_BUILD_DIR)/http_parser_test $(TEST_BUILD_DIR)/http_response_test
	@$(TEST_BUILD_DIR)/http_parser_test
	@$(TEST_BUILD_DIR)/http_response_test

$(TEST_BUILD_DIR)/http_parser_test: $(TEST_DIR)/http_parser_test.cpp $(SRC_DIR)/http_parser/http_parser.cpp
	@mkdir -p $(TEST_BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BUILD_DIR)/http_response_test: $(TEST_DIR)/http_response_test.cpp $(SRC_DIR)/http_response/http_response_state.cpp $(SRC_DIR)/http_response/src_http_response.cpp
	@mkdir -p $(TEST_BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: all clean fclean re test