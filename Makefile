NAME		= web_serv

CC			= c++
FLAGS		= -Wall -Wextra -Werror -std=c++98
DEBUG_FLAGS = -D DEBUG=1
RM			= rm -rf

OBJDIR = .objFiles
SRCDIR = src
INCDIR = inc

FILES		= main Config Utils Client Server Supervisor Response
SRC			= $(addprefix $(SRCDIR)/, $(FILES:=.cpp))
OBJ			= $(addprefix $(OBJDIR)/, $(FILES:=.o))
HEADER		= $(addprefix $(INCDIR)/, WebServ.hpp Config.hpp Utils.hpp Client.hpp Server.hpp Supervisor.hpp Response.hpp)

# Colors:
GREEN		=	\e[92;5;118m
YELLOW		=	\e[93;5;226m
GRAY		=	\e[33;2;37m
RESET		=	\e[0m
CURSIVE		=	\e[33;3m

.PHONY: all clean fclean re debug norm

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(OBJ) -o $(NAME)
	@printf "$(GREEN)- Executable ready.\n$(RESET)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) -c $< -o $@ -I$(INCDIR)

debug: FLAGS += $(DEBUG_FLAGS)
debug: re

clean:
	@$(RM) $(OBJDIR)
	@printf "$(YELLOW)- Object files removed.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)- Executable removed.$(RESET)\n"

re: fclean all
