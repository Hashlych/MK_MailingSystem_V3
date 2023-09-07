QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Contacts/contact.cpp \
    Contacts/ctgroup.cpp \
    Contacts/ctsmanager.cpp \
    Contacts/ctsmanagerie.cpp \
    Contacts/ctsmanagerui.cpp \
    Database/dbmanager.cpp \
    Emails/SMTPClient.cpp \
    Emails/emailcreateform.cpp \
    Emails/emailobject.cpp \
    Emails/emailslistui.cpp \
    Emails/emailsmanager.cpp \
    Emails/identitiesmanager.cpp \
    Emails/identitiesui.cpp \
    Emails/identity.cpp \
    Emails/identitycreateform.cpp \
    Emails/mailaddr.cpp \
    Emails/mailpkt.cpp \
    Emails/mlsprocessor.cpp \
    HttpServer/httprequest.cpp \
    HttpServer/httpresponse.cpp \
    HttpServer/httpserver.cpp \
    Logs/logobject.cpp \
    Logs/logsmanager.cpp \
    Logs/logsui.cpp \
    QtAES/qaesencryption.cpp \
    main.cpp \
    win.cpp

HEADERS += \
    Contacts/contact.h \
    Contacts/ctgroup.h \
    Contacts/ctsmanager.h \
    Contacts/ctsmanagerie.h \
    Contacts/ctsmanagerui.h \
    Database/dbmanager.h \
    Emails/SMTPClient.h \
    Emails/emailcreateform.h \
    Emails/emailobject.h \
    Emails/emailslistui.h \
    Emails/emailsmanager.h \
    Emails/identitiesmanager.h \
    Emails/identitiesui.h \
    Emails/identity.h \
    Emails/identitycreateform.h \
    Emails/mailaddr.h \
    Emails/mailpkt.h \
    Emails/mlsprocessor.h \
    HttpServer/http_defs.h \
    HttpServer/httprequest.h \
    HttpServer/httpresponse.h \
    HttpServer/httpserver.h \
    Logs/logobject.h \
    Logs/logsmanager.h \
    Logs/logsui.h \
    QtAES/aesni/aesni-enc-cbc.h \
    QtAES/aesni/aesni-enc-ecb.h \
    QtAES/aesni/aesni-key-exp.h \
    QtAES/qaesencryption.h \
    win.h

FORMS += \
    win.ui

TRANSLATIONS += \
    MK_MailingSystem_V3_fr_FR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
  rsc.qrc

DISTFILES += \
  QtAES/LICENSE
