RESOLUTION=+W32 +H32

IMAGES=eastwall.ppm floor.ppm goal.ppm horizwall.ppm man.ppm object.ppm \
	saveman.ppm treasure.ppm vertiwall.ppm westwall.ppm

# no antialias
#ANTIALIAS=

# normal antialias
ANTIALIAS=+A

# slow antialias
#ANTIALIAS=+A0 +R5

POVRAY=povray +L/usr/local/lib/povray3/include +L.. $(ANTIALIAS)

all-local: ../bin2c data.c

../bin2c: $(srcdir)/../bin2c.c
	(cd .. && $(MAKE) bin2c)

#eastwall.ppm: ../eastwall.pov ../wall_common.inc
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga
#
#horizwall.ppm: ../horizwall.pov ../wall_common.inc
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga
#
#vertiwall.ppm: ../vertiwall.pov ../wall_common.inc
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga
#
#westwall.ppm: ../westwall.pov ../wall_common.inc
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga
#
#
#treasure.ppm: ../treasure.pov ../goal.pov
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga
#
#object.ppm: ../object.pov ../floor.pov
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga
#
#man.ppm: ../man.pov ../man_common.inc ../floor.pov
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga
#
#saveman.ppm: ../saveman.pov ../man_common.inc ../goal.pov
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga
#
#goal.ppm: ../goal.pov
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga
#
#floor.ppm: ../floor.pov
#	$(POVRAY) $(RESOLUTION) +I$< +Oimage.tga
#	tgatoppm <image.tga >$@
#	rm -f image.tga

data.c: $(IMAGES) ../bin2c
	list=""; for i in $(IMAGES); do list="$$list $(srcdir)/$$i"; done; \
	../bin2c $$list

DISTCLEANFILES=data.c
EXTRA_DIST=$(IMAGES)
