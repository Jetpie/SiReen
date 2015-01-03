# ENV
CC = gcc
LINK = g++

VLROOT = /home/bingqingqu/user-libs/vlfeat/vlfeat-0.9.19
VLLIB = /home/bingqingqu/user-lib/vlfeat/vlfeat-0.9.19/bin/glnxa64
BIN = bin/
SRC = src/
LIB = lib/
TEMP = res/temp/
# CUSTOM DEFINITION
DEFS =

# FLAGS
CFLAGS = -g -Wall -DOS_LINUX -std=c++0x
#CFLAGS = -O2 -Wall -DOS_LINUX -std=c++0x
INCLUDES = -I$(VLROOT)
LDFLAGS =  -L$(VLLIB) -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib -lvl



# Target
PROG = $(BIN)/test

all: CREATE_DIR $(PROG)


# Directory
CREATE_DIR:
	mkdir -p $(BIN)

# Sources
SRC_CPP = $(wildcard $(SRC)testmain.cpp $(SRC)similarBasicFunction.cpp $(SRC)similarKeyFunction.cpp $(SRC)image_feature_extract.cpp)


# Objects
OBJ_CPP = $(patsubst $(SRC)%.cpp, $(TEMP)%.o, $(SRC_CPP))


# Command
CMD_COMPILE = $(CC) -c $(CFLAGS) $< -o $@ $(INCLUDES) $(DEFS)
CMD_LINK = $(LINK) -o $@ $^ $(LDFLAGS)

# Make targets
$(PROG): $(OBJ_CPP)
	$(CMD_LINK)


$(TEMP)%.o: $(SRC)%.cpp
	$(CMD_COMPILE)

# clean
.PHONY: clean
clean:
	$(RM) $(PROG) $(OBJ_CPP)
