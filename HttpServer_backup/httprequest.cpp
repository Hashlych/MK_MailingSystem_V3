#include "httprequest.h"
#include "HttpServer/httpresponse.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif


HttpRequest::HttpRequest(QObject *parent)
  : QObject{parent}
{
}



void HttpRequest::DebugInfos()
{
  qDebug().noquote() << QString("--------[ HttpRequest Informations - 0x%1] --------")
                        .arg((qint64)this, 8, 16, QChar('0'));

  qDebug().noquote() << QString("  + command = %1")
                        .arg(_http_cmds_str.value(this->GetCommand()));

  qDebug().noquote() << QString("  + http version = %1")
                        .arg(_http_vers_str.value(this->GetHttpVersion()));

  qDebug().noquote() << QString("  + page = %1")
                        .arg(this->GetPage());

  qDebug().noquote() << QString("  + params count = %1")
                        .arg(_params.size());

  QHash<MK_HTTP::PARAMS, QByteArray>::const_iterator it = _params.constBegin(), ite = _params.constEnd();

  for (; it != ite; it++)
    qDebug().noquote() << QString("  -> %1 = %2")
                          .arg(_http_params_str.value(it.key()), it.value() );

  qDebug().noquote() << QString("  + gml_params.state = %1")
                        .arg(_gml_params.state);
  qDebug().noquote() << QString("  + gml_params.code = %1")
                        .arg(_gml_params.code);
  qDebug().noquote() << QString("  + gml_params.scope = %1")
                        .arg(_gml_params.scope);

  qDebug().noquote() << QString("--------[ HttpRequestInformations END ]--------\n");
}



HttpResponse HttpRequest::BuildResponse(const HttpRequest &req)
{
  qDebug() << QString("Building HttpResponse for HttpRequest 0x%1")
              .arg((qint64)&req, 8, 16, QChar('0'));

  HttpResponse rep;
  QByteArray page = req.GetPage();

  rep.SetHttpVersion(req.GetHttpVersion());

  rep.AddParam(MK_HTTP::PARAMS::SERVER, "MK_MailingSystem_V3");
  rep.AddParam(MK_HTTP::PARAMS::DATE,
               QDateTime::currentDateTime().toString("ddd, d MMMM yyyy hh:mm:ss").toUtf8());
  rep.AddParam(MK_HTTP::PARAMS::CONNECTION, "keep-alive");
  //rep.AddParam(MK_HTTP::PARAMS::KEEP_ALIVE, "3600");


  switch (req.GetCommand()) {

    case MK_HTTP::CMDS::GET:
      qDebug().noquote() << QString("[HttpRequest::BuildResponse] command : GET");

      if (page.isEmpty()) {
          if (!req.GetGMLParams().state.isEmpty()) {
              rep.SetHttpCode(MK_HTTP::CODES::OK);
              rep.SetPageTitle("MK_MailingSystem_V3 - Authorization required");
              rep.AddParam(MK_HTTP::PARAMS::CONTENT_TYPE, "text/html");

              rep.AddVariable("page_title", rep.GetPageTitle());
              rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");

              rep.GenerateResponse("ask_consent");
            }
        } else {
          if (req.GetPage().compare("ok", Qt::CaseSensitive) == 0) {
              rep.SetHttpCode(MK_HTTP::CODES::OK);
              rep.SetPageTitle("MK_MailingSystem_V3 - Confirmed");
              rep.AddParam(MK_HTTP::PARAMS::CONTENT_TYPE, "text/html");

              rep.AddVariable("page_title", rep.GetPageTitle());

              rep.GenerateResponse(page);
            } else if (req.GetPage().compare("cancel", Qt::CaseSensitive) == 0) {
              rep.SetHttpCode(MK_HTTP::CODES::OK);
              rep.SetPageTitle("MK_MailingSystem_V3 - Refused");
              rep.AddParam(MK_HTTP::PARAMS::CONTENT_TYPE, "text/html");

              rep.AddVariable("page_title", rep.GetPageTitle());

              rep.GenerateResponse(page);
            } else if (req.GetPage().compare("assoc_logo.png", Qt::CaseSensitive) == 0) {
              qDebug().noquote() << QString("[HttpRequest::BuildReponse] GET assoc_logo.png");

              rep.SetHttpCode(MK_HTTP::CODES::OK);
              rep.AddParam(MK_HTTP::PARAMS::CONTENT_TYPE, "image/png,*/*");

              QFile f(":/assoc_logo.png");

              if (f.open(QIODevice::ReadOnly)) {
                  QFileInfo info(f);

                  rep.SetParsedContent(f.readAll());

                  rep.AddParam(MK_HTTP::PARAMS::LAST_MODIFIED,
                               QString("%1 GMT").arg(info.lastModified().toString("ddd, d MMMM yyyy hh:mm:ss"))
                               .toUtf8());

                  f.close();
                }
            }
        }
      break;

    case MK_HTTP::CMDS::HEAD:
      qDebug().noquote() << QString("[HttpRequest::BuildResponse] command : HEAD");
      break;

    default:
      qDebug().noquote() << QString("[HttpRequest::BuildResponse] command : default action = break");
      break;
    }

  return rep;
}



void HttpRequest::AddParam(const MK_HTTP::PARAMS &param_ref, const QByteArray &param_val)
{
  if (_params.contains(param_ref))
    return;

  _params.insert(param_ref, param_val);
}



void HttpRequest::SetCommand(const MK_HTTP::CMDS &cmd) { _cmd = cmd; }
void HttpRequest::SetHttpVersion(const MK_HTTP::VERSIONS &vers) { _http_vers = vers; }
void HttpRequest::SetPage(const QByteArray &page) { _page = page; }
void HttpRequest::SetGMLParams(const S_GML_Params &params) { _gml_params = params; }


const MK_HTTP::CMDS & HttpRequest::GetCommand() const { return _cmd; }
const MK_HTTP::VERSIONS & HttpRequest::GetHttpVersion() const { return _http_vers; }
const QByteArray & HttpRequest::GetPage() const { return _page; }
const S_GML_Params & HttpRequest::GetGMLParams() const { return _gml_params; }
