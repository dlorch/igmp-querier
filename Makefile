VERSION=1.0
ARCH=armada370
EXEC=igmp-querier
SRC=igmp-querier.c
PKG=IGMPQuerier-$(ARCH)-$(VERSION).spk

# Marvell armada 370
CC=/usr/local/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-gcc
LD=/usr/local/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-ld
CFLAGS +=-I/usr/local/arm-marvell-linux-gnueabi/arm-marvell-linux-gnueabi/libc/include -I/usr/local/arm-marvell-linux-gnueabi/include -mhard-float -mfpu=vfpv3-d16 -DLIBNET_LIL_ENDIAN=1 -Wall
LDFLAGS+=-L/usr/local/arm-marvell-linux-gnueabi/arm-marvell-linux-gnueabi/libc/lib -L/usr/local/arm-marvell-linux-gnueabi/lib
LDLIBS+=-lnet

all: $(EXEC)
	mkdir -p package/sbin/
	cp $(EXEC) package/sbin/
	tar zcf package.tgz package
	tar cf $(PKG) INFO package.tgz scripts

clean:
	/bin/rm -rf package/ package.tgz $(PKG) $(EXEC)
