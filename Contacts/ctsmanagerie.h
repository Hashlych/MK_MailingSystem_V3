#ifndef CTSMANAGERIE_H
#define CTSMANAGERIE_H

/*
 * This class is responsible for the import/export operations of 'contacts' and 'groups'.
 * Allowed file format: CSV / text
*/

#include <QObject>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSharedPointer>

#include "contact.h"
#include "ctgroup.h"
#include "Logs/logobject.h"


typedef struct S_CT_Datas {
  quint32 id = 0;
  QByteArray firstName, lastName, email, grpName;
} S_CT_Datas;


typedef struct S_GRP_Datas {
  quint32 id = 0;
  QByteArray name, desc;
} S_GRP_Datas;


typedef struct S_IDT_Datas {
  quint32 id = 0;
  QByteArray name, addr, code;

  struct {
    QByteArray access = "", refresh = "", type = "";
    quint32 expires_in = 0;
  } tokens;
} S_IDT_Datas;



class CTSManagerIE : public QObject
{
  Q_OBJECT

public:
  CTSManagerIE(const QByteArray &defaultLocation = QByteArray(), QObject *parent = nullptr);
  ~CTSManagerIE();


  // Mutators:
  void SetDefaultLocation(const QByteArray &path);


  // Accessors:
  const QByteArray & GetDefaultLocation() const;


signals:
  void SI_AddLog(const LOG_TYPES &type, const QByteArray &text, const QDateTime &dt = QDateTime::currentDateTime());

  void ContactsImported(QVector<S_CT_Datas> contacts);
  void ContactsExported(QVector<QSharedPointer<Contact*>> cts);

  void GroupsImported(QVector<S_GRP_Datas> groups);
  void GroupsExported(QVector<QSharedPointer<CTGroup*>> grps);


public slots:
  void ImportContacts(QSharedPointer<CTGroup*> grp, const QByteArray &filePath);
  void ImportGroups(const QByteArray &filePath);

  void ExportContacts(QVector<QSharedPointer<Contact *> > cts, const QByteArray &filePath);
  void ExportGroups(QVector<QSharedPointer<CTGroup*>> grps, const QByteArray &filePath);


private:
  QByteArray _def_location;

  QVector<QSharedPointer<Contact*>> _buffer_contacts;
  QVector<QSharedPointer<CTGroup*>> _buffer_groups;
};

#endif // CTSMANAGERIE_H
