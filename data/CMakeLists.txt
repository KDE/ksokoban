



########### install files ###############

ecm_install_icons(
    ICONS
        16-apps-ksokoban.png
        22-apps-ksokoban.png
        32-apps-ksokoban.png
        48-apps-ksokoban.png
        64-apps-ksokoban.png
        128-apps-ksokoban.png
    DESTINATION
        ${KDE_INSTALL_ICONDIR}
    THEME
        hicolor
)

install(FILES org.kde.ksokoban.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.ksokoban.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
