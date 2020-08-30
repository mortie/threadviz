DESTDIR ?=
PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib
BINDIR ?= $(PREFIX)/bin
DATAROOTDIR ?= $(PREFIX)/share

libthreadviz.so: libthreadviz.c
	$(CC) -shared -fPIC -o $@ $^ -lpthread -ldl

.PHONY: install
install: libthreadviz.so
	install -d $(DESTDIR)$(LIBDIR) 
	install libthreadviz.so $(DESTDIR)$(LIBDIR)/libthreadviz.so
	install -d $(DESTDIR)$(BINDIR)
	install threadviz.sh $(DESTDIR)$(BINDIR)/threadviz
	install -d $(DESTDIR)$(DATAROOTDIR)/threadviz
	install visualize.html $(DESTDIR)$(DATAROOTDIR)/threadviz/visualize.html
	sed -i 's#@LIBDIR@#$(LIBDIR)#; s#@DATAROOTDIR@#$(DATAROOTDIR)#' \
		$(DESTDIR)$(BINDIR)/threadviz

.PHONY: uninstall
uninstall:
	rm -rf $(DESTDIR)$(LIBDIR)/libthreadviz.so
	rm -rf $(DESTDIR)$(BINDIR)/threadviz
	rm -rf $(DESTDIR)$(DATAROOTDIR)/threadviz

.PHONY: clean
clean:
	rm -f libthreadviz.so
