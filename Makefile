# COLORS
RED				:= \033[31m
GREEN			:= \033[32m
YELLOW			:= \033[33m
BLUE			:= \033[34m
NC				:= \033[m



CXX				:= c++
DEBUG_MSG		:= false

# FLAGS
W_FLAGS			:= -Wall -Wextra -Werror
PTH_FLAGS		:=  -pthread
VERSION			:= -std=c++98 
SANITIZER		:= -g -fsanitize=address
# FDEBUG			:=  -D DEBUG=$(DEBUG_MSG)
FLAGS			:= $(W_FLAGS) $(PTH_FLAGS) $(VERSION) $(SANITIZER) # $(FDEBUG)

RM 				:= rm -f

# SRC
SRCS 			=  main.cpp \
					Config.cpp InputArgs.cpp RequestConfig.cpp ServerConfig.cpp \
					CGI.cpp Client.cpp Request.cpp Response.cpp Server.cpp \
					Base64.cpp File.cpp Logger.cpp MimeTypes.cpp StatusCode.cpp StringUtils.cpp Utils.cpp

# FOLDERS
OBJS_DIR		:= obj
BIN_DIR			:= bin

# OBJECTS
OBJS		= $(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))

#INCLUDES
INCLUDES 		:= src/ src/config src/http src/utils
INC			= $(addprefix -I , $(INCLUDES))

# DEPENDECES
DEPS				= $(patsubst %.o, %.d, $(OBJS)) 
DEPFLAGS			= -MMD -MP

# PROGRAM NAME
NAME 				:= webserv

# BINARY PATH
BIN = $(BIN_DIR)/$(NAME)

vpath %.cpp src src/http src/config src/utils

.SECONDEXPANSION:

all:$(BIN)

$(OBJS_DIR)/%.o: %.cpp | $$(@D)
	@echo "🔃 $(BLUE)Compiling" $(basename $(notdir $@)) "$(NC)"
	@$(CC) $(FLAGS) $(DEPFLAGS) $(INC) -c $< -o $@

$(BIN): $(OBJS) $(BIN_DIR)
	@$(CXX) $(FLAGS) $(INC) $(OBJS) -o $(BIN)
	@echo "✅ $(GREEN)$(NAME) compiled!$(NC)"

run: $(BIN)
	@./$(BIN)

leaks: $(BIN)
	@which valgrind | grep -o valgrind > /dev/null && \
	valgrind --leak-check=yes ./$(BIN) || \
	leaks -atExit -- ./$(BIN)

clean:
	@echo "🧹 $(RED)$(NAME) removing...$(NC)"
	@$(RM) $(OBJS) $(DEPS)
	@echo "✅ $(GREEN)$(NAME) cleaned!$(NC)"
	@$(RM) -rf  $(OBJS_DIR)

fclean:		clean
	@$(RM) $(BIN)
	@$(RM) -rf  $(BIN_DIR)

re:	fclean all

re_bonus: fclean bonus


$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

-include $(DEPS)

.PHONY: all clean fclean re leaks run
