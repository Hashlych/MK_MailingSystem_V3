#ifndef IDENTITIESMANAGER_H
#define IDENTITIESMANAGER_H

#include <QObject>

#include "identity.h"


class IdentitiesManager : public QObject
{
  Q_OBJECT

public:
  explicit IdentitiesManager(QObject *parent = nullptr);
  ~IdentitiesManager();


  QSharedPointer<Identity*> GetIdentityByID(const quint32 &id);


  void SetIdentities(const QVector<Identity*> &identities);

  const QVector<Identity*> & GetIdentities() const;


signals:
  void SI_IdentityAdded(QSharedPointer<Identity*> &idt);
  void SI_IdentityDeleted(QSharedPointer<Identity*> &idt);


public slots:
  void SL_AddIdentity(const S_IDT_Datas &datas);
  void SL_AddIdentity(Identity *idt);

  void SL_DelIdentity(QSharedPointer<Identity*> &idt);


private:
  QVector<Identity*> _identities;
};

#endif // IDENTITIESMANAGER_H
