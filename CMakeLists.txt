cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

project(ksokoban VERSION "0.5.0")

set(QT_MIN_VERSION "5.15.2")
set(KF_MIN_VERSION "5.91.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMDeprecationSettings)
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

ecm_set_disabled_deprecation_versions(
    QT 5.15.2
    KF 5.91
)
# added by KDE_COMPILERSETTINGS_LEVEL 5.85
remove_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
)

add_subdirectory(src)
add_subdirectory(data)
if(KF5DocTools_FOUND)
    add_subdirectory(doc)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
