# makefile for bins

all: bin-all
clean: bin-clean
info: bin-info

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

#
VLROOT ?= /home/bingqingqu/user-libs/vlfeat/vlfeat-0.9.19
VLLIB := $(VLROOT)/bin/$(ARCH)
EIGENROOT ?= /home/bingqingqu/user-libs/eigen-3.2.4

BIN_CFLAGS := $(CFLAGS) -g -Wall -std=c++0x -O3 \
				-I$(VLROOT) -I$(EIGENROOT) -I$(SIREENROOT)/include
BIN_LDFLAGS := $(LDFLAGS) -L$(LIBDIR) -L$(VLLIB) -lopencv_core \
				-lopencv_imgproc -lopencv_highgui -lopencv_contrib -lvl

# Mac OS X Intel 32
ifeq ($(ARCH),maci)
endif

# Mac OS X Intel 64
ifeq ($(ARCH),maci64)
endif

# Linux-32
ifeq ($(ARCH),glnx86)
BIN_CFLAGS += -DOS_LINUX
endif

# Linux-64
ifeq ($(ARCH),glnxa64)
BIN_CFLAGS += -DOS_LINUX
endif

# set bin directory
BINDIR = $(SIREENROOT)/bin/$(ARCH)
OBJDIR = $(SIREENROOT)/obj/$(ARCH)
# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------
DEP_SRC:= $(wildcard $(SIREENROOT)/src/ic/*.cpp)
BIN_SRC:= $(wildcard $(SIREENROOT)/src/tests/*.cpp)
BIN_OBJ:= $(addprefix $(OBJDIR)/, $(patsubst %.cpp,%.o,$(notdir $(DEP_SRC) $(BIN_SRC))))
BIN_TGT:= $(addprefix $(BINDIR)/, $(patsubst %.cpp,%,$(notdir $(BIN_SRC))))
.PHONY: bin-all, bin-info, bin-clean
bin-all: $(BIN_TGT)

# BIN_LDFLAGS includes the libraries to link to and must be
# specified after the object "$<" that uses them. If not, stricter
# linkers (e.g. --as-needed option with the GNU toolchain)
# will break as they will not include the dependencies. See
# also http://wiki.debian.org/ToolChain/DSOLinking

$(BIN_TGT): $(BIN_OBJ)
	@mkdir -p $(BINDIR)
	@echo "	Linking..."
	$(CC) $^ -o $(BIN_TGT) $(BIN_LDFLAGS)

$(OBJDIR)/%.o: $(SIREENROOT)/src/tests/%.cpp
	@mkdir -p $(OBJDIR)
	$(call C,CC) $(BIN_CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(SIREENROOT)/src/ic/%.cpp
	@mkdir -p $(OBJDIR)
	$(call C,CC) $(BIN_CFLAGS) -c -o $@ $<

# only cleaning generated targets here
# cleaning bins under all architectures
bin-clean:
	@echo "	Cleaning bins..."
	$(RM) -r $(BIN_TGT)


bin-info:
	$(call echo-title, Exectuable Binary Compiling Parameters)
	$(call dump-var,BIN_SRC)
	$(call dump-var,BIN_OBJ)
	$(call echo-var,BIN_CFLAGS)
	$(call echo-var,BIN_LDFLAGS)
	@echo
