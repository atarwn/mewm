X11INC = /usr/include/X11
X11LIB = /usr/lib/X11

FREETYPEINC = /usr/include/freetype2

INCS = -I${X11INC} -I${FREETYPEINC}
LIBS = -L${X11LIB} -lX11 -lfontconfig -lXft

CFLAGS += -std=c99 -Wall -Wextra -pedantic -Wold-style-declaration
CFLAGS += -Wmissing-prototypes -Wno-unused-parameter ${INCS}
LDFLAGS += ${LIBS}
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin
CC     ?= cc

all: mewm

config.h:
	cp def.config.h config.h

mewm: mewm.c mewm.h config.h Makefile
	$(CC) -O3 $(CFLAGS) -o $@ $< -lX11 $(LDFLAGS)

install: all
	install -Dm755 mewm $(DESTDIR)$(BINDIR)/mewm

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/mewm

clean:
	rm -f mewm *.o

.PHONY: all install uninstall clean
