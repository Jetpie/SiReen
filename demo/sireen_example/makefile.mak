# makefile for demos

demo: demo-all
clean: demo-clean
info: demo-info

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

DEMO_CFLAGS := $(CFLAGS) -g -Wall -std=c++0x -O3 \
				-I$(VLROOT) -I$(EIGENROOT) -I$(SIREENROOT)/include
DEMO_LDFLAGS := $(LDFLAGS) -L$(LIBDIR) -L$(VLLIB) -lopencv_core \
				-lopencv_imgproc -lopencv_highgui -lopencv_contrib -lvl

# Mac OS X Intel 32
ifeq ($(ARCH),maci)
endif

# Mac OS X Intel 64
ifeq ($(ARCH),maci64)
endif

# Linux-32
ifeq ($(ARCH),glnx86)
DEMO_CFLAGS += -DOS_LINUX
endif

# Linux-64
ifeq ($(ARCH),glnxa64)
DEMO_CFLAGS += -DOS_LINUX
endif

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------
DEMO_SRC := $(wildcard $(SIREENROOT)/demo/sireen_example/src/*.cpp)
DEMO_OBJ:= $(addprefix $(OBJDIR)/, $(patsubst %.cpp,%.o,$(notdir $(DEP_SRC) $(DEMO_SRC))))
DEMO_TGT := $(addprefix $(BINDIR)/, $(patsubst %.cpp,%,$(notdir $(DEMO_SRC))))

.PHONY: demo-all, demo-info, demo-clean
demo-all: $(DEMO_TGT)

# BIN_LDFLAGS includes the libraries to link to and must be
# specified after the object "$<" that uses them. If not, stricter
# linkers (e.g. --as-needed option with the GNU toolchain)
# will break as they will not include the dependencies. See
# also http://wiki.debian.org/ToolChain/DSOLinking

$(BINDIR)/%: $(SIREENROOT)/demo/sireen_example/src/%.cpp $(DEP_OBJ) $(dirs)
	@echo "	Linking..."
	$(CC) $(DEMO_CFLAGS) $< $(DEP_OBJ) $(DEMO_LDFLAGS) -o $(DEMO_TGT)


demo-clean:
	@echo "	Cleaning demos..."
	$(RM) -r $(DEMO_TGT)


demo-info:
	$(call echo-title, DEMO Compilation Parameters)
	$(call dump-var,DEMO_SRC)
	$(call dump-var,DEMO_OBJ)
	$(call echo-var,DEMO_CFLAGS)
	$(call echo-var,DEMO_LDFLAGS)
	@echo
