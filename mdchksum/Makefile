PREFIX=/usr/local
CFLAGS=-Os
LD=$(CC)

.SUFFIXES : 
.SUFFIXES : .c .o

.c.o : ; $(CC) $(CFLAGS) -o $(.TARGET) -c $(.IMPSRC)

.MAIN : mdchksum

mdchksum : mdchksum.o
	$(LD) $(LDFLAGS) $(.ALLSRC:M*.o:O) $(LIBS) -o $(.TARGET)

install : mdchksum mdchksum.1
	install -d "$(PREFIX)/bin"
	install -d "$(PREFIX)/share/man/man1"
	install -m0755 "$(.OBJDIR)/mdchksum" "$(PREFIX)/bin/"
	install -m0644 "mdchksum.1" "$(PREFIX)/share/man/man1/"
