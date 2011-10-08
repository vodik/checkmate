AC_INIT([checkmate], 0.1, [simongmzlj@gmail.com])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign])
AM_CONFIG_HEADER([config.h])

LT_INIT([disable-static])

AC_SUBST([CHECKMATE_SO_VERSION], [0:1:0])
AC_SUBST([CHECKMATE_API_VERSION], [1.0])

# Check for programs
AC_PROG_CC
AC_PROG_LIBTOOL

# Add some default build flags
CFLAGS="-Wall -Werror -std=gnu99"

# Check for libraries
# Check for header files
# Check for typedefs, structures, and compiler characteristics
# Check for library functions

AC_CONFIG_FILES([Makefile
                 checkmate/Makefile
                 tests/Makefile
                 checkmate-${CHECKMATE_API_VERSION}.pc:checkmate.pc.in])
AC_OUTPUT
