PREFIX=/usr/local
CFLAGS=-std=c99 -Os -Wall
PROG=lightrack

all: $(PROG)

$(PROG): argz/argz.o

install: $(PROG)
	install -s $(PROG) $(PREFIX)/bin

clean:
	rm -f *.[ios] $(PROG)
