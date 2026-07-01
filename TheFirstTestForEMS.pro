QT       += core gui widgets charts svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = TheFirstTestForEMS
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    employee.cpp \
    engineer.cpp \
    leader.cpp \
    chiefengineer.cpp \
    employeemanager.cpp \
    addeditdialog.cpp \
    statdialog.cpp \
    logindialog.cpp \
    uistyle.cpp

HEADERS += \
    mainwindow.h \
    employee.h \
    engineer.h \
    leader.h \
    chiefengineer.h \
    employeemanager.h \
    addeditdialog.h \
    statdialog.h \
    logindialog.h \
    uistyle.h

FORMS += \
    mainwindow.ui \
    addeditdialog.ui \
    logindialog.ui

RESOURCES += \
    resources/resources.qrc

# 构建时自动复制示例数据到输出目录
COPIES += data
data.files = employee.json
data.path = $$OUT_PWD
