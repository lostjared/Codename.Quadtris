AM_CXXFLAGS = -std=c++11 -O2 $(LIBSDL_CFLAGS)  -I$(top_srcdir)/include -I$(prefix)/include 
AM_LDFLAGS = #$(LIBSDL_LIBS)  -lpng -lz -ljpeg
bin_PROGRAMS=Quadtris
Quadtris_SOURCES=font.mxf.c main.cc quadtris.cc debug.cc event.cc font.cc  joystick.cc parser.cc system.cc thread.cc types.cc video.cc
Quadtris_LDADD=$(LIBSDL_LIBS) -lpng -lz -ljpeg
