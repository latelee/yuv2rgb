#
# (C) Copyleft 2011 2012 2013
# Late Lee from http://www.latelee.org
# 
# A simple Makefile for *ONE* project(c or/and cpp file) in *ONE*  directory
#
# note: 
# you can put head file(s) in 'include' directory, so it looks 
# a little neat.
#
# usage: $ make
#        $ make debug=y
#
# log
#       2013-05-14 sth about debug...
###############################################################################

# !!!=== cross compile...
CROSS_COMPILE = 

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar

ARFLAGS = cr
RM = -rm -rf
MAKE = make

CFLAGS := -Wall

#****************************************************************************
# debug can be set to y to include debugging info, or n otherwise
debug          := y

#****************************************************************************

ifeq ($(debug), y)
CFLAGS += -ggdb -rdynamic
else
CFLAGS += -O2 -s
endif

# !!!===
DEFS = 

CFLAGS += $(DEFS)

LDFLAGS := $(LIBS)

# !!!===
INCDIRS := -I./

# !!!===
CFLAGS += $(INCDIRS)

# !!!===
LDFLAGS += 

# !!!===
# source file(s), including c file(s) cpp file(s)
# you can also use $(wildcard *.c), etc.
SRC_C   := $(wildcard *.c)
SRC_CPP := $(wildcard *.cpp)

# object file(s)
OBJ_C   := $(patsubst %.c,%.o,$(SRC_C))
OBJ_CPP := $(patsubst %.cpp,%.o,$(SRC_CPP))

# !!!===
# executable file
target = a.out

###############################################################################

all: $(target)

$(target): $(OBJ_C) $(OBJ_CPP)
	@echo "Generating executable file..." $(notdir $(target))
	@$(CXX) $(CFLAGS) $^ -o $(target) $(LDFLAGS)

# make all .c or .cpp
%.o: %.c
	@echo "Compiling: " $(addsuffix .c, $(basename $(notdir $@)))
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "Compiling: " $(addsuffix .cpp, $(basename $(notdir $@)))
	@$(CXX) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	@$(RM) $(target)
	@$(RM) *.o *.back *~

.PHONY: all clean
