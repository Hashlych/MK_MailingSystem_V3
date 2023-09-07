#ifndef EMAILOBJECT_H
#define EMAILOBJECT_H

#include <QObject>
#include <QDateTime>

#include "Contacts/ctsmanager.h"


class EmailObject : public QObject
{
  Q_OBJECT

public:
  explicit EmailObject(QObject *parent = nullptr);
  EmailObject(const QByteArray &subject, const QByteArray &content, const quint64 &dt_exec = 0, const quint32 &id = 0,
              QObject *parent = nullptr);
  EmailObject(const EmailObject &other);
  ~EmailObject();

  bool operator == (const EmailObject &other);
  bool operator != (const EmailObject &other);
  EmailObject * operator = (const EmailObject &other);


  void ShowDebugInfos();

  void AddContact(QSharedPointer<Contact*> pct);
  void AddContacts(QVector<QSharedPointer<Contact*>> cts);

  void AddGroup(QSharedPointer<CTGroup*> pgrp);
  void AddGroups(QVector<QSharedPointer<CTGroup*>> grps);


  void SetID(const quint32 &id);
  void SetSubject(const QByteArray &subject);
  void SetContent(const QByteArray &content);
  void SetDT_Exec(const quint64 &dt);
  void SetProcessing(const bool &processing);
  void SetSent(const bool &sent);
  void SetContacts(QVector<QSharedPointer<Contact*>> cts);
  void SetGroups(QVector<QSharedPointer<CTGroup*>> grps);

  const quint32 & GetID() const;
  const QByteArray & GetSubject() const;
  const QByteArray & GetContent() const;
  const quint64 & GetDT_Exec() const;
  const bool & IsProcessing() const;
  const bool & HasBeenSent() const;
  const QVector<QSharedPointer<Contact*>> & GetContacts() const;
  const QVector<QSharedPointer<CTGroup*>> & GetGroups() const;


private:
  quint32     _id         = 0;
  QByteArray  _subject    = QByteArray(),
              _content    = QByteArray();
  quint64     _dt_exec    = 0;
  bool        _processing = false, _sent = false;

  QVector<QSharedPointer<Contact*>> _contacts;
  QVector<QSharedPointer<CTGroup*>> _groups;
};

Q_DECLARE_METATYPE(EmailObject*)

#endif // EMAILOBJECT_H
