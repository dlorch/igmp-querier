AC_INIT([IGMPQuerier], [1.0], [https://github.com/dlorch/igmp-querier/issues],
        [igmp-querier], [https://github.com/dlorch/igmp-querier/]) 
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC

dnl Determine Synology CPU architecture string
AC_MSG_CHECKING(name of Synology CPU architecture)
cc_machine=`${CC} -dumpmachine`
case $cc_machine in
    arm-marvell-linux-gnueabi)
        SYNO_ARCH=armada370
        AC_MSG_RESULT($SYNO_ARCH)
        ;;
    *)
        SYNO_ARCH=$cc_machine
        AC_MSG_RESULT($SYNO_ARCH)

        AC_MSG_WARN(Could not determine Synology CPU architecture from gcc,)
        AC_MSG_WARN(will use ${cc_machine} as the architecture name. Please file)
        AC_MSG_WARN(a bug report with the output of the following command:)
        AC_MSG_WARN(  ${CC} -dumpmachine)
        ;;
esac
AC_SUBST(SYNO_ARCH)

AC_ARG_WITH(libnet_includes,
    [  --with-libnet-includes=DIR    libnet include directory],
    [with_libnet_includes="$withval"],
    [with_libnet_includes=no])

AC_ARG_WITH(libnet_libraries,
    [  --with-libnet-libraries=DIR      libnet library directory],
    [with_libnet_libraries="$withval"],
    [with_libnet_libraries=no])

if test "$with_libnet_includes" != "no"; then
   CPPFLAGS="${CPPFLAGS} -I${with_libnet_includes}"
fi

if test "$with_libnet_libraries" != "no"; then
   LIBS="${LIBS} -L${with_libnet_libraries}"
fi

CPPFLAGS="${CPPFLAGS} `libnet-config --defines`"

if test `libnet-config --cflags | wc -c` = "1"; then
    CPPFLAGS="${CPPFLAGS} -I/usr/include -I/usr/local/include"
    LIBNET_CONFIG_BROKEN_CFLAGS=yes
else
    CPPFLAGS="${CPPFLAGS} `libnet-config --cflags`"
fi

if test `libnet-config --libs | wc -c` = "1"; then
    AC_MSG_WARN(libnet-config --libs is broken on your system.  If you)
    AC_MSG_WARN(are using a precompiled package please notify the maintainer.)
    LIBS="${LIBS} -L/usr/lib -L/usr/local/lib"
else
    LIBS="${LIBS} `libnet-config --libs | sed s/\-lnet//`"
fi

AC_DEFUN([HEADER_FAIL_MESSAGE],[
   echo
   echo
   echo "**********************************************"
   echo "  ERROR: unable to find" $1
   echo "  checked in the following places"
   for i in `echo $2`; do
       echo "        $i"
   done
   echo "**********************************************"
   echo
   exit 1
])

AC_DEFUN([FUNCTION_FAIL_MESSAGE],[ 
   echo 
   echo 
   echo "**********************************************" 
   echo "  ERROR: Cannot compile without" 
   echo "         the function $1" 
   echo "**********************************************" 
   echo 
   exit 1 
]) 

dnl Library tests
AC_CHECK_LIB(nsl, inet_aton)

LNET=""
AC_CHECK_LIB(net, libnet_build_ip,, LNET="no")

if test "$LNET" = "no"; then
    echo
    echo "   ERROR!  Libnet library not found, go get it from"
    echo "   http://packetfactory.openwall.net/projects/libnet/"
    echo "   or use the --with-libnet-* options, if you have it installed"
    echo "   in unusual place"
    exit
fi

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(stdio.h unistd.h sys/types.h arpa/inet.h)

AC_MSG_CHECKING(whether libnet-config defines endianness)
if eval "libnet-config --defines | grep '_ENDIAN' >/dev/null"; then
     AC_MSG_RESULT(yes)
else
     AC_MSG_RESULT(no)
     echo
     echo "    ERROR! libnet-config --defines does neither contain a definition for" 
     echo "    LIBNET_BIG_ENDIAN nor LIBNET_LIL_ENDIAN. Please fix libnet-config or"
     echo "    compile with CFLAGS = -DLIBNET_XXX_ENDIAN"
fi

LNET=""
AC_CHECK_HEADERS(libnet.h,, LNET="no")

if test "$LNET" = "no"; then
   echo
   echo "   ERROR!  Libnet header not found, go get it from"
   echo "   http://packetfactory.openwall.net/projects/libnet/"
   echo "   or use the --with-libnet-* options, if you have it installed"
   echo "   in unusual place"
   exit
fi

AC_MSG_CHECKING(for libnet version 1.0.2a)
if test "${LIBNET_CONFIG_BROKEN_CFLAGS}" = "yes"; then
    if test -n "$with_libnet_includes" -a "$with_libnet_includes" != "no"; then
        libnet_dir="${with_libnet_includes}"
    else
        libnet_dir="/usr/include /usr/local/include"
    fi
else
    libnet_dir=`libnet-config --cflags | cut -dI -f2`
fi

LIBNET_INC_DIR=""
for i in $libnet_dir; do
    if test -r $i/libnet.h; then
        LIBNET_INC_DIR=$i
    fi
done

if test "$LIBNET_INC_DIR" != ""; then
    if eval "grep LIBNET_VERSION $LIBNET_INC_DIR/libnet.h | grep -v 1.0.2a >/dev/null"; then
        AC_MSG_RESULT(no)
        echo
        echo "   ERROR!  libnet version 1.0.2a, required go get it from"
        echo "   http://packetfactory.openwall.net/projects/libnet/"
        HEADER_FAIL_MESSAGE("libnet 1.0.2a (libnet.h)", $LIBNET_INC_DIR)
    fi
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
    HEADER_FAIL_MESSAGE("libnet 1.0.2a (libnet.h)", $libnet_dir)
fi

dnl Checks for u_intx typedefs
AC_CHECK_TYPE(u_int8_t, uint8_t)
AC_CHECK_TYPE(u_int32_t, uint32_t)

dnl Checks for library functions.
AC_CHECK_FUNCS(inet_aton,,FUNCTION_FAIL_MESSAGE("inet_aton"))
AC_CHECK_FUNCS(memset,,FUNCTION_FAIL_MESSAGE("memset"))

AC_CONFIG_FILES([Makefile INFO])
AC_OUTPUT
