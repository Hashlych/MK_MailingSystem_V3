#include "SMTPClient.h"


SMTPClient::SMTPClient(const QString &host, const quint16 &port, const quint32 &conTimeout, const quint32 &timeout,
                       QObject *parent)
  : QObject(parent),
    _host(host), _port(port),
    _conTimeout(conTimeout), _timeout(timeout)
{
  _socket = new QSslSocket(this);
}


SMTPClient::~SMTPClient()
{
  if (_processTimer && _processTimer->isActive())
    emit _processTimer->stop();

  if (_socket) {
      _socket->abort();
      _socket->deleteLater();
    }

  if (_stream) {
      delete _stream;
      _stream = nullptr;
    }
}



void SMTPClient::Init(QAESEncryption *encryption)
{
  if (_init)
    return;

  // AES Encryption:
  _encryption = encryption;

  // SSL Socket:
  connect(_socket, &QSslSocket::readyRead, this, &SMTPClient::SL_ReadyRead);

  // Init SSL configuration:
  this->InitSslConfig();

  // ProcessTimer:
  _processTimer = new QTimer(this);
  _processTimer->setInterval(15000);

  connect(_processTimer, &QTimer::timeout, this, &SMTPClient::SL_ProcessQueue);

  _init = true;

  qDebug().noquote() << QString("SMTPClient::Init | encryption = 0x%1")
                        .arg((qint64)_encryption, 8, 16, QChar('0'));
  qDebug().noquote() << QString("SMTPClient::Init | processTimer = 0x%1 | interval: %2 msecs")
                        .arg((qint64)_processTimer, 8, 16, QChar('0'))
                        .arg(_processTimer->interval());

  qDebug().noquote() << QString("SMTPClient::Init | init = true\n");

  QTimer::singleShot(10, this, &SMTPClient::StartSendingProcess);
}



void SMTPClient::InitSslConfig()
{
  _sslCfg = QSslConfiguration::defaultConfiguration();

  this->LoadCertsDatas();

  _clKey = QSslKey(_certsDatas.clKey, QSsl::Rsa, QSsl::Pem);
  _sslCfg.setPrivateKey(_clKey);

  _sslCfg.addCaCertificate(QSslCertificate(_certsDatas.ca));
  _sslCfg.setLocalCertificate(QSslCertificate(_certsDatas.clCert));

  _sslCfg.setPeerVerifyMode(QSslSocket::VerifyNone);

  _socket->setSslConfiguration(_sslCfg);

  qDebug().noquote() << QString("[SMTPClient][InitSslConfig]\n")
                     << QString("  SSL Lib build version : %1\n").arg(_socket->sslLibraryBuildVersionString())
                     << QString("  SSL Lib version : %1\n").arg(_socket->sslLibraryVersionString());
}



void SMTPClient::LoadCertsDatas()
{
  QFile clKeyFile(QStringLiteral(":/cert/cl-key.pem"));
  if (!clKeyFile.open(QIODevice::ReadOnly)) {
      qDebug() << "/cert/cl-key.pem : failed to open";
      return;
    }

  _certsDatas.clKey = clKeyFile.readAll();
  clKeyFile.close();


  QFile clCertFile(QStringLiteral(":/cert/cl-crt.pem"));
  if (!clCertFile.open(QIODevice::ReadOnly)) {
      qDebug() << "/cert/cl-cert.pem : failed to open";
      return;
    }

  _certsDatas.clCert = clCertFile.readAll();
  clCertFile.close();


  QFile caFile(QStringLiteral(":/cert/ca/ca.pem"));
  if (!caFile.open(QIODevice::ReadOnly)) {
      qDebug() << "/cert/ca/ca.pem : failed to open";
      return;
    }

  _certsDatas.ca = caFile.readAll();
  caFile.close();
}


void SMTPClient::StartSendingProcess()
{
  if (!_processTimer->isActive())
    _processTimer->start();
}



