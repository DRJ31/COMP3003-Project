export CC = gcc
export LD = $(CC)
export AR = ar

ifdef RELEASE
export CFLAGS   = -O2 -fstack-protector-all -flto -pipe -std=c99 -Wall -Wextra -pedantic
export CPPFLAGS = -DNDEBUG -D_FORTIFY_SOURCE=2
export LDFLAGS  = -rdynamic -fuse-linker-plugin -Wl,-O1,-z,defs,-z,relro,-z,now -flto
else
export CFLAGS   = -g -std=c99 -Wall -Wextra -pedantic
export CPPFLAGS =
export LDFLAGS  = -rdynamic
endif

export ARFLAGS = rcs

CFLAGS  += -I$(shell pwd)/libs/parson
LDFLAGS += $(shell pkg-config --libs gtk+-3.0 libmariadb)

SRCDIR  = src
LIBDIR  = libs
TESTDIR = tests

NAME = pb

.PHONY: all clean tests $(SRCDIR) $(LIBDIR)

all: $(NAME)

clean:
	for dir in $(SRCDIR) $(LIBDIR) $(TESTDIR); do \
		$(MAKE) clean -C $$dir; \
	done
	rm -f $(NAME)

tests: $(TESTDIR)

$(NAME): $(LIBDIR) $(SRCDIR)
	$(LD) $(LDFLAGS) -o $@ $(SRCDIR)/src.a $(LIBDIR)/libs.a

$(SRCDIR):
	$(MAKE) -C $@

$(LIBDIR):
	$(MAKE) -C $@

$(TESTDIR):
	$(MAKE) -C $@
