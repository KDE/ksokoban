
noinst_DATA = data.c

data.c: ../images/bin2c level.data
	../images/bin2c "" $(srcdir)/level.data

../images/bin2c: $(srcdir)/../images/bin2c.c
	(cd ../images && $(MAKE) bin2c)

CLEANFILES=data.c
EXTRA_DIST=level.data
