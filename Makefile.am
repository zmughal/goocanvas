## Process this file with automake to produce Makefile.in

SUBDIRS = src demo docs po bindings

# require automake 1.7
AUTOMAKE_OPTIONS = 1.7

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = goocanvas-2.0.pc

EXTRA_DIST = goocanvas-2.0.pc.in autogen.sh
