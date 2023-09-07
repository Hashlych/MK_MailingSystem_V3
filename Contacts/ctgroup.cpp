#include "ctgroup.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


CTGroup::CTGroup(QObject *parent)
  : QObject{parent}
{
}


CTGroup::CTGroup(const QByteArray &name, const QByteArray &desc, const quint32 &id, QObject *parent)
  : QObject{parent},
    _id(id), _name(name), _desc(desc)
{
  qDebug().noquote() << QString("CTGroup::CTGroup | group created : 0x%1 | id: %2 | name: %3")
                        .arg((qint64)this, 8, 16, QChar('0'))
                        .arg(this->GetID(), 3, 10, QChar('0'))
                        .arg(this->GetName());
}


CTGroup::CTGroup(const CTGroup &other)
  : QObject{other.parent()}
{
  *this = other;
}


CTGroup::~CTGroup()
{
  qDebug().noquote() << QString("CTGroup::~CTGroup | destroying group : 0x%1")
                        .arg((qint64)this, 8, 16, QChar('0'));
}



bool CTGroup::operator == (const CTGroup &other)
{
  QByteArray name = this->GetName(), oName = other.GetName();

  if ( (!name.isEmpty() && !oName.isEmpty()) && name.compare(oName, Qt::CaseSensitive) == 0)
    return true;

  return false;
}


bool CTGroup::operator != (const CTGroup &other)
{
  return !(*this == other);
}


CTGroup * CTGroup::operator = (const CTGroup &other)
{
  this->SetName(other.GetName());
  this->SetDescription(other.GetDescription());

  return this;
}




void CTGroup::SL_AddContact(const QByteArray &firstName, const QByteArray &lastName, const QByteArray &email,
                            const quint32 &id)
{
  qDebug().noquote() << QString("CTGroup::SL_AddContact | adding contact\n");

  QVector<Contact*>::const_iterator it = _contacts.constBegin(), ite = _contacts.constEnd();

  for (; it != ite; it++) {

      if ( ((*it)->GetFirstName().compare(firstName, Qt::CaseSensitive) == 0 &&
          (*it)->GetLastName().compare(lastName, Qt::CaseSensitive) == 0) ||
           ((*it)->GetEmail().compare(email, Qt::CaseSensitive) == 0) ) {

          qDebug().noquote() << QString("CTGroup::SL_AddContact | failed : contact already exists (contact: 0x%1)")
                                .arg((qint64)(*it), 8, 16, QChar('0'));
          qDebug().noquote() << QString("CTGroup::SL_AddContact | firstName = %1")
                                .arg(firstName);
          qDebug().noquote() << QString("CTGroup::SL_AddContact | lastName = %1")
                                .arg(lastName);
          qDebug().noquote() << QString("CTGroup::SL_AddContact | email = %1\n")
                                .arg(email);

          return;
        }

    }


  Contact *ct = new Contact(firstName, lastName, email, id, this);
  _contacts.append(ct);

  emit SI_ContactAdded(QSharedPointer<Contact*>::create(ct));

  /*qDebug().noquote() << QString("CTGroup::SL_AddContact | contact added : 0x%1\n")
                        .arg((qint64)ct, 8, 16, QChar('0'));*/
}


void CTGroup::SL_AddContact(Contact *ct)
{
  if (!ct)
    return;

  QVector<Contact*>::const_iterator it = _contacts.constBegin(), ite = _contacts.constEnd();

  for (; it != ite; it++) {
      if (*(*it) == *ct) {
          delete ct;
          ct = nullptr;
          return;
        }
    }

  if (!ct->parent() || ct->parent() != this)
    ct->setParent(this);

  _contacts.append(ct);

  emit SI_ContactAdded(QSharedPointer<Contact*>::create(ct));
}



void CTGroup::SL_DelContact(const QSharedPointer<Contact*> pct)
{
  if (!pct.isNull()) {
      Contact *ct = *pct.data();
      QVector<Contact*>::const_iterator it = _contacts.constBegin(), ite = _contacts.constEnd();

      for (; it != ite; it++) {
          if ((*it) == ct) {
              _contacts.takeAt( _contacts.indexOf((*it)) );
              emit SI_ContactDeleted(pct);
              break;
            }
        }
    }
}




void CTGroup::SetID(const quint32 &id) { _id = id; }

void CTGroup::SetName(const QByteArray &name)
{
  QByteArray old_name(_name);

  if (name.compare(old_name, Qt::CaseSensitive) != 0) {
      _name = name;
      emit DatasChanged();
    }
}

void CTGroup::SetDescription(const QByteArray &desc)
{
  QByteArray old_desc(_desc);

  if (desc.compare(old_desc, Qt::CaseSensitive) != 0) {
      _desc = desc;
      emit DatasChanged();
    }
}



const quint32 & CTGroup::GetID() const { return _id; }
const QByteArray & CTGroup::GetName() const { return _name; }
const QByteArray & CTGroup::GetDescription() const { return _desc; }

QVector<QSharedPointer<Contact*>> CTGroup::GetContactsList() const
{
  QVector<QSharedPointer<Contact*>> cts;
  QVector<Contact*>::const_iterator it = _contacts.constBegin(), ite = _contacts.constEnd();

  for (; it != ite; it++) {
      cts.append(QSharedPointer<Contact*>::create((*it)));
    }

  return cts;
}
