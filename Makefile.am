bin_PROGRAMS = bavovna-archiver
lib_LTLIBRARIES = libarchiver.la

AM_CXXFLAGS = -std=c++17 -I$(top_srcdir)/include -I$(top_srcdir)/types/cp -I$(top_srcdir)/types/mj -I$(top_srcdir)/types/ca -fPIC

libarchiver_la_SOURCES = \
    src/compress.cpp \
    src/keys.cpp \
    src/types.cpp \
    types/cp/cp.cpp \
    types/mj/mj.cpp \
    types/ca/ca.cpp

libarchiver_la_LIBADD = -lz

bavovna_archiver_SOURCES = \
    src/archiver.cpp

bavovna_archiver_LDADD = libarchiver.la -lz
