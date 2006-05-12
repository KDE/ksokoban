



########### install files ###############

install_files(  FILES data.c )




#original Makefile.am contents follow:

#
#noinst_DATA = data.c
#
#data.c: ../images/bin2c level.data
#	../images/bin2c "" $(srcdir)/level.data
#
#CLEANFILES=data.c
#EXTRA_DIST=level.data
