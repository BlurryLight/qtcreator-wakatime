cmake_minimum_required(VERSION 3.16)

# for setting the vcpkg path to be used
set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
#for stopping qtcreator using it's on vcpkg setup
set(QT_CREATOR_SKIP_VCPKG_SETUP ON)

# Remove when sharing with others.
#list(APPEND CMAKE_PREFIX_PATH "/Path/to/Qtcreator/Build/Dir")

project(Wakatime)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

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
	OpenSSL::SSL
	OpenSSL::Crypto
	ZLIB::ZLIB
	SOURCES
	#.github/workflows/build_cmake.yml
	#.github/workflows/README.md
	README.md
	${WAKA_HEADERS}
	${WAKA_SOURCES}
	${WAKA_FORMS}
	${WAKA_RESOURCES}
)
