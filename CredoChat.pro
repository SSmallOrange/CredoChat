QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = CredoChat

# 使用相对路径设置可执行文件和中间文件的生成路径
DESTDIR = ./bin                 # 可执行文件的输出目录
OBJECTS_DIR = ./generate/objects   # 中间文件的存放目录
MOC_DIR = ./generate/moc           # moc 文件的生成目录
UI_DIR = ./generate/ui             # .ui 文件生成的 .h 文件存放目录

# 设置构建目录
BUILD_DIR = ./build             # 构建目录

ICON += "chat.png"

INCLUDEPATH += ./src

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/Base/HttpServer/httpmanager.cpp \
    src/Base/Localcfg/localcfg.cpp \
    src/Base/global.cpp \
    src/Login/logindialog.cpp \
    src/Login/registerdialog.cpp

HEADERS += \
    mainwindow.h \
    src/Base/HttpServer/httpmanager.h \
    src/Base/Localcfg/localcfg.h \
    src/Base/global.h \
    src/Base/singleton.h \
    src/Login/logindialog.h \
    src/Login/registerdialog.h

FORMS += \
    mainwindow.ui \
    src/Login/logindialog.ui \
    src/Login/registerdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

DISTFILES += \
    src/Base/Localcfg/Localcfg.xml
