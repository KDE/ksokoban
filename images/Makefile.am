
noinst_DATA = data.c
bin2c_SOURCES = bin2c.c
bin2c_LDFLAGS = $(all_libraries)
bin2c_LDADD   = $(LIBZ)

noinst_PROGRAMS = bin2c

test:
	x-povray +W200 +H200 +I$@.pov +O$@.png +p +d
	rm -f $@.png

POVFILES=floor_common.inc goal.pov halfstone_1.pov halfstone_2.pov halfstone_3.pov halfstone_4.pov man.pov man_common.inc object.pov saveman.pov stone_1.pov stone_2.pov stone_3.pov stone_4.pov stone_5.pov stone_6.pov stone_common.inc treasure.pov

IMAGES=goal.png halfstone_1.png halfstone_2.png halfstone_3.png halfstone_4.png man.png object.png saveman.png stone_1.png stone_2.png stone_3.png stone_4.png stone_5.png stone_6.png treasure.png starfield.png

EXTRA_DIST = $(POVFILES) $(IMAGES)
CLEANFILES = data.c

RESOLUTION=+W96 +H96
STONE_RESOLUTION=+W96 +H48
HALFSTONE_RESOLUTION=+W48 +H48

# no antialias
#ANTIALIAS=

# normal antialias
#ANTIALIAS=+A

# slow antialias
ANTIALIAS=+A0 +R9

POVRAY=povray $(ANTIALIAS)

data.c: $(IMAGES) bin2c
	list=""; for i in $(IMAGES); do list="$$list $(srcdir)/$$i"; done; \
	./bin2c "" $$list

############################################################################
# Povray rules to generate images
#
#halfstone_1.png: halfstone_1.pov stone_common.inc
#	$(POVRAY) $(HALFSTONE_RESOLUTION) +I$< +O$@
#
#halfstone_2.png: halfstone_2.pov stone_common.inc
#	$(POVRAY) $(HALFSTONE_RESOLUTION) +I$< +O$@
#
#halfstone_3.png: halfstone_3.pov stone_common.inc
#	$(POVRAY) $(HALFSTONE_RESOLUTION) +I$< +O$@
#
#halfstone_4.png: halfstone_4.pov stone_common.inc
#	$(POVRAY) $(HALFSTONE_RESOLUTION) +I$< +O$@
#
#
#stone_1.png: stone_1.pov stone_common.inc
#	$(POVRAY) $(STONE_RESOLUTION) +I$< +O$@
#
#stone_2.png: stone_2.pov stone_common.inc
#	$(POVRAY) $(STONE_RESOLUTION) +I$< +O$@
#
#stone_3.png: stone_3.pov stone_common.inc
#	$(POVRAY) $(STONE_RESOLUTION) +I$< +O$@
#
#stone_4.png: stone_4.pov stone_common.inc
#	$(POVRAY) $(STONE_RESOLUTION) +I$< +O$@
#
#stone_5.png: stone_5.pov stone_common.inc
#	$(POVRAY) $(STONE_RESOLUTION) +I$< +O$@
#
#stone_6.png: stone_6.pov stone_common.inc
#	$(POVRAY) $(STONE_RESOLUTION) +I$< +O$@
#
#
#treasure.png: treasure.pov goal.pov floor_common.inc
#	$(POVRAY) $(RESOLUTION) +I$< +O$@
#
#object.png: object.pov floor_common.inc
#	$(POVRAY) $(RESOLUTION) +I$< +O$@
#
#man.png: man.pov man_common.inc floor_common.inc
#	$(POVRAY) $(RESOLUTION) +I$< +O$@
#
#saveman.png: saveman.pov man_common.inc goal.pov floor_common.inc
#	$(POVRAY) $(RESOLUTION) +I$< +O$@
#
#goal.png: goal.pov floor_common.inc
#	$(POVRAY) $(RESOLUTION) +I$< +O$@