QVector<QSharedPointer<MailPKT*>> SMTPClient::SplitEmailObj_ToPackets(QSharedPointer<EmailObject*> pml)
{
  QVector<QSharedPointer<MailPKT*>> pkts;

  if (!pml.isNull()) {
      EmailObject *ml = *pml.data();
      QVector<QSharedPointer<CTGroup*>> grps = ml->GetGroups();
      QVector<QSharedPointer<Contact*>> cts;
      QVector<QSharedPointer<CTGroup*>>::const_iterator it = grps.constBegin(), ite = grps.constEnd();

      for (; it != ite; it++)
        cts.append((*(*it).data())->GetContactsList());

      if (cts.isEmpty())
        return pkts;

      MailPKT *pkt = new MailPKT(ml);

      pkt->SetSubject(ml->GetSubject());
      pkt->SetContent(ml->GetContent());
      pkt->SetDT_Exec(QDateTime::fromMSecsSinceEpoch(ml->GetDT_Exec()).addMSecs(500));
      pkt->SetFrom( MailAddr("hashlych@gmail.com", "", pkt) );
      pkt->SetCreds( MailAddr("hashlych@gmail.com", "", pkt) );

      if (cts.size() <= 16) {
          QVector<MailAddr> to_rcpts;

          for (int n = 0; n < cts.size(); n++)
            to_rcpts.append( MailAddr( (*(cts.at(n).data()))->GetEmail(), "", pkt) );

          pkt->SetToRcpts(to_rcpts);
          pkts.append(QSharedPointer<MailPKT*>::create(pkt));
        } else {
          int pkts_count = cts.size() / 16;

          if ( cts.size() % 16 > 0 )
            pkts_count++;

          for (int n = 0; n < pkts_count; n++) {
              MailPKT *pkt = new MailPKT(ml);

              pkt->SetSubject(ml->GetSubject());
              pkt->SetContent(ml->GetContent());
              pkt->SetFrom(MailAddr("hashlych@gmail.com", "", pkt));
              pkt->SetCreds(MailAddr("hashlych@gmail.com", "", pkt));
              pkt->SetDT_Exec(QDateTime::fromMSecsSinceEpoch(ml->GetDT_Exec()).addMSecs(500));
              pkt->SetPartID(n);
              pkt->SetPartsTotal(pkts_count);

              QVector<MailAddr> to_rcpts;
              int i_max = 16;

              if (i_max > cts.size())
                i_max = cts.size();

              for (int i = 0; i < i_max; i++)
                  to_rcpts.append( MailAddr( (*(cts.takeFirst().data()))->GetEmail(), "", pkt) );

              pkt->SetToRcpts(to_rcpts);

              pkts.append(QSharedPointer<MailPKT*>::create(pkt));
            } // end for packets count loop
        }
    }

  return pkts;
}



void SMTPClient::SL_AddEmailObject(QSharedPointer<EmailObject*> pml)
{
  if (!pml.isNull()) {
      if ((*pml.data())->GetGroups().isEmpty())
        return;
      else {

          (*pml.data())->SetProcessing(true);

          {
            quint64 cts_count = 0;

            QVector<QSharedPointer<CTGroup*>> grps = (*pml.data())->GetGroups();
            QVector<QSharedPointer<CTGroup*>>::const_iterator it = grps.constBegin(), ite = grps.constEnd();

            for (; it != ite; it++)
              cts_count += (*(*it).data())->GetContactsList().size();

            qDebug().noquote() << QString("SMTPClient::SL_AddEmailObject | cts_count = %1")
                                  .arg(cts_count);

            if (cts_count == 0) {
                (*pml.data())->SetProcessing(false);
                return;
              }
          }


          QVector<QSharedPointer<MailPKT*>> pkts = this->SplitEmailObj_ToPackets(pml);
          QVector<QSharedPointer<MailPKT*>>::const_iterator it = pkts.constBegin(), ite = pkts.constEnd();

          qint64 old_size = _mls_packets.size();

          for (; it != ite; it++)
            _mls_packets.append( *(*it).data() );

          qDebug().noquote() << QString("SMTPClient::SL_AddEmailObject | email (0x%1) splitted and added to mls_packets list.")
                                .arg((qint64)(*pml.data()), 8, 16, QChar('0'));

          qDebug().noquote() << QString("SMTPClient::SL_AddEmailObject | mls_packets.old_size = %1 | mls_packets.size = %2")
                                .arg(old_size).arg(_mls_packets.size());
        }
    }
}



