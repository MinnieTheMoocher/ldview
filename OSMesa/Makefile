POSTFIX = -osmesa
CC = g++
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM

LDLIBDIRS = -L../../TCFoundation -L../../TRE -L../../LDLoader -L../../LDLib -L../../LDExporter
LIBDIRS = $(LDLIBDIRS) -L/usr/X11R6/lib
LIBS = -lLDraw$(POSTFIX) -lTRE$(POSTFIX) -lLDLoader$(POSTFIX) -lTCFoundation$(POSTFIX) -lLDExporter$(POSTFIX) -lgl2ps -lpng -lz -lOSMesa -lGLU -lGL -ljpeg -ltinyxml -L ../../lib
STATIC =
ARCH32 =
LDLIBS = ../TCFoundation/libTCFoundation$(POSTFIX).a ../LDLoader/libLDLoader$(POSTFIX).a ../TRE/libTRE$(POSTFIX).a ../LDLib/libLDraw$(POSTFIX).a ../LDExporter/libLDExporter$(POSTFIX).a
MAKEMODE = POSTFIX=-osmesa USE_BOOST=NO

CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -Wall -D_GNU_SOURCE -O3

CSRCS = $(wildcard *.c)
CCSRCS =  ldview.cpp

BASEDIR=$(realpath $(shell if test -d 3rdParty ; then pwd ; else if test -d ../3rdParty ; then echo .. ;fi;fi))

ifeq ($(shell uname -s), Darwin)
CFLAGS += -arch i386 -arch ppc
AR = libtool -static  -o
else
AR = ar rcs
endif

INCLUDE = -I.. -I../include

ifeq ($(shell uname -m), x86_64)
ifeq ($(LDVDEV32), YES)
CFLAGS += -m32
ARCH32 = -m32
LIBDIRS += -L/usr/lib32
LIBS += -l3ds
else
LIBS += -l3ds-64
endif
else
LIBS += -l3ds
endif

# slurm is media.peeron.com
ifeq ($(shell hostname), slurm)
INCLUDE += -I../../Mesa-7.0.2/include
LIBDIRS += -L../../../Mesa-7.0.2/lib
#STATIC = -static
endif

ifeq ($(shell hostname | cut -d. -f2-), pair.com)
# CFLAGS += -DNO_WSTRING -DNO_WCSTOUL -DTC_NO_UNICODE -DNO_WPRINTF
INCLUDE += -I../../Mesa-7.11/include -I/usr/local/include
LIBDIRS += -L../../../Mesa-7.11/lib -L/usr/local/lib/pth -L/usr/local/lib
LIBS += -lpth
STATIC = -static
endif

CFLAGS += -D_OSMESA -D_NO_BOOST

ifeq ($(shell if [ -d /usr/include/qt3 ]; then echo OK; fi),OK)
INCLUDE += -I/usr/include/qt3
endif

OBJDIR = .obj$(POSTFIX)

VPATH = $(OBJDIR)

SRCS = $(CSRCS) $(CCSRCS)
COBJS = $(CSRCS:.c=.o)
CCOBJS = $(CCSRCS:.cpp=.o)
OBJS = $(COBJS) $(CCOBJS)

.SUFFIXES:

.SUFFIXES:  .cpp .o .c

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(CFLAGSLOC) -c $<

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(CFLAGSLOC) -c $<

all:    $(OBJDIR) ldview

../TCFoundation/libTCFoundation$(POSTFIX).a:
	cd ../TCFoundation; $(MAKE) $(MAKEMODE)

../LDLoader/libLDLoader$(POSTFIX).a:
	cd ../LDLoader; $(MAKE) $(MAKEMODE)

../TRE/libTRE$(POSTFIX).a:
	cd ../TRE; $(MAKE) $(MAKEMODE)

../LDLib/libLDraw$(POSTFIX).a:
	cd ../LDLib; $(MAKE) $(MAKEMODE)

../LDExporter/libLDExporter$(POSTFIX).a:
	cd ../LDExporter; $(MAKE) $(MAKEMODE)

ifneq ($(shell if test -f /usr/include/tinyxml.h ; then echo OK ; fi),OK)
LIBS +=-L$(BASEDIR)/3rdParty/tinyxml
ldview:    $(BASEDIR)/3rdParty/tinyxml/libtinyxml.a
$(BASEDIR)/3rdParty/tinyxml/libtinyxml.a: $(BASEDIR)/3rdParty/tinyxml/*.cpp $(BASEDIR)/3rdParty/tinyxml/*.h
	cd $(BASEDIR)/3rdParty/tinyxml/ ; make -f Makefile.pbartfai
tinyxmlclean:
	cd ../3rdParty/tinyxml ; make -f Makefile.pbartfai clean
clean: tinyxmlclean
endif

ifneq ($(shell if test -f /usr/include/gl2ps.h ; then echo OK ; fi),OK)
LIBS +=-L$(BASEDIR)/gl2ps
ldview: $(BASEDIR)/gl2ps/libgl2ps.a
$(BASEDIR)/gl2ps/libgl2ps.a: $(BASEDIR)/gl2ps/*.c $(BASEDIR)/gl2ps/*.h
	cd $(BASEDIR)/gl2ps ; make

gl2psclean:
	cd ../gl2ps ; make clean
clean: gl2psclean
endif

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	$(RM) .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

Headerize: Headerize.o $(LDLIBS)
	cd $(OBJDIR); $(CC) $(STATIC) $(ARCH32) $(TESTING) -o ../Headerize Headerize.o $(LIBDIRS) -L../TCFoundation -lTCFoundation-osmesa -ljpeg -lpng -lz

LDViewMessages.h: Headerize ../LDViewMessages.ini
	./Headerize ../LDViewMessages.ini

StudLogo.h: Headerize ../Textures/StudLogo.png
	./Headerize ../Textures/StudLogo.png

ldview.o: StudLogo.h LDViewMessages.h

ldview: $(LDLIBS) $(OBJS)
	cd $(OBJDIR); $(CC) $(STATIC) $(ARCH32) $(TESTING) -o ../ldview $(OBJS) $(LIBDIRS) $(LIBS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

clean: MAKEMODE = clean POSTFIX=-osmesa
clean:
	cd ../TRE ;          $(MAKE) $(MAKEMODE)
	cd ../LDLib ;        $(MAKE) $(MAKEMODE)
	cd ../TCFoundation ; $(MAKE) $(MAKEMODE)
	cd ../LDLoader ;     $(MAKE) $(MAKEMODE)
	cd ../LDExporter ;   $(MAKE) $(MAKEMODE)
	if [ -d $(OBJDIR) ] ; then	\
		cd $(OBJDIR);			\
		$(RM) $(OBJS);			\
	fi
	$(RMDIR) $(OBJDIR)
	$(RM) ldview core Headerize StudLogo.h

debug: CFLAGSLOC = -g -DUNZIP_CMD
debug: MAKEMODE = debug POSTFIX=-osmesa
debug: all

test:
	if [ ! -f ~/.ldviewrc ] ; then      \
		cp ldviewrc.sample ~/.ldviewrc; \
	fi
	./ldview ../8464.mpd -SaveSnapshot=/tmp/8464.png \
	-SaveWidth=128 -SaveHeight=128 -ShowErrors=0 -SaveActualSize=0
	file /tmp/8464.png

