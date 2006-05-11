kde4_header()




########### install files ###############

install_files(  FILES data.c )

kde4_footer()



#original Makefile.am contents follow:

#
#noinst_DATA = data.c
#
#data.c: ../images/bin2c level.data
#	../images/bin2c "" $(srcdir)/level.data
#
#CLEANFILES=data.c
#EXTRA_DIST=level.data
