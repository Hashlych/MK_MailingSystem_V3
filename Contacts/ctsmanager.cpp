#include "ctsmanager.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


CTSManager::CTSManager(QObject *parent)
  : QObject{parent}
{
}



bool CTSManager::GroupExists(const CTGroup &grp)
{
  QVector<CTGroup*>::const_iterator it = _groups.constBegin(), ite = _groups.constEnd();

  for (; it != ite; it++) {
      if ( *(*it) == grp )
        return true;
    }

  return false;
}


bool CTSManager::GroupExists(const QByteArray &name)
{
  QVector<CTGroup*>::const_iterator it = _groups.constBegin(), ite = _groups.constEnd();

  for (; it != ite; it++) {
      if ((*it)->GetName().compare(name, Qt::CaseSensitive) == 0)
        return true;
    }

  return false;
}



void CTSManager::SL_AddGroup(const QByteArray &name, const QByteArray &desc, const quint32 &id)
{
  if (!name.isEmpty()) {
      if (this->GroupExists(name))
        return;

      CTGroup *grp = new CTGroup(name, desc, id, this);

      connect(grp, &CTGroup::SI_ContactAdded, this, &CTSManager::SI_GRP_ContactAdded);

      if (!grp->parent())
        grp->setParent(this);

      _groups.append(grp);

      emit SI_GroupAdded(QSharedPointer<CTGroup*>::create(grp));
    }
}



void CTSManager::SL_AddGroup(CTGroup *grp)
{
  if (grp) {
      if (this->GroupExists(*grp))
        return;

      connect(grp, &CTGroup::SI_ContactAdded, this, &CTSManager::SI_GRP_ContactAdded);
      connect(grp, &CTGroup::SI_ContactDeleted, this, &CTSManager::SI_GRP_ContactDeleted);

      if (!grp->parent())
        grp->setParent(this);

      _groups.append(grp);

      emit SI_GroupAdded(QSharedPointer<CTGroup*>::create(grp));
    }
}



void CTSManager::SL_RemoveGroup(const QSharedPointer<CTGroup*> pgrp)
{
  if (pgrp.isNull())
    return;

  CTGroup *grp = *pgrp.data();
  QVector<CTGroup*>::const_iterator it = _groups.constBegin(), ite = _groups.constEnd();

  for (; it != ite; it++) {
      if ((*it) == grp) {
          /*qDebug().noquote() << QString("CTSManager::SL_RemoveGroup | removing group 0x%1 (%2)")
                                .arg((qint64)grp, 8, 16, QChar('0'))
                                .arg(grp->GetName());*/

          disconnect((*it), &CTGroup::SI_ContactAdded, this, &CTSManager::SI_GRP_ContactAdded);
          disconnect((*it), &CTGroup::SI_ContactDeleted, this, &CTSManager::SI_GRP_ContactDeleted);

          _groups.takeAt( _groups.indexOf((*it)) );

          emit SI_GroupRemoved(pgrp);

          break;
        }
    }
}



void CTSManager::SL_GRP_AddContact(Contact *ct, const quint32 &id)
{
  if (ct) {
      CTGroup *grp = dynamic_cast<CTGroup*>(ct->parent());

      if (grp) {
          grp->SL_AddContact(ct);
        } else {
          if (id > 0) {
              QVector<CTGroup*>::const_iterator it = _groups.constBegin(), ite = _groups.constEnd();

              for (; it != ite; it++) {
                  if ((*it)->GetID() == id) {
                      (*it)->SL_AddContact(ct);
                      break;
                    }
                }
            }
        }
    }
}



void CTSManager::SL_GRP_DelContact(QSharedPointer<Contact*> pct)
{
  if (pct.isNull())
    return;

  Contact *ct = *pct.data();
  CTGroup *grp = dynamic_cast<CTGroup*>(ct->parent());

  qDebug().noquote() << QString("CTSManager::SL_GRP_DelContact | ct = 0x%1 (ID: %2 | %3)")
                        .arg((qint64)ct, 8, 16, QChar('0'))
                        .arg(ct->GetID(), 4, 10, QChar('0'))
                        .arg(ct->GetFullName());

  if (grp)
    grp->SL_DelContact(pct);
}




QVector<QSharedPointer<CTGroup*>> CTSManager::GetGroups() const
{
  QVector<QSharedPointer<CTGroup*>> groups;
  QVector<CTGroup*>::const_iterator it = _groups.constBegin(), ite = _groups.constEnd();

  for (; it != ite; it++)
    groups.append(QSharedPointer<CTGroup*>::create( (*it) ));

  return groups;
}


QSharedPointer<CTGroup*> CTSManager::GetDefaultGroup() const
{
  if (_groups.size() > 0) {
      QVector<CTGroup*>::const_iterator it = _groups.constBegin(), ite = _groups.constEnd();

      for (; it != ite; it++) {
          if ((*it)->GetName().compare("Default", Qt::CaseSensitive) == 0)
            return QSharedPointer<CTGroup*>::create((*it));
        }
    }

  return QSharedPointer<CTGroup*>(nullptr);
}


QSharedPointer<CTGroup*> CTSManager::GetGroupByName(const QByteArray &name) const
{
  if (!name.isEmpty()) {
      QVector<CTGroup*>::const_iterator it = _groups.constBegin(), ite = _groups.constEnd();

      for (; it != ite; it++) {
          if ((*it)->GetName().compare(name, Qt::CaseSensitive) == 0)
            return QSharedPointer<CTGroup*>::create((*it));
        }
    }

  return QSharedPointer<CTGroup*>(nullptr);
}


QSharedPointer<CTGroup*> CTSManager::GetGroupByID(const quint32 &id) const
{
  if (id > 0) {
      QVector<CTGroup*>::const_iterator it = _groups.constBegin(), ite = _groups.constEnd();

      for (; it != ite; it++) {
          if ((*it)->GetID() == id)
            return QSharedPointer<CTGroup*>::create((*it));
        }
    }

  return QSharedPointer<CTGroup*>::create(nullptr);
}
