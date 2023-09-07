#include "httpserver.h"


// Constructors

// Default:
HttpServer::HttpServer(QObject *parent)
  : QObject{parent}
{
}

// Variant 01:
HttpServer::HttpServer(const QHostAddress &addr, const quint16 &port, QObject *parent)
  : QObject{parent},
    _addr(addr), _port(port)
{
}



// Destructor:
HttpServer::~HttpServer()
{
  // cleanup clients list:
  while (!_clients.isEmpty()) {
      if (_clients.first()->isOpen())
        _clients.first()->close();
      else
        delete _clients.takeFirst();
    }


  // close server (delete handle by parent):
  if (_sv && _sv->isListening())
    _sv->close();
}



// SL_Initialize:
void HttpServer::SL_Initialize()
{
  if (_init)
    return;

  this->LoadHtmlModels();

  _sv = new QTcpServer(this);

  connect(_sv, &QTcpServer::newConnection, this, &HttpServer::SL_NewConnection);


  _ntmanager = new QNetworkAccessManager(this);

  connect(_ntmanager, &QNetworkAccessManager::finished, this, &HttpServer::SL_NTRepFinished);

  _init = true;


  // auto start server:
  QTimer::singleShot(50, this, &HttpServer::SL_Start);
}



// SL_Start:
void HttpServer::SL_Start()
{
  if (!_init || _started)
    return;

  if (_sv->listen(_addr, _port))
    _started = true;
}



// SL_Pause:
void HttpServer::SL_Pause(const bool &pause)
{
  if (!_init || !_started)
    return;

  if (pause)
    _sv->pauseAccepting();
  else
    _sv->resumeAccepting();
}



// SL_StartGMailProcedure:
void HttpServer::SL_StartGMailProcedure(S_IDT_Datas &idt_datas)
{
  QObject *s_obj = this->sender();
  Identity *idt = new Identity(idt_datas.name, idt_datas.addr, nullptr);

  idt->SetCode(idt_datas.code);
  idt->SetTokens(idt_datas.tokens.access,
                 idt_datas.tokens.refresh,
                 idt_datas.tokens.type,
                 idt_datas.tokens.expires_in);

  if (s_obj && s_obj->thread() != this->thread())
    idt->moveToThread(s_obj->thread());


}



// SL_NewConnection:
void HttpServer::SL_NewConnection()
{
  QTcpSocket *cl = _sv->nextPendingConnection();

  if (cl && !_clients.contains(cl)) {
      connect(cl, &QTcpSocket::disconnected, this, &HttpServer::SL_Disconnected);
      connect(cl, &QTcpSocket::readyRead, this, &HttpServer::SL_ReadyRead);

      _clients.append(cl);
    }
}



// SL_Disconnected:
void HttpServer::SL_Disconnected()
{
  QTcpSocket *cl = qobject_cast<QTcpSocket*>(sender());

  if (cl) {
      for (int n = 0; n < _clients.size(); n++) {
          if (_clients.at(n) == cl) {
              delete _clients.takeAt(n);
              break;
            }
        } // end for each clients
    }
}



// SL_ReadyRead:
void HttpServer::SL_ReadyRead()
{
  QTcpSocket *cl = qobject_cast<QTcpSocket*>(sender());

  if (cl) {
      // Read all available bytes from client socket:
      QByteArray datas = cl->readAll();

      // Split by CarriageReturn '\r':
      QByteArrayList lines = datas.split(QChar::SpecialCharacter::CarriageReturn);

      qDebug().noquote() << datas;


      // Parse request, build response and send it to the client:
      HttpRequest req;
      HttpResponse rep;

      // Extract http command, versions and the page requested:
      QByteArray status_line = lines.takeFirst().replace("\n", "");
      req.ExtractFromStatusLine(status_line);

      if (req.GetPage().startsWith("/?state"))
        this->ExtractGMailVars(req.GetPage());

      // Build the response based on the request parameters:
      rep = req.BuildResponse();

      // Send datas to client:
      cl->write(rep.GetPacketDatas());



      if (req.GetPage().compare("/ok", Qt::CaseSensitive) == 0) {
          cl->close();

          QNetworkRequest ntreq(QUrl("https://oauth2.googleapis.com/token"));
          QUrlQuery postDatas;

          postDatas.addQueryItem("code", this->GetGMailVars_Code());
          postDatas.addQueryItem("client_id", G_CL_ID);
          postDatas.addQueryItem("client_secret", G_CL_SECRET);
          postDatas.addQueryItem("redirect_uri", QUrl("http://127.0.0.1:45062").toString(QUrl::FullyEncoded).toUtf8());
          postDatas.addQueryItem("grant_type", "authorization_code");

          ntreq.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/x-www-form-urlencoded");

          if (_ntmanager->post(ntreq, postDatas.toString(QUrl::FullyEncoded).toUtf8()))
            qDebug().noquote() << QString("[HttpServer::SL_ReadyRead] POST request sent to https://oauth2.googleapis.com/token");
        }
    }
}



