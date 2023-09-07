#include "logobject.h"

LogObject::LogObject(QObject *parent)
  : QObject{parent}
{
}


LogObject::LogObject(const LOG_TYPES &type, const QByteArray &text, const QDateTime &dt, QObject *parent)
  : QObject{parent},
    _type(type), _dt(dt), _text(text)
{
}


LogObject::LogObject(const LogObject &other)
  : QObject{other.parent()}
{
  *this = other;
}


LogObject::~LogObject()
{
  qDebug().noquote() << QString("LogObject::~LogObject | destroying log : 0x%1")
                        .arg((qint64)this, 8, 16, QChar('0'));
}



LogObject * LogObject::operator = (const LogObject &other)
{
  this->SetType(other.GetType());
  this->SetDT(other.GetDT());
  this->SetText(other.GetText());

  return this;
}




void LogObject::SetType(const LOG_TYPES &type) { _type = type; }
void LogObject::SetDT(const QDateTime &dt) { _dt = dt; }
void LogObject::SetText(const QByteArray &text) { _text = text; }


const LOG_TYPES & LogObject::GetType() const { return _type; }
const QDateTime & LogObject::GetDT() const { return _dt; }
const QByteArray & LogObject::GetText() const { return _text; }
