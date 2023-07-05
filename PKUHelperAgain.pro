QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    backend/iaaa.cpp \
    backend/pkuportal.cpp \
    backend/scoresheet.cpp \
    backend/unifieddatabase.cpp \
    frontend/aboutpage.cpp \
    frontend/clickablelabel.cpp \
    frontend/coursemanagepage.cpp \
    frontend/coursequerypage.cpp \
    frontend/gradequerypage.cpp \
    frontend/logindialog.cpp \
    main.cpp \
    frontend/mainwindow.cpp \
    backend/CourseTable.cpp

HEADERS += \
    backend/iaaa.h \
    backend/pkuportal.h \
    backend/scoresheet.h \
    backend/unifieddatabase.h \
    frontend/aboutpage.h \
    frontend/clickablelabel.h \
    frontend/coursemanagepage.h \
    frontend/coursequerypage.h \
    frontend/gradequerypage.h \
    frontend/logindialog.h \
    frontend/mainwindow.h \
    backend/CourseTable.hpp

FORMS += \
    frontend/aboutpage.ui \
    frontend/coursemanagepage.ui \
    frontend/coursequerypage.ui \
    frontend/gradequerypage.ui \
    frontend/logindialog.ui \
    frontend/mainwindow.ui

# ICON
# RC_ICONS = favicon.png

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
