all: data.c

data.c: ../images/bin2c level.data
	../images/bin2c level.data

../images/bin2c: ../images/bin2c.c
	$(MAKE) -C ../images bin2c

DISTCLEANFILES=data.c
EXTRA_DIST=level.data
