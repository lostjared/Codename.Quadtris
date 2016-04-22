#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT(Codename.Quadtris, 1.1, jaredbruni@gmail.com)

AC_CONFIG_HEADERS(config.h)

AM_INIT_AUTOMAKE
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG_CPLUSPLUS
# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

dnl KDE_FIND_PATH(programm-name, variable-name, list of directories,
dnl     if-not-found, test-parameter)
AC_DEFUN(KDE_FIND_PATH,
[
AC_MSG_CHECKING([for $1])
if test -n "$$2"; then
kde_cv_path="$$2";
else
kde_cache=`echo $1 | sed 'y%./+-%__p_%'`

AC_CACHE_VAL(kde_cv_path_$kde_cache,
[
kde_cv_path="NONE"
dirs="$3"
kde_save_IFS=$IFS
IFS=':'
for dir in $PATH; do
dirs="$dirs $dir"
done
IFS=$kde_save_IFS

for dir in $dirs; do
if test -x "$dir/$1"; then
if test -n "$5"
then
evalstr="$dir/$1 $5 2>&1 "
if eval $evalstr; then
kde_cv_path="$dir/$1"
break
fi
else
kde_cv_path="$dir/$1"
break
fi
fi
done

eval "kde_cv_path_$kde_cache=$kde_cv_path"

])

eval "kde_cv_path=\"`echo '$kde_cv_path_'$kde_cache`\""

fi

if test -z "$kde_cv_path" || test "$kde_cv_path" = NONE; then
AC_MSG_RESULT(not found)
$4
else
AC_MSG_RESULT($kde_cv_path)
$2=$kde_cv_path

fi
])

KDE_FIND_PATH(sdl-config, LIBSDL_CONFIG, [${prefix}/bin ${exec_prefix}/bin ], [
AC_MSG_ERROR([Could not find libsdl anywhere, check http://www.sdl.org])
])

if test -n "$LIBSDL_CONFIG"; then
LIBSDL_LIBS="`$LIBSDL_CONFIG --static-libs`"
LIBSDL_RPATH=
for args in $LIBSDL_LIBS; do
case $args in
-L*)
LIBSDL_RPATH="$LIBSDL_RPATH $args"
;;
esac
done
LIBSDL_RPATH=`echo $LIBSDL_RPATH | sed -e "s/-L/-R/g"`
LIBSDL_CFLAGS="`$LIBSDL_CONFIG --cflags`"

AC_DEFINE_UNQUOTED(HAVE_LIBSDL, 1, [Defines if your system has the LIBSDL library])
fi


LDPREFIX=${prefix}

if test  ${prefix} = "NONE"; then
LDPREFIX=/usr/local
fi


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h stddef.h stdint.h string.h strings.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_SUBST(LIBSDL_CFLAGS)
AC_SUBST(LIBSDL_LIBS)

echo "SDL CFLAGS found: $LIBSDL_CFLAGS"
echo "SDL LDFLAGS $LIBSDL_LIBS"


AC_OUTPUT(Makefile code/Makefile)
