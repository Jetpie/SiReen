# ENV
CC = gcc
LINK = g++

VLROOT = /home/bingqingqu/user-libs/vlfeat/vlfeat-0.9.19
VLLIB = /home/bingqingqu/user-lib/vlfeat/vlfeat-0.9.19/bin/glnxa64
BIN = bin/
SRC = src/
INCLUDE = include/
LIB = lib/
TEMP = res/temp/
# CUSTOM DEFINITION
DEFS =

# FLAGS
CFLAGS = -g -Wall -DOS_LINUX -std=c++0x
#CFLAGS = -O2 -Wall -DOS_LINUX -std=c++0x
INCLUDES = -I$(LIB) -I$(LIB) -I$(INCLUDE) -I$(VLROOT)
LDFLAGS = -L$(BIN) -L$(LIB) -L$(VLLIB) -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lvl
#LDFLAGS =  -L$(VLLIB) -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lvl


# Target
PROGS1 = $(BIN)$/LLCcoder

all: CREATE_DIR $(PROGS1)


# Directory
CREATE_DIR:
	mkdir -p $(BIN)

# Sources
SRCS_C1 = $(wildcard $(SRC)*.c)
SRCS_CPP1 = $(wildcard $(SRC)similarMain.cpp $(SRC)similarBasicFunction.cpp $(SRC)similarKeyFunction.cpp)


# Objects
OBJS_C = $(patsubst $(SRC)%.c, $(TEMP)%.o, $(SRCS_C))
OBJS_CPP1 = $(patsubst $(SRC)%.cpp, $(TEMP)%.o, $(SRCS_CPP1))


# Command
CMD_COMPILE = $(CC) -c $(CFLAGS) $< -o $@ $(INCLUDES) $(DEFS)
CMD_LINK = $(LINK) -o $@ $^ $(LDFLAGS)

# Make targets
$(PROGS1): $(OBJS_CPP1)
	$(CMD_LINK)

# Make .o files
$(TEMP)%.o: $(SRC)%.c
	$(CMD_COMPILE)

$(TEMP)%.o: $(SRC)%.cpp
	$(CMD_COMPILE)

# clean
.PHONY: clean
clean:
	$(RM) $(PROGS1) $(OBJS_CPP1)

