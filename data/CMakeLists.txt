kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES ksokoban.desktop )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#
#KDE_ICON = ksokoban
#
#xdg_apps_DATA = ksokoban.desktop
#
