#-------------------------------------------------
#
# Project created by QtCreator 2018-08-24T12:09:16
#
#-------------------------------------------------

QT       += core gui widgets serialport

TARGET = code_generator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++1z no_keywords

SOURCES += \
        main.cpp \
        code_generator_wizard.cpp

HEADERS += \
        code_generator_wizard.h \
        include/ast.h \
        include/code_generator.h \
        include/command_runner.h \
        include/cpp_file_generator.h \
        include/msg_types_file_generator.h \
        include/oil_file_generator.h \
        include/parser.h \
        include/replacer.h \
        include/util.h

FORMS +=

INCLUDEPATH += C:/boost_mingw/boost/include/boost-1_67

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
