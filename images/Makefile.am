SUBDIRS=20 32 40

all-local: bin2c data.c

EXTRA_PROGRAMS = bin2c

bin2c: $(srcdir)/bin2c.c
	@echo "#############################################################"
	@echo "If the file 'bin2c.c' fails to compile then you probably"
	@echo "don't have zlib version 1.0.3 or later properly installed"
	@echo "on your system. You can get zlib from it's homepage at:"
	@echo "http://www.cdrom.com/pub/infozip/zlib/zlib.html"
	@echo
	@echo "If you your zlib is installed at a nonstandard place then"
	@echo "use the --with-extra-includes and --with-extra-libs configure"
	@echo "options to tell it's location."
	@echo "#############################################################"
	$(CC) $(CFLAGS) $(DEFS) $(all_includes) $(srcdir)/bin2c.c -o bin2c $(all_libraries) $(LIBZ)

data.c: bin2c background.bmp
	./bin2c $(srcdir)/background.bmp

vertiwall horizwall eastwall westwall object:
	x-povray +L/usr/local/lib/povray3/include +W200 +H200 +I$@.pov +Oimage.tga +p +d
	rm -f image.tga

treasure:
	x-povray +L/usr/local/lib/povray3/include +W100 +H100 +I$@.pov +Oimage.tga +p +d
	rm -f image.tga

POVFILES=eastwall.pov   goal.pov       man.pov        saveman.pov    \
	 vertiwall.pov  floor.pov      horizwall.pov  object.pov     \
	 treasure.pov   westwall.pov   man_common.inc wall_common.inc

EXTRA_DIST=background.bmp bin2c.c $(POVFILES)
DISTCLEANFILES=data.c bin2c
