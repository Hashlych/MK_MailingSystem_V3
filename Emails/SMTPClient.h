#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QAbstractSocket>
#include <QTimer>
#include <QFile>
#include <QSslKey>
#include <QSslConfiguration>
#include <QMutex>

#include "QtAES/qaesencryption.h"
#include "identity.h"
#include "mailpkt.h"
#include "emailobject.h"


class SMTPClient : public QObject
{
  Q_OBJECT

public:
  SMTPClient(const QString &host, const quint16 &port = 465, const quint32 &conTimeout = 30000,
             const quint32 &timeout = 60000, QObject *parent = nullptr);
  ~SMTPClient();

  const QByteArray & GetCLKey() const;


signals:
  void SI_MailSent(QSharedPointer<MailPKT*> ml_pkt);


public slots:
  void Init(QAESEncryption *encryption);
  void StartSendingProcess();
  void SL_AddEmailObject(QSharedPointer<EmailObject*> pml);


private slots:
  void SL_ReadyRead();
  void SL_Abort();
  void SL_ProcessQueue();


private:
  enum class states {
    None, TLS, HandShake, Auth, User, Pass, Rcpt, Mail, Data, Init, Body, Quit, Close
  };


  // Functions:
  void InitSslConfig();
  void LoadCertsDatas();
  QVector<QSharedPointer<MailPKT*>> SplitEmailObj_ToPackets(QSharedPointer<EmailObject*> pml);
  void SendMail(MailPKT *mlpkt);


  // Properties:
  bool _init = false;
  bool _busy = false;

  QString _host;
  quint16 _port = 0;
  quint32 _conTimeout = 0, _timeout = 0;
  QTimer  *_processTimer = nullptr;

  QVector<MailPKT*> _mls_packets;
  MailPKT *_curr_ml_packet = nullptr;

  QTextStream       *_stream = nullptr;
  QSslKey           _clKey;
  QSslConfiguration _sslCfg;
  QSslSocket        *_socket = nullptr;
  QAESEncryption    *_encryption = nullptr;

  struct {
    QByteArray clKey, clCert, ca;
  } _certsDatas;

  states  _state = states::Init;
  QString _response;
};

#endif // SMTPCLIENT_H
