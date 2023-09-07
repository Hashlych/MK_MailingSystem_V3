#ifndef CTGROUP_H
#define CTGROUP_H

#include <QObject>
#include <QSharedPointer>

#include "contact.h"


class CTGroup : public QObject
{
  Q_OBJECT

public:
  explicit CTGroup(QObject *parent = nullptr);
  CTGroup(const QByteArray &name, const QByteArray &desc = QByteArray(),
          const quint32 &id = 0, QObject *parent = nullptr);
  CTGroup(const CTGroup &other);
  ~CTGroup();


  bool operator == (const CTGroup &other);
  bool operator != (const CTGroup &other);
  CTGroup * operator = (const CTGroup &other);


  void SetID(const quint32 &id);
  void SetName(const QByteArray &name);
  void SetDescription(const QByteArray &desc);


  const quint32 & GetID() const;
  const QByteArray & GetName() const;
  const QByteArray & GetDescription() const;
  QVector<QSharedPointer<Contact*>> GetContactsList() const;


signals:
  // signal emitted when a contact is successfuly added to the group:
  void SI_ContactAdded(const QSharedPointer<Contact*> &ct);

  void SI_ContactDeleted(const QSharedPointer<Contact*> &ct);

  // signal emitted whenever group's datas changed:
  void DatasChanged();


public slots:
  // slots called to add a contact to the group:
  void SL_AddContact(const QByteArray &firstName, const QByteArray &lastName, const QByteArray &email,
                     const quint32 &id);
  void SL_AddContact(Contact *ct);

  void SL_DelContact(const QSharedPointer<Contact*> pct);


private:
  quint32     _id = 0;
  QByteArray  _name, _desc;
  QVector<Contact*> _contacts;
};


Q_DECLARE_METATYPE(CTGroup*)


#endif // CTGROUP_H
