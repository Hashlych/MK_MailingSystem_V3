#include "httpserver.h"


/*QHash<HTTP_VERS, QByteArray> _http_vers_str = {
  QPair<HTTP_VERS, QByteArray>(HTTP_VERS::NONE, ""),
  QPair<HTTP_VERS, QByteArray>(HTTP_VERS::V1_1, "HTTP/1.1"),
  QPair<HTTP_VERS, QByteArray>(HTTP_VERS::V2_0, "HTTP/2"),
};

QHash<HTTP_CODES, QByteArray> _http_codes_str = {
  QPair<HTTP_CODES, QByteArray>(HTTP_CODES::NONE, ""),
  QPair<HTTP_CODES, QByteArray>(HTTP_CODES::OK, "OK"),
  QPair<HTTP_CODES, QByteArray>(HTTP_CODES::NOT_FOUND, "Not Found"),
  QPair<HTTP_CODES, QByteArray>(HTTP_CODES::NOT_MODIFIED, "Not Modified"),
  QPair<HTTP_CODES, QByteArray>(HTTP_CODES::BAD_REQUEST, "Bad Request"),
};

QHash<HTTP_REQ_PARAMS, QByteArray> _http_req_params_str = {
  QPair<HTTP_REQ_PARAMS, QByteArray>(HTTP_REQ_PARAMS::ACCEPT_LANGUAGE, "Accept-Language"),
  QPair<HTTP_REQ_PARAMS, QByteArray>(HTTP_REQ_PARAMS::ACCEPT_ENCODING, "Accept-Encoding"),
  QPair<HTTP_REQ_PARAMS, QByteArray>(HTTP_REQ_PARAMS::CONNECTION, "Connection"),
  QPair<HTTP_REQ_PARAMS, QByteArray>(HTTP_REQ_PARAMS::USER_AGENT, "User-Agent"),
};

QHash<HTTP_RSP_PARAMS, QByteArray> _http_rsp_params_str = {
  QPair<HTTP_RSP_PARAMS, QByteArray>(HTTP_RSP_PARAMS::CONTENT_TYPE, "Content-Type"),
  QPair<HTTP_RSP_PARAMS, QByteArray>(HTTP_RSP_PARAMS::CONTENT_LENGTH, "Content-Length"),
  QPair<HTTP_RSP_PARAMS, QByteArray>(HTTP_RSP_PARAMS::DATE, "Date"),
  QPair<HTTP_RSP_PARAMS, QByteArray>(HTTP_RSP_PARAMS::LAST_MODIFIED, "Last-Modified"),
  QPair<HTTP_RSP_PARAMS, QByteArray>(HTTP_RSP_PARAMS::SERVER, "Server"),
  QPair<HTTP_RSP_PARAMS, QByteArray>(HTTP_RSP_PARAMS::CONNECTION, "Connection"),
  QPair<HTTP_RSP_PARAMS, QByteArray>(HTTP_RSP_PARAMS::KEEP_ALIVE, "Keep-Alive"),
};*/



QHash<MK_HTTP::VERSIONS, QByteArray> _http_vers_str = {
  QPair<MK_HTTP::VERSIONS, QByteArray>(MK_HTTP::VERSIONS::NONE, ""),
  QPair<MK_HTTP::VERSIONS, QByteArray>(MK_HTTP::VERSIONS::V1_1, "HTTP/1.1"),
  QPair<MK_HTTP::VERSIONS, QByteArray>(MK_HTTP::VERSIONS::V2_0, "HTTP/2"),
};

QHash<MK_HTTP::CODES, QByteArray> _http_codes_str = {
  QPair<MK_HTTP::CODES, QByteArray>(MK_HTTP::CODES::NONE, ""),
  QPair<MK_HTTP::CODES, QByteArray>(MK_HTTP::CODES::OK, "OK"),
  QPair<MK_HTTP::CODES, QByteArray>(MK_HTTP::CODES::NOT_FOUND, "Not Found"),
  QPair<MK_HTTP::CODES, QByteArray>(MK_HTTP::CODES::NOT_MODIFIED, "Not Modified"),
  QPair<MK_HTTP::CODES, QByteArray>(MK_HTTP::CODES::BAD_REQUEST, "Bad Request"),
};

QHash<MK_HTTP::CMDS, QByteArray> _http_cmds_str = {
  QPair<MK_HTTP::CMDS, QByteArray>(MK_HTTP::CMDS::NONE, ""),
  QPair<MK_HTTP::CMDS, QByteArray>(MK_HTTP::CMDS::GET, "GET"),
  QPair<MK_HTTP::CMDS, QByteArray>(MK_HTTP::CMDS::POST, "POST"),
  QPair<MK_HTTP::CMDS, QByteArray>(MK_HTTP::CMDS::HEAD, "HEAD"),
};


