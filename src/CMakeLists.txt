# SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(images)
add_subdirectory(levels)

ecm_setup_version(${ksokoban_VERSION}
    VARIABLE_PREFIX KSOKOBAN
    VERSION_HEADER version.h
)

add_executable(ksokoban
    Bookmark.cpp
    LevelMap.cpp
    Map.cpp
    Move.cpp
    PlayField.cpp
    PlayFieldView.cpp
    History.cpp
    InternalCollections.cpp
    main.cpp
    MoveSequence.cpp
    HtmlPrinter.cpp
    LevelCollection.cpp
    MainWindow.cpp
    PathFinder.cpp
    StoneIndex.cpp
    GroundItem.cpp

    ksokoban.qrc
)

if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(ksokoban KDEGames6)
else()
    target_link_libraries(ksokoban KF5KDEGames)
endif()

target_link_libraries(ksokoban
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::KIOCore
)

install(TARGETS ksokoban ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES org.kde.ksokoban.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.ksokoban.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
