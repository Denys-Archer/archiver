AC_INIT([archiver], [0.1], [denys@archlinux])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])

# Ініціалізація Libtool та архіватора
LT_INIT
AM_PROG_AR

# Перевірка C++ компілятора та примусове ввімкнення C++17
AC_PROG_CXX
CXXFLAGS="$CXXFLAGS -std=c++17"

# Перевірка наявності zlib
AC_CHECK_LIB([z], [compress], [], [
    AC_MSG_ERROR([zlib library is required but not found!])
])
AC_CHECK_HEADER([zlib.h], [], [
    AC_MSG_ERROR([zlib.h header is required but not found!])
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