QHash<MK_HTTP::PARAMS, QByteArray> _http_params_str = {
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::ACCEPT_ENCODING, "Accept-Encoding"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::ACCEPT_LANGUAGE, "Accept-Language"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::ACCEPT, "Accept"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::CONNECTION, "Connection"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::CONTENT_LENGTH, "Content-Length"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::CONTENT_TYPE, "Content-Type"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::DATE, "Date"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::KEEP_ALIVE, "Keep-Alive"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::LAST_MODIFIED, "Last-Modified"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::SERVER, "Server"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::USER_AGENT, "User-Agent"),
  QPair<MK_HTTP::PARAMS, QByteArray>(MK_HTTP::PARAMS::HOST, "Host"),
};



QHash<QByteArray, QByteArray> _html_models = {
  QPair<QByteArray, QByteArray>("ask_consent",
    HttpResponse::LoadHtmlModel(QStringLiteral(":/html_models/ask_consent.html"))),
  QPair<QByteArray, QByteArray>("ok",
    HttpResponse::LoadHtmlModel(QStringLiteral(":/html_models/ok.html"))),
  QPair<QByteArray, QByteArray>("cancel",
    HttpResponse::LoadHtmlModel(QStringLiteral(":/html_models/cancel.html"))),
};



/* ---------------- Constructors ---------------- */
HttpServer::HttpServer(QObject *parent)
  : QObject{parent}
{
}


/* ---------------- Destructor ---------------- */
HttpServer::~HttpServer()
{
  if (_server)
    _server->close();
}


/* ---------------- SL_Initialize ---------------- */
void HttpServer::SL_Initialize()
{
  if (_init)
    return;

  _server = new QTcpServer(this);

  connect(_server, &QTcpServer::newConnection, this, &HttpServer::SL_NewConnection);

  _init = true;
}


/* ---------------- SL_Start ---------------- */
void HttpServer::SL_Start()
{
  if (_server && !_server->isListening()) {

      if (_paused) {
          _server->resumeAccepting();
          _paused = false;
        } else {
          if (!_server->listen(_sv_params.addr, _sv_params.port))
            emit SI_AddLog(LOG_TYPES::ERR,
                           QString("Failed to start internal HTTP Server at %1:%2")
                           .arg(_sv_params.addr.toString())
                           .arg(_sv_params.port).toUtf8());
        }
    }
}


/* ---------------- SL_Stop ---------------- */
void HttpServer::SL_Stop()
{
  if (_server && _server->isListening()) {
      _server->pauseAccepting();
      _paused = true;
    }
}


/* ---------------- SL_NewConnection ---------------- */
void HttpServer::SL_NewConnection()
{
  if (_server && _server->isListening() && _server->hasPendingConnections() && !_client) {
      _client = _server->nextPendingConnection();

      if (_client)
        connect(_client, &QTcpSocket::readyRead, this, &HttpServer::SL_ReadyRead);
    }
}





