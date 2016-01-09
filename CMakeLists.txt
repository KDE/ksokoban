project (ksokoban)

# find_package(KDE4 REQUIRED)
# These statements were taken from the gwenview CMakeLists.txt - thanks.

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.2.0")

find_package(ECM 1.7.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMOptionalAddSubdirectory)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMMarkNonGuiExecutable)
include(ECMGenerateHeaders)
include(ECMAddAppIcon)
include(GenerateExportHeader)
include(FeatureSummary)

find_package(Qt5 REQUIRED Core Gui Widgets)

find_package(KF5KIO 5.4 REQUIRED)

find_package(KF5 REQUIRED COMPONENTS
    # KDELibs4Support
    CoreAddons
    KIO
    IconThemes
    I18n
    Activities
    WidgetsAddons
    XmlGui
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

find_package(KF5I18n NO_MODULE)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

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

add_executable(ksokoban ${ksokoban_SRCS})
# qt5_use_modules(ksokoban Core Gui Widgets)
target_link_libraries(ksokoban
    # KF5::KDELibs4Support
    KF5::IconThemes
    KF5::I18n
    KF5::Activities
    Qt5::Widgets
    Qt5::Gui
    KF5::WidgetsAddons
    Qt5::Core
    KF5::XmlGui
    KF5::KIOCore
)
