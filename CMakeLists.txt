cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

project(skladnik VERSION "0.5.0")

set(QT_MIN_VERSION "5.15.2")
set(KF_MIN_VERSION "5.91.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMDeprecationSettings)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(FeatureSummary)

if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_MIN_VERSION "6.5.0")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED Core Gui Widgets)

find_package(KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    I18n
    KIO
    WidgetsAddons
    XmlGui
)

find_package(KF${KF_MAJOR_VERSION}DocTools ${KF_MIN_VERSION})

if (QT_MAJOR_VERSION STREQUAL "6")
    find_package(KDEGames6 6.0.0 REQUIRED)
else()
    find_package(KF5KDEGames 7.3.0 REQUIRED)
endif()

if (QT_MAJOR_VERSION STREQUAL "6")
    ecm_set_disabled_deprecation_versions(
        QT 6.5
        KF 5.239
        KDEGAMES 6.0
    )
else()
    ecm_set_disabled_deprecation_versions(
        QT 5.15.2
        KF 5.91
        KDEGAMES 7.3
    )
endif()

add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(themes)
if(KF${KF_MAJOR_VERSION}DocTools_FOUND)
    add_subdirectory(doc)
    kdoctools_install(po)
endif()

ki18n_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
