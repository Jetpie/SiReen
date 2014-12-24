# ENV
CC = gcc
LINK = g++

VLROOT = /home/bingqingqu/user-libs/vlfeat/vlfeat-0.9.19
VLLIB = /home/bingqingqu/user-lib/vlfeat/vlfeat-0.9.19/bin/glnxa64
BIN = bin/
SRC = src/
INCLUDE = include/
LIB = lib/
# CUSTOM DEFINITION
DEFS =

# FLAGS
CFLAGS = -g -Wall -DOS_LINUX -std=c++0x
#CFLAGS = -O2 -Wall -DOS_LINUX -std=c++0x
INCLUDES = -I$(LIB) -I$(LIB) -I$(INCLUDE) -I$(VLROOT)
LDFLAGS = -L$(BIN) -L$(LIB) -L$(VLLIB) -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lvl
#LDFLAGS =  -L$(VLLIB) -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lvl


# Target
PROGS1 = $(BIN)$/EDAautorun
#PROGS2 = $(BIN)STYLEautorun
#PROGS3 = $/KMEANS
#PROGS4 = $/NEWKMEANS
all: CREATE_DIR $(PROGS1)
#all: CREATE_DIR $(PROGS2)
#all: CREATE_DIR $(PROGS3)
#all: CREATE_DIR $(PROGS4)

# Directory
CREATE_DIR:
	mkdir -p $(BIN)

# Sources
SRCS_C1 = $(wildcard $(SRC)*.c)
SRCS_CPP1 = $(wildcard $(SRC)similarMain.cpp $(SRC)similarBasicFunction.cpp $(SRC)similarKeyFunction.cpp)
#SRCS_CPP2 = $(wildcard $(SRC)stylemain.cpp $(SRC)styleBasicFunction.cpp $(SRC)styleKeyFunction.cpp)
#SRCS_CPP3 = $(wildcard $(SRC)kmeansmain.cpp $(SRC)kmeans.cpp)
#SRCS_CPP4 = $(wildcard $(SRC)newkmeans.cpp)

# Objects
OBJS_C = $(patsubst $(SRC)%.c, $(BIN)%.o, $(SRCS_C))
OBJS_CPP1 = $(patsubst $(SRC)%.cpp, $(BIN)%.o, $(SRCS_CPP1))
#OBJS_CPP2 = $(patsubst $(SRC)%.cpp, $(BIN)%.o, $(SRCS_CPP2))
#OBJS_CPP3 = $(patsubst $(SRC)%.cpp, $(BIN)%.o, $(SRCS_CPP3))
#OBJS_CPP4 = $(patsubst $(SRC)%.cpp, $(BIN)%.o, $(SRCS_CPP4))

# Command
CMD_COMPILE = $(CC) -c $(CFLAGS) $< -o $@ $(INCLUDES) $(DEFS)
CMD_LINK = $(LINK) -o $@ $^ $(LDFLAGS)

# Make targets
$(PROGS1): $(OBJS_CPP1)
	$(CMD_LINK)
#$(PROGS2): $(OBJS_CPP2)
#	$(CMD_LINK)
#$(PROGS3): $(OBJS_CPP3)
#	$(CMD_LINK)
#$(PROGS4): $(OBJS_CPP4)
#	$(CMD_LINK)

# Make .o files
$(BIN)%.o: $(SRC)%.c
	$(CMD_COMPILE)

$(BIN)%.o: $(SRC)%.cpp
	$(CMD_COMPILE)

# clean
.PHONY: clean
clean:
	#$(RM) $(PROGS1) $(PROGS2) $(PROGS3) $(PROGS4) $(OBJS_CPP1) $(OBJS_CPP2) $(OBJS_CPP3) $(OBJS_CPP4)
	$(RM) $(PROGS1) $(OBJS_CPP1)

