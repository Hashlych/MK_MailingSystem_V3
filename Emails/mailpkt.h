#ifndef MAILPKT_H
#define MAILPKT_H

#include <QObject>
#include <QDateTime>

#include "mailaddr.h"


class MailPKT : public QObject
{
  Q_OBJECT

public:
  explicit MailPKT(QObject *parent = nullptr);
  MailPKT(const MailAddr &creds, const MailAddr &from, const QVector<MailAddr> &to,
          const QByteArray &subject, const QByteArray &content, const QDateTime &dt_exec,
          const quint32 &part_id = 0, const quint32 &parts_total = 0,
          QObject *parent = nullptr);
  ~MailPKT();


  void SetCreds(const MailAddr &creds);
  void SetFrom(const MailAddr &from);
  void SetToRcpts(const QVector<MailAddr> &to);
  void SetDT_Exec(const QDateTime &dt_exec);
  void SetSubject(const QByteArray &subject);
  void SetContent(const QByteArray &content);
  void SetPartID(const quint32 &id);
  void SetPartsTotal(const quint32 &total);


  const MailAddr & GetCreds() const;
  const MailAddr & GetFrom() const;
  const QVector<MailAddr> & GetToRcpts() const;
  const QDateTime & GetDT_Exec() const;
  const QByteArray & GetSubject() const;
  const QByteArray & GetContent() const;
  quint64 GetPartID() const;
  quint64 GetPartsTotal() const;


private:
  QByteArray _subject, _content;
  MailAddr _creds, _from;
  QVector<MailAddr> _to;
  QDateTime _dt_exec;
  quint32 _part_id = 0, _parts_total = 0;
};

#endif // MAILPKT_H
