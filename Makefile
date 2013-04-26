
PREFIX ?= /usr/local

VERSION_MAJOR = 0
VERSION_MINOR = 1

LIBNAME = libini.so
SONAME = $(LIBNAME).$(VERSION_MAJOR)
LIBINI = $(SONAME).$(VERSION_MINOR)

CC = $(CROSS_COMPILE)gcc
ANALYZER = clang --analyze
INSTALL ?= install

CFLAGS += -Wall -O2 -Wextra -fPIC

OBJS = libini.o

.PHONY: all analyze clean install install-lib uninstall

all: $(LIBINI)

$(LIBINI): $(OBJS)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $@ $^ $(LDFLAGS)

analyze:
	$(ANALYZER) $(CFLAGS) $(OBJS:%.o=%.c)

install-lib: $(LIBINI)
	$(INSTALL) -D $(LIBINI) $(DESTDIR)$(PREFIX)/lib/$(LIBINI)
	ln -sf $(LIBINI) $(DESTDIR)$(PREFIX)/lib/$(SONAME)

install: install-lib
	$(INSTALL) -D -m 0644 ini.h $(DESTDIR)$(PREFIX)/include/ini.h
	ln -sf $(SONAME) $(DESTDIR)$(PREFIX)/lib/$(LIBNAME)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/include/ini.h \
		$(DESTDIR)$(PREFIX)/lib/$(LIBINI) \
		$(DESTDIR)$(PREFIX)/lib/$(SONAME) \
		$(DESTDIR)$(PREFIX)/lib/$(LIBNAME)

clean:
	rm -f $(OBJS) $(LIBINI)
