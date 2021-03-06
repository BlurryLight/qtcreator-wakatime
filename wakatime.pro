DEFINES += WAKATIME_LIBRARY

# Wakatime files

SOURCES += \
    waka_plugin.cpp \
    waka_options.cpp \
    waka_options_page.cpp \
    waka_options_widget.cpp

HEADERS += \
    waka_plugin.h \
    waka_options.h \
    waka_constants.h \
    waka_global.h \
    waka_options_page.h \
    waka_options_widget.h

FORMS += \
    waka_options.ui

RESOURCES += \
    waka.qrc

QT += core network widgets
CONFIG += c++17
# Qt Creator linking
## Either set the IDE_SOURCE_TREE when running qmake,
## or set the QTC_SOURCE environment variable, to override the default setting
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = $$(QTC_SOURCE)
isEmpty(IDE_SOURCE_TREE): IDE_SOURCE_TREE = qt-src

## Either set the IDE_BUILD_TREE when running qmake,
## or set the QTC_BUILD environment variable, to override the default setting
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = qt-bin

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
## USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = Wakatime
QTC_LIB_DEPENDS += \
    # nothing here at this time

QTC_PLUGIN_DEPENDS += \
    coreplugin projectexplorer

QTC_PLUGIN_RECOMMENDS += \
    # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)
DESTDIR = $${QTP_OUTPUT_DIR}
isEmpty(QTP_OUTPUT_DIR): DESTDIR=./output
