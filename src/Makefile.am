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

EXTRA_DIST = epanet2csvw.cpp inptools-about.c inptools-file-dialog.cpp \
	Makefile.in epanet2csv.dsp epanet2csv.dsw inpproj.dsw inpproj.dsp \
	inp2shpw.cpp
noinst_HEADERS = inpproj.h inp2shp.h shp2inp.h

EPANET_SOURCES = @EPANET_SRC@/epanet.c \
	@EPANET_SRC@/hash.c \
	@EPANET_SRC@/hydraul.c \
	@EPANET_SRC@/inpfile.c \
	@EPANET_SRC@/input1.c \
	@EPANET_SRC@/input2.c \
	@EPANET_SRC@/input3.c \
	@EPANET_SRC@/mempool.c \
	@EPANET_SRC@/output.c \
	@EPANET_SRC@/quality.c \
	@EPANET_SRC@/report.c \
	@EPANET_SRC@/rules.c \
	@EPANET_SRC@/smatrix.c
	
SHAPELIB_SOURCES = @SHAPELIB_SRC@/shpopen.c \
	@SHAPELIB_SRC@/dbfopen.c \
	@SHAPELIB_SRC@/shptree.c \
	@SHAPELIB_SRC@/safileio.c
	
ALL_SOURCES = $(epanet2csv_SOURCES) $(inpproj_SOURCES) $(inp2shp_SOURCES) \
	$(shp2inp_SOURCES) epanet2csvw.cpp inp2shpw.cpp inptools-about.c \
	inptools-file-dialog.cpp	

indent: $(ALL_SOURCES)
	indent $(ALL_SOURCES) -kr -i8

epanet2csvw.exe: epanet2csvw.cpp
	i586-mingw32msvc-windres ../data/icons/pump.rc pump.o
	i586-mingw32msvc-g++ -mwindows epanet2csvw.cpp pump.o -o epanet2csvw.exe

inp2shp.exe: inp2shp.c
	i586-mingw32msvc-windres ../data/icons/pump.rc pump.o
	i586-mingw32msvc-gcc -I@EPANET_SRC@ -I@SHAPELIB_SRC@ pump.o inp2shp.c $(EPANET_SOURCES) $(SHAPELIB_SOURCES) -o inp2shp.exe
	
inp2shpw.exe: inp2shpw.cpp
	i586-mingw32msvc-windres ../data/icons/pump.rc pump.o
	i586-mingw32msvc-g++ -mwindows inp2shpw.cpp pump.o -o inp2shpw.exe -lshell32 -lole32

inptools-about.exe: inptools-about.c
	i586-mingw32msvc-windres ../data/icons/pump.rc pump.o
	i586-mingw32msvc-gcc -mwindows inptools-about.c pump.o -o inptools-about.exe

inptools-file-dialog.exe: inptools-file-dialog.cpp
	i586-mingw32msvc-windres ../data/icons/pump.rc pump.o
	i586-mingw32msvc-g++ -mwindows inptools-file-dialog.cpp pump.o -o inptools-file-dialog.exe
