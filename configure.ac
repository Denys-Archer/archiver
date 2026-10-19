# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([bavovna-archiver], [1.5.6], [denys@archlinux])
AC_CONFIG_SRCDIR([src/archiver.cpp])
AC_CONFIG_AUX_DIR([build-aux])

# Ініціалізація Automake (попередження про errors переведені в режим стандартної сумісності)
AM_INIT_AUTOMAKE([foreign -Wall -Werror  subdir-objects])

# Перевірка компіляторів C та C++
AC_PROG_CC
AC_PROG_CXX

# Важливо: AM_PROG_AR має йти ДО LT_INIT, щоб уникнути помилок Autotools
AM_PROG_AR

# Ініціалізація Libtool для збірки спільних бібліотек (.la)
LT_INIT

# Перевірка наявності бібліотеки zlib (необхідна для -lz)
AC_CHECK_LIB([z], [deflate], [], [
    AC_MSG_ERROR([zlib library not found! Please install zlib development packages.])
])

# Генерація Makefile-ів для проєкту
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
