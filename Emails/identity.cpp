#include "identity.h"


Identity::Identity(QObject *parent)
  : QObject(parent)
{
}


Identity::Identity(const QByteArray &name, const QByteArray &addr, QObject *parent)
  : QObject{parent},
    _name(name), _addr(addr)
{
}


Identity::Identity(const Identity &other)
  : QObject(other.parent())
{
  *this = other;
}


Identity::~Identity()
{
}



// Operators:
bool Identity::operator == (const Identity &other)
{
  QByteArray name = this->GetName(), oName = other.GetName();

  if ( (!name.isEmpty() && !oName.isEmpty()) && name.compare(oName, Qt::CaseSensitive) == 0)
    return true;

  return false;
}


bool Identity::operator != (const Identity &other)
{
  return !(*this == other);
}


Identity * Identity::operator = (const Identity &other)
{
  this->SetName(other.GetName());
  this->SetAddress(other.GetAddress());
  this->SetCode(other.GetCode());

  this->SetTokens(other.GetToken_Access(),
                  other.GetToken_Refresh(),
                  other.GetToken_Type(),
                  other.GetToken_ExpiresIn());

  return this;
}



QTextStream & operator << (QTextStream &stream, const Identity &/*idt*/)
{
  return stream;
}


QDataStream & operator << (QDataStream &stream, const Identity &/*idt*/)
{
  return stream;
}


QDataStream & operator >> (QDataStream &stream, Identity &/*idt*/)
{
  return stream;
}



void Identity::SetID(const quint32 &id) { _id = id; }
void Identity::SetName(const QByteArray &name) { _name = name; }
void Identity::SetAddress(const QByteArray &addr) { _addr = addr; }
void Identity::SetCode(const QByteArray &code) { _code = code; }

void Identity::SetTokens(const QByteArray &tkn_access, const QByteArray &tkn_refresh, const QByteArray &tkn_type,
                         const qint32 &tkn_expires_in)
{
  _tokens = {
    tkn_access,
    tkn_refresh,
    tkn_type,
    tkn_expires_in,
    QDateTime::currentDateTime().addSecs(tkn_expires_in)
  };
}

void Identity::SetToken_Access(const QByteArray &token) { _tokens.access = token; }
void Identity::SetToken_Refresh(const QByteArray &token) { _tokens.refresh = token; }
void Identity::SetToken_Type(const QByteArray &type) { _tokens.type = type; }
void Identity::SetToken_ExpiresIn(const qint32 &secs)
{
  _tokens.expires_in = secs;
  _tokens.expires_on = QDateTime::currentDateTime().addSecs(secs);
}


const quint32 & Identity::GetID() const { return _id; }
const QByteArray & Identity::GetName() const { return _name; }
const QByteArray & Identity::GetAddress() const { return _addr; }
const QByteArray & Identity::GetCode() const { return _code; }

const QByteArray & Identity::GetToken_Access() const { return _tokens.access; }
const QByteArray & Identity::GetToken_Refresh() const { return _tokens.refresh; }
const QByteArray & Identity::GetToken_Type() const { return _tokens.type; }
const qint32 & Identity::GetToken_ExpiresIn() const { return _tokens.expires_in; }
const QDateTime & Identity::GetToken_ExpiresOn() const { return _tokens.expires_on; }