void SMTPClient::SL_ProcessQueue()
{
  qDebug().noquote() << QString("SMTPClient::SL_ProcessQueue | mls_packets.size = %1 | busy = %2")
                        .arg(_mls_packets.size()).arg(_busy);

  if (!_mls_packets.isEmpty() && !_busy) {
      MailPKT *pkt = _mls_packets.takeFirst();

      qDebug().noquote() << QString("SMTPClient::SL_ProcessQueue | processing ml packet 0x%1 ..")
                            .arg((qint64)pkt, 8, 16, QChar('0'));

      this->SendMail(pkt);

      //emit SI_MailSent(QSharedPointer<MailPKT*>::create(pkt));



      //delete pkt;
      //pkt = nullptr;

      //this->SendMail(pkt);
    }
}



void SMTPClient::SendMail(MailPKT *mlpkt)
{
  if (_busy || !mlpkt) {
      if (_busy)
        qDebug().noquote() << QString("SMTPClient::SendMail | busy = true");

      if (!mlpkt)
        qDebug().noquote() << QString("SMTPClient::SendMail | mlpkt = nullptr");
      return;
  } else
    _busy = true;

  _curr_ml_packet = mlpkt;

  QString str;
  QByteArray
      deEmailAddrTo   = _curr_ml_packet->GetToRcpts().first().GetAddress(),
      deEmailAddrFrom = _curr_ml_packet->GetFrom().GetAddress();

  // Create header used by email server:
  QString tos_str = "Maerka Newsletter Group",
          smtp_sep = QDateTime::currentDateTime().toString("dd.MM.yyyy.hh.mm.ss.zzz").toUtf8().toBase64();

  str.append(QString("To: %1\n").arg(tos_str));
  str.append(QString("From: %1\n").arg(deEmailAddrFrom));
  str.append(QString("Subject: %1\n").arg(_curr_ml_packet->GetSubject()));
  str.append(QString("MIME-Version: 1.0\n"));
  str.append(QString("Content-Type: multipart/mixed; boundary=%1\n\n").arg(smtp_sep));
  str.append(QString("--%1\nContent-Type: text/html\n").arg(smtp_sep));
  str.append(QString("Content-Transfer-Encoding: base64\n\n"));
  str.append(_curr_ml_packet->GetContent().toBase64());
  str.append("\n\n");

  //QList<Email::S_Attach> attachments = _mail->GetAttachments();
  //QList<Email::S_Attach>::const_iterator it = attachments.constBegin(), ite = attachments.constEnd();

  /*for (; it != ite; it++) {
      str.append(QString("--%1\n").arg(smtp_sep));
      str.append(QString("Content-Type: %1\n").arg((*it).type));
      str.append(QString("Content-Disposition: inline; filename=%1;\n").arg( QString((*it).name).replace(" ", "_") ));
      str.append(QString("Content-Transfer-Encoding: base64\n\n"));
      str.append((*it).datas.toBase64());
      str.append("\n\n");
    }
  */

  str.append(QString("--%1--").arg(smtp_sep));

  str.replace(QString::fromLatin1("\n"), QString::fromLatin1("\r\n"));
  str.replace(QString::fromLatin1("\r\n.\r\n"), QString::fromLatin1("\r\n..\r\n"));


  qDebug().noquote() << QString("SMTPClient::SendMail | str = \n%1\n").arg(str);


  _curr_ml_packet->SetContent(str.toUtf8());


  // Connect to host:
  if (_socket->state() != QSslSocket::ConnectedState) {

      qDebug().noquote() << QString("SMTPClient::SendMail | connecting to smtp server %1 on port %2")
                            .arg(_host).arg(_port);

      _socket->connectToHostEncrypted(_host, _port);

      if (!_socket->waitForConnected(_conTimeout)) {
          return;
        }

      // Create text stream:
      if (!_stream)
        _stream = new QTextStream(_socket);

      // Call abort function when reach timeout:
      QTimer::singleShot(_timeout, this, &SMTPClient::SL_Abort);
    } else {
      _state = states::Auth;

      *_stream << "EHLO localhost" << "\r\n";
      _stream->flush();
    }
}



