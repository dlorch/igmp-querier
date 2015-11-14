AC_INIT([IGMPQuerier], [1.0], [https://github.com/dlorch/igmp-querier/issues],
        [igmp-querier], [https://github.com/dlorch/igmp-querier/]) 
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC

dnl Determine Synology CPU family
AC_MSG_CHECKING(name of Synology CPU family)
CC_MACHINE=`${CC} -dumpmachine`
case $CC_MACHINE in
    # ARM Kirkwood
    arm-none-linux-gnueabi)
        SYNO_CPU_FAMILY=armv5tejl
        SYNO_PKG_ARCH="88f6281 88f6282 88f628x 88fr131"
        ;;
    # ARM Armada
    arm-marvell-linux-gnueabi)
        SYNO_CPU_FAMILY=armv7l
        SYNO_PKG_ARCH="armada370 armadaxp" 
        ;;
    # PowerPC MPC85xx PowerCUICC III 
    powerpc-linux-gnuspe)    
        SYNO_CPU_FAMILY=mpc85xx
        SYNO_PKG_ARCH="ppc853x ppc854x"
        ;;
    # PowerPC QorIQ
    powerpc-none-linux-gnuspe)
        SYNO_CPU_FAMILY=qoriq 
        SYNO_PKG_ARCH="qoriq"
        ;;
    # Intel x86
    i686-linux-gnu|i686-pc-linux-gnu)
        SYNO_CPU_FAMILY=x86
        SYNO_PKG_ARCH="x86"
        ;;
    # Intel x64
    x86_64-linux-gnu|x86_64-pc-linux-gnu)
        SYNO_CPU_FAMILY=x64
        SYNO_PKG_ARCH="x86 cedarview evansport bromolow"
        ;;
    *)
        echo "    ERROR! Synology CPU family ${CC_MACHINE} not recognized. Please"
        echo "    file a bug report at https://github.com/dlorch/igmp-querier/issues"
        echo "    with the output of '${CC} -dumpmachine'"
        exit 1
        ;;
esac
AC_MSG_RESULT($SYNO_CPU_FAMILY)
AC_SUBST(SYNO_CPU_FAMILY)
AC_SUBST(SYNO_PKG_ARCH)

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
