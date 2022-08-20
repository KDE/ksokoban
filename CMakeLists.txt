cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project (ksokoban)

set(QT_MIN_VERSION "5.11.0")
set(KF5_MIN_VERSION "5.46.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMOptionalAddSubdirectory)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMMarkNonGuiExecutable)
include(ECMGenerateHeaders)
include(ECMAddAppIcon)
include(GenerateExportHeader)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED Core Gui Widgets)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    I18n
    KIO
    WidgetsAddons
    XmlGui
)

find_package(KF5DocTools ${KF5_MIN_VERSION})

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050C00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x052e00)

if (${KF5Config_VERSION} STRGREATER "5.56.0")
        add_definitions(-DQT_NO_FOREACH)
        MESSAGE(STATUS "compile without foreach")
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(ksokoban_SRCS
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
)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_executable(ksokoban ${ksokoban_SRCS})
target_link_libraries(ksokoban
    KF5::I18n
    Qt5::Widgets
    Qt5::Gui
    KF5::WidgetsAddons
    Qt5::Core
    KF5::XmlGui
    KF5::KIOCore
)

add_subdirectory(data)
if(KF5DocTools_FOUND)
    add_subdirectory(doc)
endif()

install(TARGETS ksokoban ${INSTALL_TARGETS_DEFAULT_ARGS})
