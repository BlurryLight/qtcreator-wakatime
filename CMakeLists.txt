cmake_minimum_required(VERSION 3.16)

# Remove when sharing with others.
#list(APPEND CMAKE_PREFIX_PATH "/Path/to/Qtcreator/Build/Dir")

project(Wakatime)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(SetupZLIB)
include(SetupOpenSSL)

message(STATUS "CMAKE_PREFIX_PATH IS ${CMAKE_PREFIX_PATH}")

find_package(QtCreator COMPONENTS Core ProjectExplorer REQUIRED)
find_package(QT NAMES Qt6 Qt5
	COMPONENTS Core Network Widgets REQUIRED)
set(QtX Qt${QT_VERSION_MAJOR})

set(WAKA_SOURCES
	waka_plugin.cpp
	waka_options.cpp
	waka_options_page.cpp
	waka_options_widget.cpp
	cligetter.cpp
)

set(WAKA_HEADERS
	waka_plugin.h
	waka_options.h
	waka_constants.h
	waka_global.h
	waka_options_page.h
	waka_options_widget.h
	cligetter.h #Handle getting wakatime-cli release
)

set(WAKA_FORMS waka_options.ui)
set(WAKA_RESOURCES waka.qrc)

add_qtc_plugin(${PROJECT_NAME}
	PLUGIN_DEPENDS
	QtCreator::Core
	QtCreator::TextEditor
	QtCreator::ProjectExplorer
	DEPENDS
	${QtX}::Widgets
	QtCreator::ExtensionSystem
	QtCreator::Utils
	LibreSSL::SSL
	SOURCES
	#.github/workflows/build_cmake.yml
	#.github/workflows/README.md
	README.md
	${WAKA_HEADERS}
	${WAKA_SOURCES}
	${WAKA_FORMS}
	${WAKA_RESOURCES}
)
