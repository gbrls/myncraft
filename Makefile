DIR := src

# binary output directory name
OUTPUTDIR := bin

NAME := Myncraft

# add C flags, like: CFLAGS := -Wall -Werror -Wconversion -Wextra
CFLAGS :=

# add load flags, others like -pthread
LDLIB := -lm -lSDL2 -lGL -lGLEW

CC := g++ -std=c++17
RM := rm -f
MK := mkdir -p

EXT := cpp

SRC := $(shell find $(DIR) -type f -name *.$(EXT))

OBJS := $(subst .$(EXT),.o,$(SRC))

all: mkdirs buildClient runClient

debug: mkdirs buildClientDebug runClient

game: mkdirs buildClient clean runClient

run: runClient

buildClient: $(OBJS)
	@echo "Linking $(NAME)..."
	$(CC) -o $(OUTPUTDIR)/$(NAME) $(OBJS) $(LDLIB) $(CFLAGS)

buildClientDebug: $(OBJS)
	@echo "Linking (DEBUG) $(NAME)..."
	$(CC) -o $(OUTPUTDIR)/$(NAME) $(OBJS) $(LDLIB) $(CFLAGS) -fsanitize=address

%.o : %.$(EXT)
	$(CC) -c $< -o $@ $(CFLAGS) -MD -MP

mkdirs:
	$(MK) $(OUTPUTDIR)

clean:
	@echo "Cleaning..."
	$(RM) $(OBJS)

runClient:
	@echo "Starting to run $(NAME)... "; ./$(OUTPUTDIR)/$(NAME)

-include $(OBJS:.o=.d)
