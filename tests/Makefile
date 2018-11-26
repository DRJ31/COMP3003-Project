TESTDIRS = gui

.PHONY: all clean

all:
	for dir in $(TESTDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	for dir in $(TESTDIRS); do \
		$(MAKE) clean -C $$dir; \
	done
