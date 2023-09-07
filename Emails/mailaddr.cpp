#include "mailaddr.h"

MailAddr::MailAddr(QObject *parent)
  : QObject{parent}
{
}


MailAddr::MailAddr(const QByteArray &addr, const QByteArray &pwd, QObject *parent)
  : QObject{parent},
    _addr(addr), _pwd(pwd)
{

}


MailAddr::MailAddr(const MailAddr &other)
  : QObject{other.parent()}
{
  *this = other;
}


MailAddr::~MailAddr()
{

}



MailAddr * MailAddr::operator = (const MailAddr &other)
{
  this->SetAddress(other.GetAddress());
  this->SetPasswd(other.GetPasswd());

  return this;
}



void MailAddr::SetAddress(const QByteArray &addr) { _addr = addr; }
void MailAddr::SetPasswd(const QByteArray &pwd) { _pwd = pwd; }


const QByteArray & MailAddr::GetAddress() const { return _addr; }
const QByteArray & MailAddr::GetPasswd() const { return _pwd; }
