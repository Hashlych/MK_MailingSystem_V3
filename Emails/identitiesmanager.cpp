#include "identitiesmanager.h"


IdentitiesManager::IdentitiesManager(QObject *parent)
  : QObject{parent}
{
}


IdentitiesManager::~IdentitiesManager()
{
}



QSharedPointer<Identity*> IdentitiesManager::GetIdentityByID(const quint32 &id)
{
  QVector<Identity*>::const_iterator it = _identities.constBegin(), ite = _identities.constEnd();

  for (; it != ite; it++) {
      if ((*it)->GetID() == id)
        return QSharedPointer<Identity*>::create((*it));
    }

  return QSharedPointer<Identity*>(nullptr);
}



void IdentitiesManager::SL_AddIdentity(const S_IDT_Datas &datas)
{
  Identity *idt = new Identity(datas.name, datas.addr, this);

  idt->SetCode(datas.code);
  idt->SetTokens(datas.tokens.access,
                 datas.tokens.access,
                 datas.tokens.type,
                 datas.tokens.expires_in);

  this->SL_AddIdentity(idt);
}



void IdentitiesManager::SL_AddIdentity(Identity *idt)
{
  _identities.append(idt);
  QSharedPointer<Identity*> p_idt = QSharedPointer<Identity*>::create(idt);

  emit SI_IdentityAdded(p_idt);
}



void IdentitiesManager::SL_DelIdentity(QSharedPointer<Identity*> &idt)
{
  if (idt.isNull())
    return;

  if (_identities.contains((*idt.data()))) {
      if (_identities.removeOne((*idt.data()))) {
          emit SI_IdentityDeleted(idt);
          return;
        }
    }


  QVector<Identity*>::const_iterator it = _identities.constBegin(), ite = _identities.constEnd();

  for (; it != ite; it++) {
      if ((*it) == (*idt.data())) {
          if (_identities.removeOne((*idt.data()))) {
              emit SI_IdentityDeleted(idt);
              return;
            }
        }
    }
}



void IdentitiesManager::SetIdentities(const QVector<Identity*> &identities) { _identities = identities; }


const QVector<Identity*> & IdentitiesManager::GetIdentities() const { return _identities; }
