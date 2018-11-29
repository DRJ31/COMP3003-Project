LIBDIRS = parson

.PHONY: all clean $(LIBDIRS)

all: $(LIBDIRS)
	$(AR) $(ARFLAGS) libs.a $(LIBDIRS)/$(LIBDIRS).o

clean:
	for dir in $(LIBDIRS); do \
		$(MAKE) clean -C $$dir; \
	done
	rm -f libs.a

parson:
	$(MAKE) -C $@
