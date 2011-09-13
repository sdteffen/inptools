SUBDIRS = InpToolsExt

datadir = @datadir@
localedir = $(datadir)/locale

bin_PROGRAMS = epanet2csv inpproj inp2shp shp2inp

epanet2csv_SOURCES = epanet2csv.c
epanet2csv_CFLAGS = $(AM_CFLAGS)
epanet2csv_LDFLAGS = $(all_libraries)

inpproj_SOURCES = inpproj.c
inpproj_CFLAGS = $(AM_CFLAGS) -DCLE=1 -DLOCALEDIR=\"$(localedir)\"
INCLUDES = $(all_includes)
inpproj_LDFLAGS = $(all_libraries)

inp2shp_SOURCES = inp2shp.c
inp2shp_CFLAGS = $(AM_CFLAGS)
inp2shp_LDFLAGS = $(all_libraries)

shp2inp_SOURCES = shp2inp.c
shp2inpi_CFLAGS = $(AM_CFLAGS)
shp2inp_LDFLAGS = $(all_libraries)

EXTRA_DIST = Makefile.in epanet2csv.dsp epanet2csv.dsw inpproj.dsw inpproj.dsp
noinst_HEADERS = inpproj.h inp2shp.h shp2inp.h