void HttpServer::SL_NTRepFinished(QNetworkReply *rep)
{
  if (rep) {

      QByteArray datas = rep->readAll();

      qDebug().noquote() << QString("[HttpServer::SL_NTRepFinished] datas.size = %1")
                            .arg(datas.size());

      qDebug() << "\n----------------\n" << datas << "\n----------------\n";

      QJsonDocument jdoc = QJsonDocument::fromJson(datas);

      if (jdoc.isNull())
        qDebug().noquote() << QString("[HttpServer::SL_NTRepFinished] jdoc is null");

      if (jdoc.isArray()) {
          qDebug().noquote() << QString("[HttpServer::SL_NTRepFinished] jdoc is array");

          QJsonArray jarr = jdoc.array();

          for (int n = 0; n < jarr.size(); n++)
            qDebug().noquote() << QString("jarr[%1] = %2")
                                  .arg(n).arg(jarr.at(n).toString());
        }

      if (jdoc.isObject()) {
          qDebug().noquote() << QString("[HttpServer::SL_NTRepFinished] jdoc is object");

          QJsonObject jobj = jdoc.object();
          QVariantHash vh = jobj.toVariantHash();
          QVariantHash::const_iterator it = vh.constBegin(), ite = vh.constEnd();

          for (; it != ite; it++) {
              qDebug().noquote() << QString("%1 = %2")
                                    .arg(vh.key((*it)))
                                    .arg((*it).toString());
            }
        }


      QTimer::singleShot(10, rep, &QNetworkReply::deleteLater);
    }
}



void HttpServer::SL_SslErrors(const QList<QSslError> &errs)
{
  for (int n = 0; n < errs.size(); n++) {
      qDebug().noquote() << QString("[HttpServer::SL_SslErrors] errs[%1] = %2")
                            .arg(n).arg(errs.at(n).errorString());
    }
}



// Load HTML pages:
void HttpServer::LoadHtmlModels()
{
  if (!_html_models.isEmpty())
    return;

  QFile file;

  QByteArrayList pages = {
    "index",
    "ask_consent",
    "ok",
    "cancel",
    "not_found",
    "gmail_api",
    "contacts_ie",
    "contacts_groups",
  };

  QByteArrayList::const_iterator it = pages.constBegin(), ite = pages.constEnd();

  for (int n = 0; it != ite; it++, n++) {
      file.setFileName( QString(":/html_models/%1.html").arg((*it)) );

      if (file.open(QIODevice::ReadOnly)) {
          _html_models.insert( QString("%1").arg((*it)).toUtf8(), file.readAll() );

          file.close();
          file.setFileName("");
        }
    }
}



// ExtractGMailVars:
void HttpServer::ExtractGMailVars(const QByteArray &url)
{
  QByteArray tmp(url);
  QByteArrayList vars = tmp.replace("/?", "").split('&');

  QByteArray state, code;
  QByteArrayList scopes;

  for (int n = 0; n < vars.size(); n++) {
      QByteArrayList tmpl = vars.at(n).split('=');

      if (tmpl.size() == 2) {
          if (state.isEmpty() && tmpl.first().compare("state", Qt::CaseSensitive) == 0)
            state = tmpl.last();

          if (code.isEmpty() && tmpl.first().compare("code", Qt::CaseSensitive) == 0)
            code = tmpl.last();

          if (scopes.isEmpty() && tmpl.first().compare("scope", Qt::CaseSensitive) == 0)
            scopes = tmpl.last().split('+');
        }
    }

  this->SetGMailVars(state, code, scopes);
}




void HttpServer::SetGMailVars(const QByteArray &state, const QByteArray &code, const QByteArrayList &scopes)
{
  _gmail_vars = {state, code, scopes};
}
void HttpServer::SetGMailVars_State(const QByteArray &state) { _gmail_vars.state = state; }
void HttpServer::SetGMailVars_Code(const QByteArray &code) { _gmail_vars.code = code; }
void HttpServer::SetGMailVars_Scopes(const QByteArrayList &scopes) { _gmail_vars.scopes = scopes; }


const QByteArray & HttpServer::GetGMailVars_State() const { return _gmail_vars.state; }
const QByteArray & HttpServer::GetGMailVars_Code() const { return _gmail_vars.code; }
const QByteArrayList & HttpServer::GetGMailVars_Scopes() const { return _gmail_vars.scopes; }

QSharedPointer<QNetworkAccessManager*> HttpServer::GetNetworkAccessMG() const
{
  return QSharedPointer<QNetworkAccessManager*>::create(_ntmanager);
}