void HttpServer::SL_ReadyRead()
{
  QTcpSocket *cl = qobject_cast<QTcpSocket*>(sender());

  if (!cl) return;

  /*QByteArray content =
      "<!DOCTYPE html>"
      "<html>"
      "<head>"
      "<title></title>"
      "<meta charset=\"utf-8\" />"
      "</head>"
      "<body>"
      "test<br />"
      "<a href=\"http://127.0.0.1:45062/ok\">test - ok</a><br />"
      "<a href=\"http//127.0.0.1:45062/cancel\">test - cancel</a>"
      "</body>"
      "</html>";

  QString datas;

  datas.append("HTTP/1.1 200 OK\r\n");
  datas.append("Server: MK_MailingSystem_V3\r\n");
  datas.append(QString("Date: %1\r\n").arg(QDateTime::currentDateTime().toString("ddd, d, MMMM yyyy hh:mm:ss")).toUtf8());
  datas.append("Content-Type: text/html\r\n");
  datas.append("Connection: keep-alive\r\n");
  datas.append(QString("Content-Length: %1\r\n\r\n").arg(content.size()));
  datas.append(content);

  qDebug() << "datas =\n" << datas;

  cl->write(datas.toUtf8());*/



  // read datas sent by client:
  QByteArray datas = cl->readAll();

  // Split to lines list:
  QByteArrayList lines = datas.split(QChar::SpecialCharacter::CarriageReturn),
      tmp_list;

  if (lines.isEmpty()) return;

  qDebug().noquote() << QString("[HttpServer::SL_ReadyRead] Received datas from client:");
  qDebug().noquote() << QString("[HttpServer::SL_ReadyRead]   + datas.size = %1")
                        .arg(datas.size());
  qDebug().noquote() << QString("[HttpServer::SL_ReadyRead]   + lines = %1")
                        .arg(lines.size());


  QByteArray cmd, page, http_vers;

  for (int n = 0; n < lines.size(); n++) {
      QByteArray tmp = lines.at(n).trimmed();
      tmp = tmp.replace("\n", "");

      if (n == 0 && cmd.isEmpty()) {
          QByteArrayList tmpl = tmp.split(QChar::Space);

          if (tmpl.size() == 3) {
              cmd = tmpl.at(0);
              page = tmpl.at(1);
              //page = page.remove(0,1);
              http_vers = tmpl.at(2);
            }
        }
    }


  QByteArray gg_state, gg_code, gg_scope;
  HttpRequest hreq;

  hreq.SetCommand(_http_cmds_str.key(cmd));
  hreq.SetHttpVersion(_http_vers_str.key(http_vers, MK_HTTP::VERSIONS::V1_1));

  if (cmd.compare("GET", Qt::CaseSensitive) == 0) {

      QByteArrayList
          tmpl = page.split('?'),
          tmpll;

      if (tmpl.size() >= 2) {
          tmpl[0] = tmpl[0].remove(0,1);
          tmpll = tmpl[1].split('&');

          hreq.SetPage(tmpl.at(0));
        }


      // extract state, code, scope:
      QByteArrayList::const_iterator it = tmpll.constBegin(), ite = tmpll.constEnd();

      for (; it != ite; it++) {
          QByteArrayList tmp_param = (*it).split('=');

          if (tmp_param.size() == 2) {
              if (gg_state.isEmpty() && tmp_param.first().compare("state", Qt::CaseSensitive) == 0)
                gg_state = tmp_param.last();

              if (gg_code.isEmpty() && tmp_param.first().compare("code", Qt::CaseSensitive) == 0)
                gg_code = tmp_param.last();

              if (gg_scope.isEmpty() && tmp_param.first().compare("scope", Qt::CaseSensitive) == 0)
                gg_scope = tmp_param.last();
            }
        }

      hreq.SetGMLParams( S_GML_Params{gg_state, gg_code, gg_scope} );

      // extract http headers:
      for (int n = 1; n < lines.size(); n++) {
          QByteArrayList tmpl = lines.at(n).split(':');

          if (tmpl.size() == 2) {
              tmpl[0] = tmpl[0].replace("\n", "");
              hreq.AddParam(_http_params_str.key(tmpl.first()), tmpl.last());
            }
        }
    }

  hreq.DebugInfos();

  HttpResponse rep = HttpRequest::BuildResponse(hreq);

  rep.DebugInfos();

  QByteArray result = rep.GetResult();

  qint64 wrote = cl->write(result);
  cl->flush();

  qDebug().noquote() << QString("[HttpServer::SL_ReadyRead] wrote %1/%2 bytes")
                        .arg(wrote).arg(result.size());


  /*HttpResponse img_rep;

  img_rep.SetHttpCode(MK_HTTP::CODES::OK);
  img_rep.AddParam(MK_HTTP::PARAMS::SERVER, "MK_MailingSystem_V3");
  img_rep.AddParam(MK_HTTP::PARAMS::DATE,
                   QDateTime::currentDateTime().toString("ddd, d MMMM yyyy hh:mm:ss").toUtf8());
  img_rep.AddParam(MK_HTTP::PARAMS::CONNECTION, "keep-alive");*/

  //img_rep.AddParam(MK_HTTP::PARAMS::CONTENT_TYPE, "image/png,*/*");

  /*QFile f(":/assoc_logo.png");

  if (f.open(QIODevice::ReadOnly)) {
      QFileInfo info(f);

      img_rep.SetParsedContent(f.readAll());

      img_rep.AddParam(MK_HTTP::PARAMS::LAST_MODIFIED,
                   QString("%1 GMT").arg(info.lastModified().toString("ddd, d MMMM yyyy hh:mm:ss"))
                   .toUtf8());

      f.close();
    }

  img_rep.AddParam(MK_HTTP::PARAMS::CONTENT_LENGTH, QString("%1").arg(f.size()).toUtf8());

  if (img_rep.GenerateResponse("")) {
      result = img_rep.GetResult();
      wrote = cl->write(result);

      qDebug().noquote() << QString("[HttpServer::SL_ReadyRead] wrote %1/%2 bytes")
                            .arg(wrote).arg(result.size());
    }

  //qDebug() << result;*/
}


/* ----------------  ---------------- */
