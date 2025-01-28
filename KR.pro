QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    admin.cpp \
    loginform.cpp \
    main.cpp \
    mainwindow.cpp \
    seller.cpp \
    user.cpp \
    warehouse.cpp

HEADERS += \
    admin.h \
    loginform.h \
    mainwindow.h \
    seller.h \
    user.h \
    warehouse.h

FORMS += \
    AddEmployeeDialog.ui \
    AddMedicineDialog.ui \
    AdminForm.ui \
    LoginForm.ui \
    SalesHistoryForm.ui \
    SellerForm.ui \
    WarehouseForm.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += users.json \
    build/Desktop_Qt_6_7_3_MinGW_64_bit-Debug/debug/users.json

win32:CONFIG(release, debug|release): \
    QMAKE_POST_LINK += copy /Y $$PWD/users.json $$OUT_PWD/
