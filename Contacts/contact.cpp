#include "contact.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


/* -------- Constructors -------- */
// Default:
Contact::Contact(QObject *parent)
  : QObject{parent}
{
}

// With datas:
Contact::Contact(const QByteArray &firstName, const QByteArray &lastName, const QByteArray &email,
                 const quint32 &id, QObject *parent)
  : QObject{parent},
    _id(id), _firstName(firstName), _lastName(lastName), _email(email)
{
}

// Copy:
Contact::Contact(const Contact &other)
  : QObject{other.parent()}
{
  *this = other;
}


/* -------- Destructor -------- */
Contact::~Contact()
{
  qDebug().noquote() << QString("Contact::~Contact | destroying contact : 0x%1")
                        .arg((qint64)this, 8, 16, QChar('0'));
}



bool Contact::operator == (const Contact &other)
{
  QByteArray
      fullName = this->GetFullName(), oFullName = other.GetFullName(),
      email = this->GetEmail(), oEmail = other.GetEmail();

  if ( ((!fullName.isEmpty() && !oFullName.isEmpty()) && fullName.compare(oFullName, Qt::CaseSensitive) == 0) ||
       ((!email.isEmpty() && !oEmail.isEmpty()) && email.compare(oEmail, Qt::CaseSensitive) == 0) )
    return true;

  return false;
}


bool Contact::operator != (const Contact &other)
{
  return !(*this == other);
}


Contact * Contact::operator = (const Contact &other)
{
  this->SetFirstName(other.GetFirstName());
  this->SetLastName(other.GetLastName());
  this->SetEmail(other.GetEmail());

  return this;
}



/* -------- Mutators -------- */
void Contact::SetID(const quint32 &id) { _id = id; }

void Contact::SetFirstName(const QByteArray &name)
{
  QByteArray old_name(_firstName);

  if (name.compare(old_name, Qt::CaseSensitive) != 0) {
      _firstName = name;
      emit DatasChanged();
    }
}


void Contact::SetLastName(const QByteArray &name)
{
  QByteArray old_name(_lastName);

  if (name.compare(old_name, Qt::CaseSensitive) != 0) {
      _lastName = name;
      emit DatasChanged();
    }
}


void Contact::SetEmail(const QByteArray &email)
{
  QByteArray old_email(_email);

  if (email.compare(old_email, Qt::CaseSensitive) != 0) {
      _email = email;
      emit DatasChanged();
    }
}



/* -------- Accessors -------- */
const quint32 & Contact::GetID() const { return _id; }
const QByteArray & Contact::GetFirstName() const { return _firstName; }
const QByteArray & Contact::GetLastName() const { return _lastName; }
const QByteArray & Contact::GetEmail() const { return _email; }
const QByteArray Contact::GetFullName() const
{
  return QString("%1 %2").arg(this->GetFirstName(), this->GetLastName()).toUtf8();
}
