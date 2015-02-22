# Makefile
# Description: this is the makefile for SiReen project
# Author: Bingqing Qu
# Partial Modified From: VLFEAT MAKEFILE

# Accodring to the authors' COPYING, the following copying notice
# is put in advance
#
# Copyright (C) 2007-11, Andrea Vedaldi and Brian Fulkerson
# Copyright (C) 2012-13, The VLFeat Team
# All rights reserved.

# #USAGE#
# the makefile builds the typical executable cpp programs on Unix based
# installation system distributions with the GNU toolchain. Mac OS X
# and GNU-Linux are explictily supported. The simple usage command can
# be:
#
# > cd PATH_TO_SIREEN_ROOT
# > make


SHELL = /bin/bash
.PHONY : all
all:
# --------------------------------------------------------------------
#                                                       Error Messages
# --------------------------------------------------------------------

err_no_arch  =
err_no_arch +=$(shell echo "** Unknown host architecture '$(UNAME)'. This identifier"   1>&2)
err_no_arch +=$(shell echo "** was obtained by running 'uname -sm'. Edit the Makefile " 1>&2)
err_no_arch +=$(shell echo "** to add the appropriate configuration."                   1>&2)
err_no_arch +=config

err_internal  =$(shell echo Internal error)
err_internal +=internal

err_spaces  = $(shell echo "** SIREEN root dir SIREENROOT='$(SIREENROOT)' contains spaces."  1>&2)
err_spaces += $(shell echo "** This is not supported due to GNU Make limitations." 1>&2)
err_spaces +=spaces


# --------------------------------------------------------------------
#                                             Auto-detect architecture
# --------------------------------------------------------------------

Darwin_PPC_ARCH := mac
Darwin_Power_Macintosh_ARCH := mac
Darwin_i386_ARCH := maci64
Darwin_x86_64_ARCH := maci64
Linux_i386_ARCH := glnx86
Linux_i686_ARCH := glnx86
Linux_unknown_ARCH := glnx86
Linux_x86_64_ARCH := glnxa64

UNAME := $(shell uname -sm)
ARCH ?= $($(shell echo "$(UNAME)" | tr \  _)_ARCH)

# sanity check
ifeq ($(ARCH),)
die:=$(error $(err_no_arch))
endif

ifneq ($(SIREENROOT),$(shell echo "$(SIREENROOT)" | sed 's/ //g'))
die:=$(error $(err_spaces))
endif
# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

SIREENROOT ?= .
CC = g++
# TODO: determine CFLAGS by ARCH

# Architecture specific ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Detect compiler
COMPILER_VER_STRING:=$(shell $(CC) --version) $(shell $(CC) -v 2>&1)
COMPILER:=other

ifneq ($(shell echo "$(COMPILER_VER_STRING)" | grep "gcc"),)
COMPILER:=gcc
COMPILER_VER:=$(shell \
$(CC) -dumpversion | \
sed -e 's/\.\([0-9][0-9]\)/\1/g' \
    -e 's/\.\([0-9]\)/0\1/g' \
    -e 's/^[0-9]\{3,4\}$$/&00/' )
endif

ifeq "$(findstring clang,$(COMPILER_VER_STRING))" "clang"
COMPILER:=clang
COMPILER_VER:=$(shell \
echo "$(COMPILER_VER_STRING)" | \
sed -n -e 's/.*version *\([0-9.][0-9.]*\).*/\1/p;' | \
sed -e 's/\.\([0-9][0-9]\)/\1/g' \
    -e 's/\.\([0-9]\)/0\1/g' \
    -e 's/^[0-9]\{3,4\}$$/&00/' )
endif

$(info Detected compiler: $(COMPILER) $(COMPILER_VER))
ifeq "$(COMPILER_VER)" "other"
$(warning Unsupported compiler detected, use at your own risk!)
endif

ifeq "$(COMPILER)" "clang"
$(info Clang does not support OpenMP yet, disabling.)
endif

# Mac OS X Intel
ifeq "$(ARCH)" "$(filter $(ARCH),maci maci64)"
ifeq "$(ARCH)" "maci"
march=32
else
march=64
endif
endif

# Linux
ifeq "$(ARCH)" "$(filter $(ARCH),glnx86 glnxa64)"
ifeq "$(ARCH)" "glnx86"
march=32
else
march=64
endif
endif
# --------------------------------------------------------------------
#                                                            Functions
# --------------------------------------------------------------------

# $(call if-like,FILTER,WHY,WHAT)
define if-like
$(if $(filter $(1),$(2)),$(3))
endef

# $(call dump-var,VAR) pretty-prints the content of a variable VAR on
# multiple columns
ifdef VERB
define dump-var
@echo "$(1) ="
@echo "$($(1))" | sed 's/\([^ ][^ ]* [^ ][^ ]*\) */\1#/g' | \
tr '#' '\n' | column -t | sed 's/\(.*\)/  \1/g'
endef
else
define dump-var
@printf "%15s = %s\n" "$(1)" \
"$$(echo '$($(1))' | sed -e 's/[^ ][^ ]* /\.\.\./3' -e 's/\.\.\..*$$/\.\.\./g')"
endef
endif

# $(call echo-var,VAR) pretty-prints the content of a variable VAR on
# one line
define echo-var
@printf "%15s = %s\n" '$(1)' '$($(1))'
endef

# $(call echo-title,TITLE) pretty-prints TITLE as a title
define echo-title
@printf "** %s\n" '$(1)'
endef

# $(call C, CMD) runs $(CMD) silently
define C
@printf "%10s %s\n" '$(1)' '$(@)'
$(Q)"$($(1))"
endef

# If verbose print everything
ifdef VERB
Q=
else
Q=@
endif
# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------
# Each Makefile submodule appends appropriate dependencies to the all,
# clean, and info targets. In addition, it
# appends to the deps and bins variables the list of .d files (to be
# inclued by make as auto-dependencies) and the list of files to be
# added to the binary distribution.

include src/makefile.mak
include demo/sireen_example/makefile.mak
.PHONY: clean, info, help,demo
demo:

# cleaning obj directory
# put all objects into obj before that
clean:
	@echo "	Cleaning all..."
	rm -f  `find . -name '*~'`
	rm -f  `find . -name '.DS_Store'`
	rm -f  `find . -name '.gdb_history'`
	rm -f  `find . -name '._*'`
	$(RM) -r $(SIREENROOT)/obj

info:
	$(call echo-title,General settings)
	$(call echo-var,ARCH)
	$(call echo-var,CC)
	$(call echo-var,COMPILER)
	$(call echo-var,COMPILER_VER)
	$(call echo-var,CFLAGS)
	$(call echo-var,LDFLAGS)
	@printf "\nThere are %s lines of code.\n" \
	`cat $(BIN_SRC) $(DEP_SRC) $(DEMO_SRC) | wc -l`
help:
	@cat Makefile | \
	sed -n '1,/^$$/!p' | \
	sed -n '1,/^$$/!p' | \
	sed -n '1,/^$$/p' | \
	sed 's/^# \{0,1\}\(.*\)$$/\1/'
