install( FILES
    ksokoban_classic.svg
    ksokoban_classic.desktop
    #ksokoban_classic_preview.png
    DESTINATION  ${KDE_INSTALL_DATADIR}/ksokoban/themes
)
install( FILES ${CMAKE_SOURCE_DIR}/src/images/starfield.png
    RENAME ksokoban_classic_starfield.png
    DESTINATION  ${KDE_INSTALL_DATADIR}/ksokoban/themes
)
