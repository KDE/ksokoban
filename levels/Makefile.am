all-local: data.c

data.c: ../images/bin2c level.data
	../images/bin2c level.data

../images/bin2c: ../images/bin2c.c
	(cd ../images && $(MAKE) bin2c)

DISTCLEANFILES=data.c
EXTRA_DIST=level.data
