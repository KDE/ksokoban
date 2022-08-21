ecm_setup_version(${ksokoban_VERSION}
    VARIABLE_PREFIX KSOKOBAN
    VERSION_HEADER version.h
)

add_executable(ksokoban
    Bookmark.cpp
    ImageData.cpp
    LevelMap.cpp
    Map.cpp
    Move.cpp
    PlayField.cpp
    History.cpp
    InternalCollections.cpp
    main.cpp
    MapDelta.cpp
    MoveSequence.cpp
    StaticImage.cpp
    HtmlPrinter.cpp
    LevelCollection.cpp
    MainWindow.cpp
    ModalLabel.cpp
    PathFinder.cpp

    ksokoban.qrc
)

target_link_libraries(ksokoban
    KF5::I18n
    KF5::WidgetsAddons
    KF5::XmlGui
    KF5::KIOCore
)

install(TARGETS ksokoban ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
