#include "emailobject.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


EmailObject::EmailObject(QObject *parent)
  : QObject{parent}
{
}


EmailObject::EmailObject(const QByteArray &subject, const QByteArray &content, const quint64 &dt_exec,
                         const quint32 &id, QObject *parent)
  : QObject{parent},
    _id(id), _subject(subject), _content(content), _dt_exec(dt_exec)
{
}


EmailObject::EmailObject(const EmailObject &other)
  : QObject(other.parent())
{
  *this = other;
}


EmailObject::~EmailObject()
{
  qDebug().noquote() << QString("EmailObject::~EmailObject | destroying email object : 0x%1")
                        .arg((qint64)this, 8, 16, QChar('0'));
}



void EmailObject::ShowDebugInfos()
{
  QString parent_name;
  if (this->parent())
    parent_name = this->parent()->objectName();

  qDebug().noquote() << QString("EmailObject::ShowDebugInfos | addr = 0x%1")
                        .arg((qint64)this, 8, 16, QChar('0'));
  qDebug().noquote() << QString("EmailObject::ShowDebugInfos | parent.addr = 0x%1 (%2)")
                        .arg((qint64)this->parent(), 8, 16, QChar('0'))
                        .arg(parent_name);
  qDebug().noquote() << QString("EmailObject::ShowDebugInfos | ID = %1")
                        .arg(this->GetID(), 3, 10, QChar('0'));
  qDebug().noquote() << QString("EmailObject::ShowDebugInfos | subject = %1")
                        .arg(this->GetSubject());
  qDebug().noquote() << QString("EmailObject::ShowDebugInfos | dt exec = %1 (%2)")
                        .arg(QDateTime::fromMSecsSinceEpoch(this->GetDT_Exec()).toString("yyyy/MM/dd hh:mm:ss.zzz"))
                        .arg(this->GetDT_Exec(), 8, 10, QChar('0'));
  qDebug().noquote() << QString("EmailObject::ShowDebugInfos | contacts.size = %1\n")
                        .arg(this->GetContacts().size(), 4, 10, QChar('0'));
}



bool EmailObject::operator == (const EmailObject &/*other*/)
{
  return false;
}


bool EmailObject::operator != (const EmailObject &other)
{
  return !(*this == other);
}


EmailObject * EmailObject::operator = (const EmailObject &other)
{
  this->SetSubject(other.GetSubject());
  this->SetContent(other.GetContent());
  this->SetContacts(other.GetContacts());

  return this;
}



void EmailObject::AddContact(QSharedPointer<Contact *> pct)
{
  _contacts.append(pct);
}


void EmailObject::AddContacts(QVector<QSharedPointer<Contact *>> cts)
{
  _contacts.append(cts);
}


void EmailObject::AddGroup(QSharedPointer<CTGroup *> pgrp)
{
  _groups.append(pgrp);
}


void EmailObject::AddGroups(QVector<QSharedPointer<CTGroup *>> grps)
{
  _groups.append(grps);
}




void EmailObject::SetID(const quint32 &id) { _id = id; }
void EmailObject::SetSubject(const QByteArray &subject) { _subject = subject; }
void EmailObject::SetContent(const QByteArray &content) { _content = content; }
void EmailObject::SetDT_Exec(const quint64 &dt) { _dt_exec = dt; }
void EmailObject::SetProcessing(const bool &processing) { _processing = processing; }
void EmailObject::SetSent(const bool &sent) { _sent = sent; }
void EmailObject::SetContacts(QVector<QSharedPointer<Contact *>> cts) { _contacts = cts; }
void EmailObject::SetGroups(QVector<QSharedPointer<CTGroup *>> grps) { _groups = grps; }



const quint32 & EmailObject::GetID() const { return _id; }
const QByteArray & EmailObject::GetSubject() const { return _subject; }
const QByteArray & EmailObject::GetContent() const { return _content; }
const quint64 & EmailObject::GetDT_Exec() const { return _dt_exec; }
const bool & EmailObject::IsProcessing() const { return _processing; }
const bool & EmailObject::HasBeenSent() const { return _sent; }
const QVector<QSharedPointer<Contact*>> & EmailObject::GetContacts() const { return _contacts; }
const QVector<QSharedPointer<CTGroup*>> & EmailObject::GetGroups() const { return _groups; }
