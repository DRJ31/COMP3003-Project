SRCDIR  = src
TESTDIR = tests

export NAME    = pb
export DESTDIR =

.PHONY: all clean tests $(SRCDIR)

all: $(NAME)

clean:
	for dir in $(SRCDIR) $(TESTDIR); do \
		$(MAKE) clean -C $$dir; \
	done
	rm -f $(NAME)

tests: $(TESTDIR)

$(NAME): $(SRCDIR)
	cp $(SRCDIR)/$(NAME) .

$(SRCDIR):
	$(MAKE) -C $@

$(TESTDIR):
	$(MAKE) -C $@
