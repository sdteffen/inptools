SUBDIRS = locale
EXTRA_DIST = gennsh.c inptools-setup.nsi langmacros.nsh Makefile.in

gennsh: gennsh.c
	gcc `pkg-config --cflags --libs glib-2.0` gennsh.c -o gennsh

de.nsh: gennsh
	mkdir -p ../../build/win32/locale/de/LC_MESSAGES
	msgfmt -o ../../build/win32/locale/de/LC_MESSAGES/inptools.mo ../../po/de.po
	./gennsh de_DE.UTF8 > locale/de.nsh
	
