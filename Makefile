# ENV
CC = gcc
LINK = g++

VLROOT = /home/bingqingqu/user-libs/vlfeat/vlfeat-0.9.19
VLLIB = /home/bingqingqu/user-lib/vlfeat/vlfeat-0.9.19/bin/glnxa64
EIGENROOT = /home/bingqingqu/user-libs/eigen-3.2.4
BIN = bin/
SRC = src/
LIB = lib/
TEMP = res/temp/
# CUSTOM DEFINITION
DEFS =

# FLAGS
CPPFLAGS = -g -Wall -DOS_LINUX -std=c++0x -O3
INCLUDES = -I$(VLROOT) -I$(EIGENROOT)
LDFLAGS = -L$(BIN) -L$(LIB) -L$(VLLIB) -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lvl



# Target
PROGS = $(BIN)$/LLCcoder

all: CREATE_DIR $(PROGS)


# Directory
CREATE_DIR:
	mkdir -p $(BIN)

# Sources
# SRCS_C1 = $(wildcard $(SRC)*.c)
SRCS_CPP = $(wildcard $(SRC)main.cpp $(SRC)file_utility.cpp $(SRC)image_feature_extract.cpp)


# Objects
# OBJS_C = $(patsubst $(SRC)%.c, $(TEMP)%.o, $(SRCS_C))
OBJS_CPP = $(patsubst $(SRC)%.cpp, $(TEMP)%.o, $(SRCS_CPP))


# Command
CMD_COMPILE = $(CC) -c $(CPPFLAGS) $< -o $@ $(INCLUDES) $(DEFS)
CMD_LINK = $(LINK) -o $@ $^ $(LDFLAGS)

# Make targets
$(PROGS): $(OBJS_CPP)
	$(CMD_LINK)

# Make .o files
# $(TEMP)%.o: $(SRC)%.c
# 	$(CMD_COMPILE)

$(TEMP)%.o: $(SRC)%.cpp
	$(CMD_COMPILE)

# clean
.PHONY: clean
clean:
	$(RM) $(PROGS) $(OBJS_CPP)
