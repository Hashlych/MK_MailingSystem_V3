#include "emailsmanager.h"



EmailsManager::S_ML_Timer::S_ML_Timer() { }
EmailsManager::S_ML_Timer::S_ML_Timer(const EmailsManager::S_ML_Timer &other)
{
  tm = other.tm;
  pml = other.pml;
}

EmailsManager::S_ML_Timer::~S_ML_Timer() { }


bool EmailsManager::S_ML_Timer::operator == (const EmailsManager::S_ML_Timer &other) const
{
  if (tm == other.tm)
    return true;

  return false;
}


bool EmailsManager::S_ML_Timer::operator != (const EmailsManager::S_ML_Timer &other)
{
  return !(*this == other);
}





/* ---------------- Constructors ---------------- */
EmailsManager::EmailsManager(QObject *parent)
  : QObject{parent}
{
}

/* ---------------- Destructor ---------------- */
EmailsManager::~EmailsManager()
{
  if (_tm_check && _tm_check->isActive())
    emit _tm_check->stop();
}



void EmailsManager::SL_Initialize()
{
  if (!_init) {
      _tm_check = new QTimer(this);
      _tm_check->setInterval(1000);

      connect(_tm_check, &QTimer::timeout, this, &EmailsManager::OnTM_Check);

      _init = true;


      _tm_check->start();
    }
}



void EmailsManager::OnTM_Check()
{
  //qDebug().noquote() << QString("EmailsManager::OnTM_Check");


  if (_emails.isEmpty() || _mls_timers.size() >= _max_timers) {

      //qDebug().noquote() << QString("EmailsManager::OnTM_Check | emails.isEmpty : %1")
                            //.arg(_emails.isEmpty());
      //qDebug().noquote() << QString("EmailsManager::OnTM_Check | mls_timers.size = %1 ( >= %2 )")
                            //.arg(_mls_timers.size())
                            //.arg(_max_timers);

      return;
    }


  QVector<EmailObject*>::const_iterator it = _emails.constBegin(), ite = _emails.constEnd();
  EmailObject *ml = nullptr;
  quint64 lm_exec = (quint64)(QDateTime::currentDateTime().addDays(1).toMSecsSinceEpoch());

  for (; it != ite; it++) {
      if ( !(*it)->IsProcessing() && (*it)->GetDT_Exec() <= lm_exec ) {
          ml = (*it);

          // check if already in queue:
          QVector<QSharedPointer<EmailObject*>>::const_iterator it = _emails_queue.constBegin(),
              ite = _emails_queue.constEnd();

          for (; it != ite; it++) {
              if ( *(*it).data() == ml) {
                  ml = nullptr;
                  break;
                }
            }
          // end check:

          break;
        }

      ml = nullptr;
    }


  if (!ml) {
      //qDebug().noquote() << QString("EmailsManager::OnTM_Check | ml = nullptr");
      return;
    }


  QSharedPointer<EmailObject*> pml = QSharedPointer<EmailObject*>::create(ml);
  S_ML_Timer tml;

  tml.tm = new QTimer(this);
  tml.pml = pml;

  _emails_queue.append(pml);
  ml->SetProcessing(true);


  connect(tml.tm, &QTimer::timeout, this, &EmailsManager::OnTM_ML);

  if (ml->GetDT_Exec() >= (quint64)QDateTime::currentDateTime().addDays(1).toMSecsSinceEpoch())
    return;


  quint64 msecs = 0;

  if (ml->GetDT_Exec() > (quint64)QDateTime::currentMSecsSinceEpoch())
    msecs = ml->GetDT_Exec() - (quint64)QDateTime::currentMSecsSinceEpoch();

  qDebug().noquote() << QString("EmailsManager::OnTM_Check | msecs = %1").arg(msecs);

  tml.tm->setInterval(msecs);
  tml.tm->setSingleShot(true);
  tml.tm->start();


  _mls_timers.append(tml);
}



void EmailsManager::OnTM_ML()
{
  QTimer *tm = qobject_cast<QTimer*>(sender());

  if (!tm)
    return;

  QVector<S_ML_Timer>::const_iterator it = _mls_timers.constBegin(), ite = _mls_timers.constEnd();
  S_ML_Timer tml;

  for (; it != ite; it++) {
      if ((*it).tm == tm) {
          tml = (*it);
          break;
        }
    }

  if (!tml.pml.isNull()) {
      qDebug().noquote() << QString("EmailsManager::OnTM_ML | emit signal : ReqNT_SendEmail(tm: 0x%1 | ml: 0x%2)")
                            .arg((qint64)tml.tm, 8, 16, QChar('0'))
                            .arg((qint64)(*tml.pml.data()), 8, 16, QChar('0'));

      emit SI_ReqNT_SendEmail(tml.pml);
      _mls_timers.removeOne(tml);

      /*for (int n = 0; n < _mls_timers.size(); n++) {
          if (_mls_timers.at(n) == tml) {
              _mls_timers.removeAt(n);
              break;
            }
        }*/

      delete tml.tm;
      tml.tm = nullptr;
    }
}



/* ---------------- Slots for emails management ---------------- */
// SL_AddEmail (with datas):
void EmailsManager::SL_AddEmail(const QByteArray &subject, const QByteArray &content, const quint64 &dt_exec,
                                const quint32 &id)
{
  this->SL_AddEmail( new EmailObject(subject, content, dt_exec, id, this) );
}


// SL_AddEmail (with EmailObject):
void EmailsManager::SL_AddEmail(EmailObject *ml)
{
  if (ml) {

      QVector<EmailObject*>::const_iterator it = _emails.constBegin(), ite = _emails.constEnd();

      for (; it != ite; it++) {
          if ((*it) == ml)
            return;
        }

      if (!ml->parent() || ml->parent() != this)
        ml->setParent(this);

      _emails.append(ml);
      emit SI_EmailAdded(QSharedPointer<EmailObject*>::create(ml));
    }
}


// SL_DelEmail:
void EmailsManager::SL_DelEmail(QSharedPointer<EmailObject*> pml)
{
  if (!pml.isNull()) {
      EmailObject *ml = *pml.data();

      if (_emails.removeOne(ml))
        emit SI_EmailDeleted(pml);
    }
}




/* ---------------- Accessors ---------------- */
QVector<QSharedPointer<EmailObject*>> EmailsManager::GetEmailsList() const
{
  QVector<QSharedPointer<EmailObject*>> emails;

  for (int n = 0; n < _emails.size(); n++) {
      emails.append(QSharedPointer<EmailObject*>::create(_emails.at(n)));
    }

  return emails;
}


QVector<QSharedPointer<EmailObject*>> EmailsManager::GetEmailsForProcessing() const
{
  QVector<QSharedPointer<EmailObject*>> emails;

  for (int n = 0; n < _emails.size(); n++) {
      QDateTime dt_a = QDateTime::fromMSecsSinceEpoch(_emails.at(n)->GetDT_Exec()),
                dt_b = dt_a.addDays(1);

      if ( dt_a.toMSecsSinceEpoch() <= dt_b.toMSecsSinceEpoch() ||
           dt_a.toMSecsSinceEpoch() <= QDateTime::currentMSecsSinceEpoch() )
        emails.append(QSharedPointer<EmailObject*>::create(_emails.at(n)));
    }

  return emails;
}
