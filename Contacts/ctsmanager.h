#ifndef CTSMANAGER_H
#define CTSMANAGER_H

#include <QObject>

#include "ctgroup.h"
#include "Logs/logobject.h"


class CTSManager : public QObject
{
  Q_OBJECT

public:
  explicit CTSManager(QObject *parent = nullptr);


  bool GroupExists(const CTGroup &grp);
  bool GroupExists(const QByteArray &name);


  QVector<QSharedPointer<CTGroup*>> GetGroups() const;
  QSharedPointer<CTGroup*> GetDefaultGroup() const;
  QSharedPointer<CTGroup*> GetGroupByName(const QByteArray &name) const;
  QSharedPointer<CTGroup*> GetGroupByID(const quint32 &id) const;


signals:
  void SI_AddLog(const LOG_TYPES &type, const QByteArray &text, const QDateTime &dt = QDateTime::currentDateTime());

  void SI_GroupAdded(const QSharedPointer<CTGroup*> grp);
  void SI_GroupRemoved(const QSharedPointer<CTGroup*> grp);

  void SI_GRP_ContactAdded(const QSharedPointer<Contact*> ct);
  void SI_GRP_ContactDeleted(const QSharedPointer<Contact*> pct);


public slots:
  void SL_AddGroup(const QByteArray &name, const QByteArray &desc, const quint32 &id);
  void SL_AddGroup(CTGroup *grp);

  void SL_RemoveGroup(const QSharedPointer<CTGroup*> pgrp);

  void SL_GRP_AddContact(Contact *ct, const quint32 &id = 0);
  void SL_GRP_DelContact(QSharedPointer<Contact*> pct);


private:
  CTGroup **_def_grp = nullptr;
  QVector<CTGroup*> _groups;
};

#endif // CTSMANAGER_H
