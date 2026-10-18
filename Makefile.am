bin_PROGRAMS = archiver
lib_LTLIBRARIES = libarchiver.la

AM_CXXFLAGS = -std=c++17 -I$(top_srcdir)/include -I$(top_srcdir)/types/cp -I$(top_srcdir)/types/mj -fPIC

libarchiver_la_SOURCES = \
    src/compress.cpp \
    src/keys.cpp \
    src/types.cpp \
    types/cp/cp.cpp \
    types/mj/mj.cpp

libarchiver_la_LIBADD = -lz

archiver_SOURCES = \
    src/archiver.cpp

archiver_LDADD = libarchiver.la -lz
