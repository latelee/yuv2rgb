# (C) Copyleft 2011 2012 2013 2014 2015 2016 2017 2018
# Late Lee(li@latelee.org) from http://www.latelee.org
# 
# A simple Makefile for *ONE* project(c or/and cpp file) in *ONE* or *MORE* directory
#
# note: 
# you can put head file(s) in 'include' directory, so it looks 
# a little neat.
#
# usage: 
#        $ make
#        $ make V=1     # verbose ouput
#        $ make CROSS_COMPILE=arm-arago-linux-gnueabi-  # cross compile for ARM, etc.
#        $ make debug=y # debug
#
# log
#       2013-05-14 sth about debug...
#       2016-02-29 sth for c/c++ multi diretory
#       2017-04-17 -s for .a/.so if no debug
#       2017-05-05 Add V for verbose ouput
###############################################################################

# !!!=== cross compile...
CROSS_COMPILE ?= 

MKDIR_P ?= mkdir -p

CC  = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR  = $(CROSS_COMPILE)ar

# !!!===
# in case all .c/.cpp need g++, specify CC=CXX...
# CC = $(CXX)

ARFLAGS = -cr
RM     = -rm -rf
MAKE   = make

# !!!===
# target executable file or .a or .so
target = a.out

# !!!===
# compile flags
CFLAGS += -Wall -Wfatal-errors -MMD

# !!!=== pkg-config here
#CFLAGS += $(shell pkg-config --cflags --libs glib-2.0 gattlib)
#LDFLAGS += $(shell pkg-config --cflags --libs glib-2.0 gattlib)

#****************************************************************************
# debug can be set to y to include debugging info, or n otherwise
debug  = y

#****************************************************************************

ifeq ($(debug), y)
    CFLAGS += -ggdb -rdynamic
else
    CFLAGS += -O2 -s
endif

# !!!===
# Macros define here
DEFS    += -DJIMKENT

# !!! compile flags define here...
CFLAGS  += $(DEFS)
# !!! c++ flags
CXXFLAGS = $(CFLAGS)
# !!! library here...
LIBS    += 
# !!! gcc/g++ link flags here
LDFLAGS += $(LIBS) -lpthread -lrt

# !!!===
# include head file directory here
INC = ./ ./inc
# or try this
#INC := $(shell find $(INC) -type d)

# !!!===
# build directory
BUILD_DIR ?= #./build/

# !!!===
# source file(s), including ALL c file(s) or cpp file(s)
# just need the directory.
SRC_DIRS = . 
# or try this
#SRC_DIRS = . ../outbox

# !!!===
# gcc/g++ compile flags
CFLAGS += $(INCDIRS)
CXXFLAGS += -std=c++11

# dynamic library build flags
DYNC_FLAGS += -fpic -shared

# include directory(s)
INCDIRS := $(addprefix -I, $(INC))

# source file(s)
SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name '*.cpp' -or -name '*.c')
# or try this
#SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c')

OBJS = $(patsubst %.c,$(BUILD_DIR)%.o, $(patsubst %.cpp,$(BUILD_DIR)%.o, $(SRCS))) 

# depend files(.d)
DEPS := $(OBJS:.o=.d)

ifeq ($(V),1)
Q=
NQ=true
else
Q=@
NQ=echo
endif

###############################################################################

all: $(BUILD_DIR)$(target)

$(BUILD_DIR)$(target): $(OBJS)

ifeq ($(suffix $(target)), .so)
	@$(NQ) "Generating dynamic lib file..." $(notdir $(target))
	$(Q)$(CXX) $(CXXFLAGS) $^ -o $(target) $(LDFLAGS) $(DYNC_FLAGS)
else ifeq ($(suffix $(target)), .a)
	@$(NQ) "Generating static lib file..." $(notdir $(target))
	$(Q)$(AR) $(ARFLAGS) -o $(target) $^
else
	@$(NQ) "Generating executable file..." $(notdir $(target))
	$(Q)$(CXX) $(CXXFLAGS) $^ -o $(target) $(LDFLAGS)
endif

# make all .c or .cpp
$(BUILD_DIR)%.o: %.c
	@$(MKDIR_P) $(dir $@)
	@$(NQ) "Compiling: " $(basename $(notdir $@)).c
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)%.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	@$(NQ) "Compiling: " $(basename $(notdir $@)).cpp
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(NQ) "Cleaning..."
	$(Q)$(RM) $(OBJS) $(target) $(DEPS)
# delete build directory if needed
ifneq ($(BUILD_DIR),)
	$(Q)$(RM) $(BUILD_DIR)
endif
# use 'grep -v soapC.o' to skip the file
	@find . -iname '*.o' -o -iname '*.bak' -o -iname '*.d' | xargs rm -f

.PHONY: all clean

-include $(DEPS)