void SMTPClient::SL_ReadyRead()
{
  QString responseLine;

  do {
      responseLine = _socket->readLine();
      _response += responseLine;
    } while ( _socket->canReadLine() && responseLine[3] != ' ' );

  responseLine.truncate(3);


  if (_state == states::Init && responseLine == "220") {
      *_stream << "EHLO localhost" << "\r\n";
      _stream->flush();

      _state = states::HandShake;
    } else if (_state == states::HandShake && responseLine == "250") {
      _socket->startClientEncryption();
      _socket->waitForEncrypted(_conTimeout);

      *_stream << "EHLO localhost" << "\r\n";
      _stream->flush();

      _state = states::Auth;
    } else if (_state == states::Auth && responseLine == "250") {
      *_stream << "AUTH LOGIN" << "\r\n";
      _stream->flush();

      _state = states::User;
    } else if (_state == states::User && responseLine == "334") {
      QByteArray deAddr = _curr_ml_packet->GetCreds().GetAddress();

      //deAddr = _encryption->decode(deAddr, _certsDatas.clKey);
      //deAddr = _encryption->removePadding(deAddr);

      *_stream << QByteArray().append(deAddr).toBase64() << "\r\n";
      _stream->flush();

      _state = states::Pass;
    } else if (_state == states::Pass && responseLine == "334") {
      QByteArray dePwd = _curr_ml_packet->GetCreds().GetPasswd();

      //dePwd = _encryption->decode(dePwd, _certsDatas.clKey);
      //dePwd = _encryption->removePadding(dePwd);

      *_stream << QByteArray().append(dePwd).toBase64() << "\r\n";
      _stream->flush();

      _state = states::Mail;
    } else if (_state == states::Mail && responseLine == "235") {
      QByteArray deFrom = _curr_ml_packet->GetFrom().GetAddress();

      //deFrom = _encryption->decode(deFrom, _certsDatas.clKey);
      //deFrom = _encryption->removePadding(deFrom);

      *_stream << "MAIL FROM:<" << deFrom << ">\r\n";
      _stream->flush();

      _state = states::Rcpt;
    } else if (_state == states::Rcpt && responseLine == "250") {
      QList<MailAddr> tos = _curr_ml_packet->GetToRcpts();
      QByteArray deTo = tos.takeFirst().GetAddress();

      //deTo = _encryption->decode(deTo, _certsDatas.clKey);
      //deTo = _encryption->removePadding(deTo);

      *_stream << "RCPT TO:<" << deTo << ">\r\n";
      _stream->flush();

      _curr_ml_packet->SetToRcpts(tos);

      if (!_curr_ml_packet->GetToRcpts().isEmpty())
        _state = states::Rcpt;
      else
        _state = states::Data;
    } else if (_state == states::Data && responseLine == "250") {
      *_stream << "DATA\r\n";
      _stream->flush();

      _state = states::Body;
    } else if (_state == states::Body && responseLine == "354") {
      *_stream << _curr_ml_packet->GetContent() << "\r\n.\r\n";
      _stream->flush();

      _state = states::Quit;
    } else if (_state == states::Quit && responseLine == "250") {
      *_stream << "QUIT\r\n";
      _stream->flush();

      _state = states::Close;
    } else if (_state == states::Close) {
      _busy = false;
      qDebug().noquote() << QString("SMTPClient.state changed to [close]");

      emit SI_MailSent(QSharedPointer<MailPKT*>::create(_curr_ml_packet));

      _curr_ml_packet = nullptr;

      _socket->abort();
      _state = states::Init;

      return;
    } else {
      _busy = false;
      _state = states::Close;

      qDebug().noquote() << QString("[SMTPClient][SL_ReadyRead]\n")
                         << QString("  Failed\n")
                         << QString("  %1\n").arg(_response);
    }
}



void SMTPClient::SL_Abort()
{
  if (_socket && _socket->isOpen()) {
      _socket->abort();
      _state = states::Close;

      qDebug().noquote() << QString("[SMTPClient][SL_Abort]\n")
                         << QString("  Timeout reached : procedure aborted");
    }
}


const QByteArray & SMTPClient::GetCLKey() const { return _certsDatas.clKey; }
