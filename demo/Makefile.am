## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	@PACKAGE_CFLAGS@

DEMO_LIBS = $(top_builddir)/src/libgoocanvas-2.0.la @PACKAGE_LIBS@ $(INTLLIBS) -lm

#	-DG_DISABLE_DEPRECATED -DPANGO_DISABLE_DEPRECATED \
#	-DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED \
#	-DGTK_DISABLE_DEPRECATED

noinst_PROGRAMS = demo table-demo generic-position-demo simple-demo scalability-demo units-demo widgets-demo

demo_SOURCES = \
	demo.c demo-fifteen.c demo-groups.c demo-grabs.c \
	demo-arrowhead.c demo-features.c demo-events.c \
	demo-paths.c demo-focus.c demo-item.h demo-item.c demo-animation.c \
	demo-clipping.c demo-table.c demo-large-line.h demo-large-line.c \
	demo-large-items.c demo-large-rect.h demo-large-rect.c

demo_LDADD = $(DEMO_LIBS)

table_demo_SOURCES = \
	table-demo.c

table_demo_LDADD = $(DEMO_LIBS)

simple_demo_SOURCES = \
	simple-demo.c

simple_demo_LDADD = $(DEMO_LIBS)

scalability_demo_SOURCES = \
	scalability-demo.c

scalability_demo_LDADD = $(DEMO_LIBS)

units_demo_SOURCES = \
	units-demo.c

units_demo_LDADD = $(DEMO_LIBS)

widgets_demo_SOURCES = \
	widgets-demo.c

widgets_demo_LDADD = $(DEMO_LIBS)

generic_position_demo_SOURCES = \
	generic-position-demo.c

generic_position_demo_LDADD = $(DEMO_LIBS)

EXTRA_DIST = flower.png toroid.png

