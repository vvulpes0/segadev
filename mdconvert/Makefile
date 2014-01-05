PREFIX=/usr/local

all:
	for dir in */; do \
		pushd $${dir};  \
		make;           \
		popd;           \
	done

install:
	for dir in */; do                \
	  pushd $${dir};                 \
		make PREFIX=${PREFIX} install; \
		popd;                          \
	done
	install -m 755 mdconvert ${PREFIX}/bin

clean:
	for dir in */; do \
		pushd $${dir};  \
		make clean;     \
		popd;           \
	done

distclean:
	for dir in */; do \
		pushd $${dir};  \
		make distclean; \
		popd;           \
	done
