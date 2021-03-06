#====================================================================
#
#Generic Makefile for c/c++ Program
#
#License:	GPL (General Public License)
#Author	: 	zhuchengzhi <zhuczloveai910@gmail.com>
#Data	:	2015/12/01(version 0.1)
#
#====================================================================

##Customizable Section:adapt those variables to suit your Program
##===================================================================


#The extra pre-processor and compiler options
EXTRA_CFLAGS =

#The extra linker options
EXTRA_LDFLAGS =

#Specify the incude dirs
INCLUDE = -I./inc 

#The pre-processor options used by the cpp (man cpp for more).
CPPFLAGS = -Wall $(INCLUDE)

#The options used in linking as well as in any direct use of ld.
LDFLAGS =

# The directories in which source files reside.
# If not specified,all subdirectories of the current directory will be serached.
SRCDIRS	:=

#The executable file name.Must be specified.
PROGRAM	= occ_apps

## Implicit Section:change the following only when necessary.
##==========================================================

# The source file types (headers excluded).
# .c indicates C source files,and others C++ ones.
SRCEXTS	= .c .C .cc .cpp .CPP .c++ .cxx .cp 

#The header file types.
HDREXTS	= .h .H .hh .hpp .HPP .h++ .hxx .hp 

#The pre-processor and compiler options.
#Users can override those variables from the commnad line.
CFLAGS	 = -g -O2
CXXFLAGS = -g -O2 

# The C Program compiler.
CC	=/usr/local/arm/4.3.2/bin/arm-linux-gcc

# The C++ Program compiler.
CXX	=/usr/local/arm/4.3.2/bin/arm-linux-g++

#Un-comment the following line to compile C programs as C++ ones.
#CC	= $(CXX)

#The commnad used to delete file .
RM = rm -f

ETAGS = etags 
ETAGSFLAGS =

CTAGS = ctags
CTAGSFLAGS = 

## Stable Section: usually no need to be changed. But you can add more.
##=====================================================================
ifeq ($(SRCDIRS),)
	SRCDIRS := $(shell find $(SRCDIRS) -type d)
endif

SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
HEADERS = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(HDREXTS))))
SRC_CXX = $(filter-out %.c,$(SOURCES))
OBJS    = $(addsuffix .o,$(basename $(SOURCES)))
#DEPS	= $(OBJS:%.o=%.d) #replace %.d while .%.d (hide dependency files)
DEPS	= $(foreach f,$(OBJS),$(addprefix $(dir $(f))., $(patsubst %.o, %.d, $(notdir $(f)))))

##Define some useful variables.
DEP_OPT	= $(shell if `$(CC) --version | grep -i "GCC" >/dev/null`;then \
		  			echo "-MM";else echo "-M"; fi)
