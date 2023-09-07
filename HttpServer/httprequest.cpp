#include "httprequest.h"

#include <QDebug>

// Constructors

// Default:
HttpRequest::HttpRequest(QObject *parent)
  : QObject{parent}
{
}


// Variant 01:
HttpRequest::HttpRequest(const HTTPSV::CMDS &cmd, const HTTPSV::VERSIONS &version, const QByteArray &page,
                         QObject *parent)
  : QObject{parent},
    _cmd(cmd), _version(version), _page(page)
{
}


// Copy:
HttpRequest::HttpRequest(const HttpRequest &other)
  : QObject{other.parent()}
{
}



// Destructor:
HttpRequest::~HttpRequest()
{
}



// Operators

// Copy from other HttpRequest:
HttpRequest * HttpRequest::operator = (const HttpRequest &other)
{
  this->SetCMD(other.GetCMD());
  this->SetVersion(other.GetVersion());
  this->SetPage(other.GetPage());

  return this;
}



// ExtractFromStatusLine:
void HttpRequest::ExtractFromStatusLine(const QByteArray &status_line)
{
  if (status_line.isEmpty())
    return;

  QByteArrayList parts = status_line.split(QChar::Space);
  QByteArray cmd = "", page = "", version = "";

  if (parts.size() == 3) {
      cmd = parts.at(0);
      page = parts.at(1);
      version = parts.at(2);
    }

  this->SetCMD(_http_cmds.key(cmd, HTTPSV::CMDS::NONE));
  this->SetVersion(_http_versions.key(version, HTTPSV::VERSIONS::NONE));
  this->SetPage(page);
}



