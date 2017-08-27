



########### install files ###############

ecm_install_icons(
    ICONS
        16-mimetypes-application-x-ksokoban.png
        22-mimetypes-application-x-ksokoban.png
        32-mimetypes-application-x-ksokoban.png
        48-mimetypes-application-x-ksokoban.png
        64-mimetypes-application-x-ksokoban.png
        128-mimetypes-application-x-ksokoban.png
    DESTINATION
        ${KDE_INSTALL_ICONDIR}
    THEME
        hicolor
)

install(FILES ksokoban.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
