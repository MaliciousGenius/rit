#
# Created by gmakemake (Sparc Feb 24 2004) on Sat Apr 17 18:11:39 2004
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	$(SUFFIXES:.cc=.C .cpp)

.C:
		$(LINK.cc) -o $@ $< $(LDLIBS)
.cpp:
		$(LINK.cc) -o $@ $< $(LDLIBS)
.C.o:
		$(COMPILE.cc) $(OUTPUT_OPTION) $<
.cpp.o:
		$(COMPILE.cc) $(OUTPUT_OPTION) $<
.C.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.cpp.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%

CC =		gcc
CCC =		g++

########## Default flags (redefine these with a header.mak file if desired)
CCFLAGS =	-ggdb
CFLAGS =	-ggdb
LIBFLAGS =	-lm
########## End of default flags


CPP_FILES =	Block.cpp Display.cpp Figure.cpp TextString.cpp Triangle.cpp lab6-1.cpp
C_FILES =	
H_FILES =	Block.h Display.h Figure.h TextString.h Triangle.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES)
.precious:	$(SOURCEFILES)
OBJFILES =	Block.o Display.o Figure.o TextString.o Triangle.o

#
# Main targets
#

all:	lab6-1 

lab6-1:	lab6-1.o $(OBJFILES)
	$(CCC) $(CCFLAGS) -o lab6-1 lab6-1.o $(OBJFILES) $(LIBFLAGS)

#
# Dependencies
#

Block.o:	Block.h Display.h Figure.h
Display.o:	Display.h
Figure.o:	Display.h Figure.h
TextString.o:	Display.h Figure.h TextString.h
Triangle.o:	Display.h Figure.h Triangle.h
lab6-1.o:	Block.h Display.h Figure.h TextString.h Triangle.h

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm -r $(OBJFILES) lab6-1.o core 2> /dev/null

realclean:	clean
	/bin/rm -rf lab6-1 
