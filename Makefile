SRCDIR  = src
TESTDIR = tests

export NAME    = pb
export DESTDIR =

.PHONY: all clean tests

all: $(NAME)

clean:
	for dir in $(SRCDIR) $(TESTDIR); do \
		$(MAKE) clean -C $$dir; \
	done
	rm -f $(NAME)

tests:
	$(MAKE) -C $(TESTDIR)

$(NAME):
	$(MAKE) -C $(SRCDIR)
	cp $(SRCDIR)/$(NAME) .
