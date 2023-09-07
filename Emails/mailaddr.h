#ifndef MAILADDR_H
#define MAILADDR_H

#include <QObject>


class MailAddr : public QObject
{
  Q_OBJECT

public:
  explicit MailAddr(QObject *parent = nullptr);
  MailAddr(const QByteArray &addr, const QByteArray &pwd, QObject *parent = nullptr);
  MailAddr(const MailAddr &other);
  ~MailAddr();

  MailAddr * operator = (const MailAddr &other);


  void SetAddress(const QByteArray &addr);
  void SetPasswd(const QByteArray &pwd);


  const QByteArray & GetAddress() const;
  const QByteArray & GetPasswd() const;


private:
  QByteArray _addr, _pwd;
};

#endif // MAILADDR_H
