#ifndef LOGOBJECT_H
#define LOGOBJECT_H

#include <QObject>
#include <QDateTime>


enum class LOG_TYPES {
  NONE  = 0x0000,
  INFO  = 0x0001,
  WARN  = 0x0002,
  ERR   = 0x0003,
  DBG   = 0x0004
};



class LogObject : public QObject
{
  Q_OBJECT

public:
  explicit LogObject(QObject *parent = nullptr);
  LogObject(const LOG_TYPES &type, const QByteArray &text,
            const QDateTime &dt = QDateTime::currentDateTime(), QObject *parent = nullptr);
  LogObject(const LogObject &other);
  ~LogObject();


  LogObject * operator = (const LogObject &other);


  void SetType(const LOG_TYPES &type);
  void SetDT(const QDateTime &dt);
  void SetText(const QByteArray &text);


  const LOG_TYPES & GetType() const;
  const QDateTime & GetDT() const;
  const QByteArray & GetText() const;


private:
  LOG_TYPES   _type = LOG_TYPES::NONE;
  QDateTime   _dt = QDateTime::currentDateTime();
  QByteArray  _text;
};

#endif // LOGOBJECT_H
