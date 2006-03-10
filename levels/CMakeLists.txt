kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


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
