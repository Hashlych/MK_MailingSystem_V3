#include "mailpkt.h"

MailPKT::MailPKT(QObject *parent)
  : QObject{parent}
{
}


MailPKT::MailPKT(const MailAddr &creds, const MailAddr &from, const QVector<MailAddr> &to,
                 const QByteArray &subject, const QByteArray &content, const QDateTime &dt_exec,
                 const quint32 &part_id, const quint32 &parts_total,
                 QObject *parent)
  : QObject{parent},
    _subject(subject), _content(content), _creds(creds), _from(from), _to(to), _dt_exec(dt_exec),
    _part_id(part_id), _parts_total(parts_total)
{
}


MailPKT::~MailPKT()
{
}




void MailPKT::SetCreds(const MailAddr &creds) { _creds = creds; }
void MailPKT::SetFrom(const MailAddr &from) { _from = from; }
void MailPKT::SetToRcpts(const QVector<MailAddr> &to) { _to = to; }
void MailPKT::SetDT_Exec(const QDateTime &dt_exec) { _dt_exec = dt_exec; }
void MailPKT::SetSubject(const QByteArray &subject) { _subject = subject; }
void MailPKT::SetContent(const QByteArray &content) { _content = content; }
void MailPKT::SetPartID(const quint32 &id) { _part_id = id; }
void MailPKT::SetPartsTotal(const quint32 &total) { _parts_total = total; }


const MailAddr & MailPKT::GetCreds() const { return _creds; }
const MailAddr & MailPKT::GetFrom() const { return _from; }
const QVector<MailAddr> & MailPKT::GetToRcpts() const { return _to; }
const QDateTime & MailPKT::GetDT_Exec() const { return _dt_exec; }
const QByteArray & MailPKT::GetSubject() const { return _subject; }
const QByteArray & MailPKT::GetContent() const { return _content; }
quint64 MailPKT::GetPartID() const { return _part_id; }
quint64 MailPKT::GetPartsTotal() const { return _parts_total; }
