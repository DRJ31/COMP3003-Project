CFLAGS += $(shell pkg-config --cflags gtk+-3.0 mariadb glib-2.0)

OBJS = main.o data.o json.o http.o gui.o server.o client.o

.PHONY: all clean

all: $(OBJS)
	$(AR) $(ARFLAGS) src.a $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f src.a

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
