#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QProcess>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSslSocket>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QUrl>

#include "Logs/logsmanager.h"
#include "httpresponse.h"
#include "httprequest.h"


class HttpServer : public QObject
{
  Q_OBJECT

public:
  explicit HttpServer(QObject *parent = nullptr);
  ~HttpServer();


signals:
  void SI_AddLog(const LOG_TYPES &type, const QByteArray &log, const QDateTime &dt = QDateTime::currentDateTime());


public slots:
  void SL_Initialize();
  void SL_Start();
  void SL_Stop();


private slots:
  void SL_NewConnection();
  void SL_ReadyRead();


private:
  bool  _init   = false,
        _paused = false;

  QTcpServer *_server = nullptr;
  QTcpSocket *_client = nullptr;

  QByteArray  _state  = "",
              _code   = "",
              _scope  = "";

  struct {
    QHostAddress  addr = QHostAddress::LocalHost;
    quint16       port = 45062;
  } _sv_params;
};

#endif // HTTPSERVER_H
