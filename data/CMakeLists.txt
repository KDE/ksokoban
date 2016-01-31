



########### install files ###############

file(GLOB app_icons *-app-ksokoban.*)
ecm_install_icons(ICONS ${app_icons}
                  DESTINATION ${ICON_INSTALL_DIR}
                  THEME hicolor
)

install(FILES ksokoban.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