// BuildResponse:
HttpResponse HttpRequest::BuildResponse()
{
  // ---- TO DO ---- //
  // refactor code below

  HttpResponse rep(HTTPSV::CODES::OK, this->GetVersion(), this);


  if (this->GetCMD() == HTTPSV::CMDS::GET) {

      if (this->GetPage().at(1) == '?') {

          rep.SetContentType("text/html");
          rep.SetContentCharset("utf-8");

          rep.AddVariable("page_title", "HTTPSV - Asking autorization");
          rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");
          rep.AddVariable("base_url", "http://127.0.0.1:45062/");
          rep.AddVariable("style_filename", "def_style.css");

          rep.ParseFromModel("ask_consent");

        } else {

          if (this->GetPage().compare("/", Qt::CaseSensitive) == 0) {

              rep.SetContentType("text/html");
              rep.SetContentCharset("utf-8");

              rep.AddVariable("page_title", "HTTPSV - Index");
              rep.AddVariable("prog_title", "HTTPSV");
              rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");
              rep.AddVariable("base_url", "http://127.0.0.1:45062/");
              rep.AddVariable("style_filename", "def_style.css");

              rep.ParseFromModel("index");

            } else if (this->GetPage().compare("/ask_consent", Qt::CaseSensitive) == 0) {

              rep.SetContentType("text/html");
              rep.SetContentCharset("utf-8");

              rep.AddVariable("page_title", "HTTPSV - Asking autorization");
              rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");
              rep.AddVariable("base_url", "http://127.0.0.1:45062/");
              rep.AddVariable("style_filename", "def_style.css");

              rep.ParseFromModel("ask_consent");

            } else if (this->GetPage().compare("/ok", Qt::CaseSensitive) == 0) {

              rep.SetContentType("text/html");
              rep.SetContentCharset("utf-8");

              rep.AddVariable("page_title", "HTTPSV - Authorization granted");
              rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");
              rep.AddVariable("base_url", "http://127.0.0.1:45062/");
              rep.AddVariable("style_filename", "def_style.css");

              rep.ParseFromModel("ok");

            } else if (this->GetPage().compare("/cancel", Qt::CaseSensitive) == 0) {

              rep.SetContentType("text/html");
              rep.SetContentCharset("utf-8");

              rep.AddVariable("page_title", "HTTPSV - Authorization procedure cancelled.");
              rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");
              rep.AddVariable("base_url", "http://127.0.0.1:45062/");
              rep.AddVariable("style_filename", "def_style.css");

              rep.ParseFromModel("cancel");

            } else if (this->GetPage().compare("/docs/gmail_api", Qt::CaseSensitive) == 0) {

              rep.SetContentType("text/html");
              rep.SetContentCharset("utf-8");

              rep.AddVariable("page_title", "HTTPSV - Documentation - GMail API.");
              rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");
              rep.AddVariable("base_url", "http://127.0.0.1:45062/");
              rep.AddVariable("style_filename", "def_style.css");

              rep.AddVariable("docs_gmail_authentification_iface", "http://127.0.0.1:45062/gmail_login_iface.png");
              rep.AddVariable("docs_gmail_consent", "http://127.0.0.1:45062/gmail_consent.png");

              rep.ParseFromModel("gmail_api");

            } else if (this->GetPage().compare("/docs/contacts/import_export", Qt::CaseSensitive) == 0) {

              rep.SetContentType("text/html");
              rep.SetContentCharset("utf-8");

              rep.AddVariable("page_title", "HTTPSV - Documentation - Contacts Import/Export.");
              rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");
              rep.AddVariable("base_url", "http://127.0.0.1:45062/");
              rep.AddVariable("style_filename", "def_style.css");

              rep.ParseFromModel("contacts_ie");

            } else if (this->GetPage().compare("/docs/contacts/groups", Qt::CaseSensitive) == 0) {

              rep.SetContentType("text/html");
              rep.SetContentCharset("utf-8");

              rep.AddVariable("page_title", "HTTPSV - Documentation - Contacts Groups.");
              rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");
              rep.AddVariable("base_url", "http://127.0.0.1:45062/");
              rep.AddVariable("style_filename", "def_style.css");

              rep.ParseFromModel("contacts_groups");

            } else if (this->GetPage().compare("/docs_gmail_authentification_iface", Qt::CaseSensitive) == 0) {

              QFile f(":/docs/gmail_login_iface.png");

              if (f.open(QIODevice::ReadOnly)) {
                  QByteArray ct = f.readAll();
                  f.close();

                  rep.SetBody(ct);
                  rep.SetContentType("image/png");
                  rep.SetContentCharset("utf-8");
                  rep.SetContentTransfertEncoding("binary");
                  rep.SetAcceptRanges("bytes");
                  rep.SetBodyLength(f.size());
                } else {
                  rep.SetCode(HTTPSV::CODES::NOT_FOUND);
                  rep.SetContentType("text/html");
                  rep.SetContentCharset("utf-8");

                  rep.AddVariable("page_title", "HTTPSV - Not Found");
                  rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");

                  rep.ParseFromModel("not_found");
                }

            } else if (this->GetPage().compare("/docs_gmail_consent", Qt::CaseSensitive) == 0) {

              QFile f(":/docs/gmail_consent.png");

              if (f.open(QIODevice::ReadOnly)) {
                  QByteArray ct = f.readAll();
                  f.close();

                  rep.SetBody(ct);
                  rep.SetContentType("image/png");
                  rep.SetContentCharset("utf-8");
                  rep.SetContentTransfertEncoding("binary");
                  rep.SetAcceptRanges("bytes");
                  rep.SetBodyLength(f.size());
                } else {
                  rep.SetCode(HTTPSV::CODES::NOT_FOUND);
                  rep.SetContentType("text/html");
                  rep.SetContentCharset("utf-8");

                  rep.AddVariable("page_title", "HTTPSV - Not Found");
                  rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");

                  rep.ParseFromModel("not_found");
                }

            } else if (this->GetPage().compare("/assoc_logo.png", Qt::CaseSensitive) == 0) {
              QFile f(":/assoc_logo.png");

              if (f.open(QIODevice::ReadOnly)) {
                  QByteArray ct = f.readAll();
                  f.close();

                  rep.SetBody(ct);
                  rep.SetContentType("image/png");
                  rep.SetContentCharset("utf-8");
                  rep.SetContentTransfertEncoding("binary");
                  rep.SetAcceptRanges("bytes");
                  rep.SetBodyLength(f.size());
                } else {
                  rep.SetCode(HTTPSV::CODES::NOT_FOUND);
                  rep.SetContentType("text/html");
                  rep.SetContentCharset("utf-8");

                  rep.AddVariable("page_title", "HTTPSV - Not Found");
                  rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");

                  rep.ParseFromModel("not_found");
                }

            } else if (this->GetPage().compare("/def_style.css", Qt::CaseSensitive) == 0) {
              QFile f(":/html_models/def_style.css");

              if (f.open(QIODevice::ReadOnly)) {
                  QByteArray ct = f.readAll();
                  f.close();

                  rep.SetContentType("text/css");
                  rep.SetContentCharset("utf-8");

                  rep.SetBody(ct);
                  rep.SetBodyLength(ct.size());
                }

            } else {
              rep.SetCode(HTTPSV::CODES::NOT_FOUND);
              rep.SetContentType("text/html");
              rep.SetContentCharset("utf-8");

              rep.AddVariable("page_title", "HTTPSV - Not Found");
              rep.AddVariable("assoc_log_url", "http://127.0.0.1:45062/assoc_logo.png");
              rep.AddVariable("base_url", "http://127.0.0.1:45062/");
              rep.AddVariable("style_filename", "def_style.css");

              rep.ParseFromModel("not_found");
            }
        }


      QByteArray packet_datas;

      // add status line:
      packet_datas.append(QString("%1 %2\r\n")
                          .arg(_http_versions.value(rep.GetVersion(), ""),
                               _http_codes.value(rep.GetCode(), ""))
                          .toUtf8());

      // add Date & Server:
      packet_datas.append(QString("Date: %1 GMT\r\n"
                                  "Server: %2\r\n")
                          .arg(QDateTime::currentDateTime().toString("ddd, d MMMM yyyy hh:mm:ss"),
                               "HTTPSV")
                          .toUtf8());

      // add Content-Type, Content-Transfert-Encoding, Content-Charset, Accept-Ranges:
      if (!rep.GetContentType().isEmpty())
        packet_datas.append(QString("Content-Type: %1\r\n").arg(rep.GetContentType()).toUtf8());

      if (!rep.GetContentCharset().isEmpty())
        packet_datas.append(QString("Content-Charset: %1\r\n").arg(rep.GetContentCharset()).toUtf8());

      if (!rep.GetContentTransfertEncoding().isEmpty())
        packet_datas.append(QString("Content-Transfert-Encoding: %1\r\n").arg(rep.GetContentTransfertEncoding()).toUtf8());

      if (!rep.GetAcceptRanges().isEmpty())
        packet_datas.append(QString("Accept-Ranges: %1\r\n").arg(rep.GetAcceptRanges()).toUtf8());


      // add Content-Length & body:
      packet_datas.append(QString("Content-Length: %1\r\n\r\n").arg(rep.GetBodyLength()).toUtf8());
      packet_datas.append(rep.GetBody());

      rep.SetPacketDatas(packet_datas);
    }

  return rep;
}


void HttpRequest::SetCMD(const HTTPSV::CMDS &cmd) { _cmd = cmd; }
void HttpRequest::SetVersion(const HTTPSV::VERSIONS &version) { _version = version; }
void HttpRequest::SetPage(const QByteArray &page) { _page = page; }


const HTTPSV::CMDS & HttpRequest::GetCMD() const { return _cmd; }
const HTTPSV::VERSIONS & HttpRequest::GetVersion() const { return _version; }
const QByteArray & HttpRequest::GetPage() const { return _page; }
