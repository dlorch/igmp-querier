ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

sbin_PROGRAMS = igmp-querier

igmp_querier_SOURCES = igmp-querier.c

SYNO_PKG = $(PACKAGE_NAME)-$(SYNO_ARCH)-$(PACKAGE_VERSION).spk

package: $(bin_PROGRAMS)
	$(MKDIR_P) dist/
	$(MKDIR_P) package/sbin/
	cp $(sbin_PROGRAMS) package/sbin/
	$(AMTAR) zcf package.tgz package
	$(AMTAR) cf dist/$(SYNO_PKG) INFO package.tgz scripts
	rm -rf package.tgz package/
