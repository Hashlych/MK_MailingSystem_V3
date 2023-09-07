#ifndef LOGSMANAGER_H
#define LOGSMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMutex>

#include "logobject.h"


class LogsManager : public QObject
{
  Q_OBJECT

public:
  explicit LogsManager(QObject *parent = nullptr);


signals:
  void SI_ReqDisplayLogs(QVector<QSharedPointer<LogObject*>> logs);


public slots:
  void SL_Initialize();
  void SL_AddLog(const LOG_TYPES &type, const QByteArray &text,
                 const QDateTime &dt = QDateTime::currentDateTime());


private slots:
  void OnTM_SendToDisplay();


private:
  bool _init = false;
  QMutex _mtx;
  QTimer *_tm_sendToDisplay = nullptr;
  QVector<LogObject*> _logs;
};

#endif // LOGSMANAGER_H
