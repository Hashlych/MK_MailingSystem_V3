#include "mlsprocessor.h"

MLSProcessor::MLSProcessor(QSharedPointer<EmailsManager*> _p_mls_mg, QObject *parent)
  : QObject{parent},
    _p_mls_mg(_p_mls_mg)
{
}

MLSProcessor::~MLSProcessor()
{
  if (_tm_check && _tm_check->isActive())
    emit _tm_check->stop();
}



void MLSProcessor::SL_Initialize()
{
  if (!_init) {
      _tm_check = new QTimer(this);

      //_tm_check->setInterval(300000);
      _tm_check->setInterval(300000);

      connect(_tm_check, &QTimer::timeout, this, &MLSProcessor::OnTM_Check);


      _init = true;


      //_tm_check->start();
    }
}



void MLSProcessor::SL_AddToWatchList(QSharedPointer<EmailObject*> pml)
{
  if (pml.isNull())
    return;

  if (this->EmailObjectExists(pml) != 0)
    return;

  QTimer *tm = new QTimer(*pml.data());
  qint64 msecs = (*pml.data())->GetDT_Exec() - QDateTime::currentMSecsSinceEpoch();

  if (msecs < 0)
    msecs = 10;


  connect(tm, &QTimer::timeout, this, &MLSProcessor::OnTM_Timeout);

  _watch_list.append( S_ML{tm, pml} );

  tm->setSingleShot(true);
  tm->start(msecs);
}



qint8 MLSProcessor::EmailObjectExists(QSharedPointer<EmailObject *> pml)
{
  if (pml.isNull())
    return -1;

  QVector<S_ML>::const_iterator it = _watch_list.constBegin(), ite = _watch_list.constEnd();

  for (; it != ite; it++) {
      if ( *(*it).pml.data() == *pml.data() )
        return 1;
    }

  return 0;
}



void MLSProcessor::OnTM_Timeout()
{
  QTimer *tm = qobject_cast<QTimer*>(sender());

  if (!tm)
    return;


  for (int n = 0; n < _watch_list.size(); n++) {
      if (_watch_list.at(n).tm == tm) {
          EmailObject *ml = (*_watch_list.at(n).pml.data());

          _watch_list.removeAt(n);

          emit SI_ReqNT_SendEmail(QSharedPointer<EmailObject*>::create(ml));

          break;
        }
    }

  /*QVector<S_ML>::const_iterator it = _watch_list.constBegin(), ite = _watch_list.constEnd();

  for (; it != ite; it++) {
      if ((*it).tm == tm) {
          EmailObject *ml = *((*it).pml.data());

          _watch_list.removeOne((*it));

          emit SI_ReqNT_SendEmail(QSharedPointer<EmailObject*>::create(ml));

          break;
        }
    }*/
}



void MLSProcessor::OnTM_Check()
{
  if (_p_mls_mg.isNull())
    return;

  QVector<QSharedPointer<EmailObject*>> mls = (*_p_mls_mg.data())->GetEmailsForProcessing();

  while (!mls.isEmpty())
    this->SL_AddToWatchList(mls.takeFirst());
}
