CC = gcc
LD = gcc

ifdef RELEASE
CFLAGS   = -O2 -fstack-protector-all -flto -pipe -std=c99 -Wall -Wextra -pedantic
CPPFLAGS = -DNDEBUG -D_FORTIFY_SOURCE=2
LDFLAGS  = -rdynamic -fuse-linker-plugin -Wl,-O1,-z,defs,-z,relro,-z,now -flto
else
CFLAGS   = -g -std=c99 -Wall -Wextra -pedantic
CPPFLAGS =
LDFLAGS  = -rdynamic
endif

CFLAGS  += $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS += $(shell pkg-config --libs gtk+-3.0)

OBJS = main.o gui.o

.PHONY: all clean

all: $(NAME)

clean:
	rm -f $(OBJS)
	rm -f $(NAME)

$(NAME): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
