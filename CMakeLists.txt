cmake_minimum_required(VERSION 3.16)

project(FileRenamer VERSION 1.2.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg Test)

if(MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/utf-8>)
endif()

add_library(filerenamer_core STATIC
    renamecore.h
    renamecore.cpp
    displaytext.h
    elidedlabel.h
    elidedlabel.cpp
    folderdropzone.h
    folderdropzone.cpp
    iconview.h
    iconview.cpp
    nomenclaturestore.h
    nomenclaturestore.cpp
)
target_include_directories(filerenamer_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filerenamer_core PUBLIC Qt6::Widgets)

qt_add_executable(FileRenamer WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    theme.h
    theme.cpp
    resources.qrc
)
target_link_libraries(FileRenamer PRIVATE filerenamer_core Qt6::Svg)

if(WIN32)
    enable_language(RC)
    target_sources(FileRenamer PRIVATE app.rc)
endif()

if(APPLE)
    set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(FileRenamer PRIVATE icon.icns)
    set_target_properties(FileRenamer PROPERTIES MACOSX_BUNDLE_ICON_FILE icon.icns)
endif()

set_target_properties(FileRenamer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.tibxla.FileRenamer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
)

include(GNUInstallDirs)
install(TARGETS FileRenamer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

enable_testing()
add_executable(filerenamer_tests tests/filerenamer_tests.cpp)
target_link_libraries(filerenamer_tests PRIVATE filerenamer_core Qt6::Test)
add_test(NAME filerenamer_tests COMMAND filerenamer_tests)
set_tests_properties(filerenamer_tests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
