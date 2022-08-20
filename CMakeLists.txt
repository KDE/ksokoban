cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

project(ksokoban VERSION "0.5.0")

set(QT_MIN_VERSION "5.15.2")
set(KF_MIN_VERSION "5.91.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.84.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMInstallIcons)
include(ECMSetupVersion)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED Core Gui Widgets)

find_package(KF5 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    I18n
    KIO
    WidgetsAddons
    XmlGui
)

find_package(KF5DocTools ${KF_MIN_VERSION})

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050C00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x052e00)

add_subdirectory(src)
add_subdirectory(data)
if(KF5DocTools_FOUND)
    add_subdirectory(doc)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
