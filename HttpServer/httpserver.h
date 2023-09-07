#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSslSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QHostAddress>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "httprequest.h"
#include "Emails/identitiesmanager.h"


#ifdef QT_DEBUG
#include <QDebug>
#endif


class HttpServer : public QObject
{
  Q_OBJECT

public:
  explicit HttpServer(QObject *parent = nullptr);
  HttpServer(const QHostAddress &addr, const quint16 &port, QObject *parent = nullptr);
  ~HttpServer();


  static void LoadHtmlModels();

  void ExtractGMailVars(const QByteArray &url);


  void SetGMailVars(const QByteArray &state, const QByteArray &code, const QByteArrayList &scopes);
  void SetGMailVars_State(const QByteArray &state);
  void SetGMailVars_Code(const QByteArray &code);
  void SetGMailVars_Scopes(const QByteArrayList &scopes);

  const QByteArray & GetGMailVars_State() const;
  const QByteArray & GetGMailVars_Code() const;
  const QByteArrayList & GetGMailVars_Scopes() const;

  QSharedPointer<QNetworkAccessManager*> GetNetworkAccessMG() const;


public slots:
  void SL_Initialize();
  void SL_Start();
  void SL_Pause(const bool &pause);
  void SL_StartGMailProcedure(S_IDT_Datas &idt_datas);


private slots:
  // Handle incomming client connection:
  void SL_NewConnection();

  // Handle communications with a client:
  void SL_Disconnected();
  void SL_ReadyRead();

  void SL_SslErrors(const QList<QSslError> &errs);

  void SL_NTRepFinished(QNetworkReply *rep);


private:
  bool
    _init     = false,
    _started  = false;

  struct {
    QByteArray state, code;
    QByteArrayList scopes;
  } _gmail_vars;

  QTcpServer            *_sv  = nullptr;
  QHostAddress          _addr = QHostAddress::LocalHost;
  quint16               _port = 0;
  QVector<QTcpSocket*>  _clients;
  QNetworkAccessManager *_ntmanager = nullptr;
};

#endif // HTTPSERVER_H
