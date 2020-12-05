.POSIX:

include config.mk

OBJECTS = main.o

all: options ccg

options:
	@echo ccg build options:
	@echo "CFLAGS  = $(MYCFLAGS)"
	@echo "LDFLAGS = $(MYLDFLAGS)"
	@echo "CC      = $(CC)"

.c.o:
	$(CC) $(MYCFLAGS) -c $<

ccg: $(OBJECTS)
	$(CC) $(OBJECTS) -o ccg $(MYLDFLAGS)

# Remove binary and object files
clean:
	rm -f ccg $(OBJECTS)

# Install program
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f ccg $(DESTDIR)$(PREFIX)/bin
	chmod +x $(DESTDIR)$(PREFIX)/bin/ccg
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/ccg

.PHONY: all options clean install uninstall