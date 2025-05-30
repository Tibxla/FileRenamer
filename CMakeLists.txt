cmake_minimum_required(VERSION 3.16)

project(FileRenamer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Ajouter les fichiers de ressources
set(RESOURCES resources.qrc)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    splashwidget.h
    splashwidget.cpp
    ${RESOURCES}  # Inclure les ressources
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FileRenamer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        app.rc
    )
else()
    if(ANDROID)
        add_library(FileRenamer SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(FileRenamer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(FileRenamer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Configuration pour Windows
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
    target_sources(FileRenamer PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()

# Configuration pour Mac
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(FileRenamer PRIVATE icon.icns)
endif()

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FileRenamer)
endif()
set_target_properties(FileRenamer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FileRenamer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FileRenamer)
endif()
