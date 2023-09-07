#ifndef IDENTITY_H
#define IDENTITY_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QDateTime>

#include "Contacts/ctsmanagerie.h"


class Identity : public QObject
{
  Q_OBJECT

public:
  Identity(QObject *parent = nullptr);
  Identity(const QByteArray &name, const QByteArray &addr, QObject *parent = nullptr);
  Identity(const Identity &other);
  ~Identity();

  // Operators:
  bool operator == (const Identity &other);
  bool operator != (const Identity &other);
  Identity * operator = (const Identity &other);


  // Mutators:
  void SetID(const quint32 &id);
  void SetName(const QByteArray &name);
  void SetAddress(const QByteArray &addr);
  void SetCode(const QByteArray &code);

  void SetTokens(const QByteArray &tkn_access, const QByteArray &tkn_refresh,
                 const QByteArray &type, const qint32 &expires_in);
  void SetToken_Access(const QByteArray &token);
  void SetToken_Refresh(const QByteArray &token);
  void SetToken_Type(const QByteArray &type);
  void SetToken_ExpiresIn(const qint32 &secs);


  // Accessors:
  const quint32     & GetID() const;
  const QByteArray  & GetName() const;
  const QByteArray  & GetAddress() const;
  const QByteArray  & GetCode() const;

  const QByteArray  & GetToken_Access() const;
  const QByteArray  & GetToken_Refresh() const;
  const QByteArray  & GetToken_Type() const;
  const qint32      & GetToken_ExpiresIn() const;
  const QDateTime   & GetToken_ExpiresOn() const;


private:
  quint32 _id = 0;
  QByteArray  _name         = "",
              _addr         = "",
              _code         = "";

  struct {
    QByteArray  access, refresh, type;
    qint32      expires_in = 0;
    QDateTime   expires_on = QDateTime::currentDateTime();
  } _tokens;
};


QTextStream & operator << (QTextStream &stream, const Identity &idt);
QDataStream & operator << (QDataStream &stream, const Identity &idt);
QDataStream & operator >> (QDataStream &stream, Identity &idt);

#endif // IDENTITY_H