DEPEND.d	= $(CC) $(DEP_OPT) $(EXTRA_CFLAGS) $(CFLAGS) $(CPPFLAGS) -lpthread
COMPILE.c	= $(CC) $(EXTRA_CFLAGS) $(CFLAGS) $(CPPFLAGS) -c	-lpthread
COMPILE.cxx = $(CXX) $(EXTRA_CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c	-lpthread
LINK.c		= $(CC) $(EXTRA_CFLAGS) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -lpthread
LINK.cxx	= $(CXX) $(EXTRA_CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -lpthread

.PHONY:all objs tags ctags clean distclean help show

#Delete the default suffixes
.SUFFIXES:

all: $(PROGRAM)

#Rules for creating dependency files (.d).

.%.d:%.c
	@echo -n $(dir $<) >$@
	@$(DEPEND.d) $< >> $@

.%.d:%.C
	@echo -n $(dir $<) >$@
	@$(DEPEND.d) $< >> $@

.%.d:%.cc 
	@echo -n $(dir $<) >$@ 
	@$(DEPEND.d) $< >> $@

.%.d:%.cpp 
	@echo -n $(dir $<) >$@
	@$(DEPEND.d) $< >> $@

.%.d:%.CPP 
	@echo -n $(dir $<) >$@
	@$(DEPEND.d) $< >> $@

.%.d:%.c++
	@echo -n $(dir $<) >$@
	@$(DEPEND.d) $< >> $@ 

.%.d:%.cp 
	@echo -n $(dir $<) >$@
	@$(DEPEND.d) $< >> $@ 

.%.d:%.cxx 
	@echo -n $(dir $<) >$@
	@$(DEPEND.d) $< >> $@ 

#Rules for generating object file (.o).
#------------------------------------------
objs:$(OBJS)

%.o:%.c
	$(COMPILE.c) $< -o $@

%.o:%.C
	$(COMPILE.cxx) $< -o $@

%.o:%.cc 
	$(COMPILE.cxx) $< -o $@

%.o:%.cpp 
	$(COMPILE.cxx) $< -o $@

%.o:%.CPP 
	$(COMPILE.cxx) $< -o $@

%.o:%.c++
	$(COMPILE.cxx) $< -o $@

%.o:%.cp 
	$(COMPILE.cxx) $< -o $@

%.o:%.cxx 
	$(COMPILE.cxx) $< -o $@

#Rules for generating the tags .
#-----------------------------------------
tags: $(HEADERS) $(SOURCES)
	$(ETAGS) $(ETAGSFLAGS) $(HEADERS) $(SOURCES)

ctags: $(HEADERS) $(SOURCES)
	$(CTAGS) $(CTAGSFLAGS) $(HEADERS) $(SOURCES)

#Rules for generating the executable.
#------------------------------------------------
$(PROGRAM) :$(OBJS)
ifeq ($(SRC_CXX),)	#C program 
	$(LINK.c) $(OBJS) $(EXTRA_LDFLAGS) -o $@
	@echo Type ./$@ to execute the Program.
else
	$(LINK.cxx) $(OBJS) $(EXTRA_LDFLAGS) -o $@
	@echo Type ./$@ to execute the Program.
endif

# TODO Modfile by zhuchengzhi ifndef NODEP 
ifdef NODEP
ifneq ($(DEPS),)
	sinclude $(DEPS)
endif
endif


clean:
	$(RM) $(OBJS) $(PROGRAM) $(PROGRAM).exe

distclean:clean
	$(RM) $(DEPS) TAGS

#Show help.
help:
	@echo 'Generic Makefile for C/C++ programs (gcmakefile) version 0.5'
	@echo 'Copyright (C) 2015,2016 zhuchengzhi <zhuczloveai910@gmail.com>'
	@echo
	@echo 'Users:make [TARGET]'
	@echo 'TARGETS:'
	@echo 'all (=make ) compile and link.'
	@echo 'NODEP=yes make without generating dependencies.'
	@echo 'objs	compile only (no linking).'
	@echo 'tags	create tags for Emacs editor'
	@echo 'ctags create ctags for VI editor.'
	@echo 'clean clean objects and the executable file.'
	@echo 'distclean clean objects,the executable and dependencies.'
	@echo 'show variables (for debug use only).'
	@echo 'help print this message/'
	@echo
	@echo 'Report bugs to <zhuczloveai910@gmail.com>'

#Show variables (for debug use only.)
show:
	@echo 'PROGRAM		:' $(PROGRAM)
	@echo 'SRCDIRS		:' $(SRCDIRS)
	@echo 'HEADERS		:' $(HEADERS)
	@echo 'SOURCES		:' $(SOURCES)
	@echo 'SRC_CXX		:' $(SRC_CXX)
	@echo 'OBJS			:' $(OBJS)
	@echo 'DEPS			:' $(DEPS)
	@echo 'DEPEND		:' $(DEPEND)
	@echo 'DEPEND.d		:' $(DEPEND.d)
	@echo 'COMPILE.c	:' $(COMPILE.c)
	@echo 'COMPILE.cxx 	:' $(COMPILE.cxx)
	@echo 'link.c 		:' $(LINK.c)
	@echo 'link.cxx		:' $(LINK.cxx)

## End of the Makefile ## Suggestions are welcome ##All rights reserved ##
##########################################################################

