PREFIX=/usr/local/
CFLAGS+=-D_POSIX_C_SOURCE=2
BINARY=mdchksum
SOURCES=mdchksum.c
PACKAGE=${BINARY}-${VERSION}
ARCHIVE=${PACKAGE}.tar
ZARCHIVE=${ARCHIVE}.gz
VERSION=1.0
MAKEABLE=${BINARY} LICENSE ${ARCHIVE} ${ZARCHIVE}

all: ${BINARY} LICENSE
archive: ${ZARCHIVE}
install: ${BINARY}
	install -m755 ${BINARY} ${PREFIX}/bin/

LICENSE: ${BINARY}
	./${BINARY} -l 2>$@

${BINARY}: ${SOURCES}
	${CC} ${CFLAGS} $< -o $@

${ZARCHIVE}: ${ARCHIVE}
	gzip -c $< >$@
	rm -f ${ARCHIVE}

${ARCHIVE}:
	make distclean
	rm -rf ${PACKAGE}
	mkdir ${PACKAGE}
	for file in *; do \
		[ "x$$file" != "x${PACKAGE}" ] && cp -ar $$file ${PACKAGE}/; \
	done
	tar cf $@ ${PACKAGE}
	rm -rf ${PACKAGE} 

clean:
	rm -f ${BINARY}

distclean:
	rm -f ${MAKEABLE}
