CFLAGS  += $(shell pkg-config --cflags gtk+-3.0)

OBJS = main.o gui.o server.o

.PHONY: all clean

all: $(OBJS)
	$(AR) $(ARFLAGS) src.a $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f src.a

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
