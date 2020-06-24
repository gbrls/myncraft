CLIENTDIR := src

# binary output directory name
OUTPUTDIR := bin

# name for the output(binary) of the client code
CLIENTNAME := game

# libary code directory name
LIBDIR := src/lib

# add C flags, like: CFLAGS := -Wall -Werror -Wconversion -Wextra
CFLAGS :=

# add load flags, others like -pthread
LDLIB := -lm -lSDL2 -lGL -lGLEW

CC := g++ -std=c++17
RM := rm -f
MK := mkdir -p

EXT := cpp

CLIENTSOURCES := $(shell find $(CLIENTDIR) -type f -name *.$(EXT))

CLIENTOBJS := $(subst .$(EXT),.o,$(CLIENTSOURCES))

all: mkdirs buildClient clean runClient

debug: mkdirs buildClientDebug clean runClient

game: mkdirs buildClient clean runClient

run: runClient

buildClient: $(LIBOBJS) $(CLIENTOBJS)
	@echo "Linking $(CLIENTNAME)..."
	$(CC) -o $(OUTPUTDIR)/$(CLIENTNAME) $(LIBOBJS) $(CLIENTOBJS) $(LDLIB) $(CFLAGS)

buildClientDebug: $(LIBOBJS) $(CLIENTOBJS)
	@echo "Linking (DEBUG) $(CLIENTNAME)..."
	$(CC) -o $(OUTPUTDIR)/$(CLIENTNAME) $(LIBOBJS) $(CLIENTOBJS) $(LDLIB) $(CFLAGS) -fsanitize=address

%.o : %.$(EXT)
	$(CC) -c $< -o $@ $(CFLAGS)

mkdirs:
	$(MK) $(OUTPUTDIR)

clean:
	@echo "Cleaning..."
	$(RM) $(LIBOBJS) $(CLIENTOBJS)

runClient:
	@echo "Starting to run $(CLIENTNAME)... "; ./$(OUTPUTDIR)/$(CLIENTNAME)
