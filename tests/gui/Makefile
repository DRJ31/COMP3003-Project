CC = gcc
LD = $(CC)

GTK_PKGCONF_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_PKGCONF_LDFLAGS = $(shell pkg-config --libs gtk+-3.0)

CFLAGS = $(GTK_PKGCONF_CFLAGS) -std=c99 -Wall -Wextra -pedantic -Wno-unused-parameter
LDFLAGS = $(GTK_PKGCONF_LDFLAGS) -rdynamic

OBJS = gui.o

.PHONY: all clean

all: pb-gui-test

clean:
	rm -f $(OBJS)
	rm -f pb-gui-test

pb-gui-test: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
