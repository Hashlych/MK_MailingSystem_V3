#include "logsmanager.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


LogsManager::LogsManager(QObject *parent)
  : QObject{parent}
{
}



void LogsManager::SL_Initialize()
{
  if (!_init) {

      _tm_sendToDisplay = new QTimer(this);
      _tm_sendToDisplay->setInterval(5);

      connect(_tm_sendToDisplay, &QTimer::timeout, this, &LogsManager::OnTM_SendToDisplay);

      _init = true;

      qDebug().noquote() << QString("LogsManager::SL_Initialize | init = true");

      _tm_sendToDisplay->start();
    }
}



void LogsManager::SL_AddLog(const LOG_TYPES &type, const QByteArray &text, const QDateTime &dt)
{
  if (!_mtx.tryLock(5))
    return;

  LogObject *log = new LogObject(type, text, dt, this);

  if (log)
    _logs.append(log);

  _mtx.unlock();
}




void LogsManager::OnTM_SendToDisplay()
{
  if (!_mtx.tryLock())
    return;

  if (_logs.isEmpty()) {
      _mtx.unlock();
      return;
    }

  int nmax = 5;

  if (nmax > _logs.size())
    nmax = _logs.size();

  QVector<QSharedPointer<LogObject*>> logs;

  for (int n = 0; n < nmax; n++)
    logs.append( QSharedPointer<LogObject*>::create(_logs.takeFirst()) );

  if (!logs.isEmpty())
    emit SI_ReqDisplayLogs(logs);

  _mtx.unlock();
}
