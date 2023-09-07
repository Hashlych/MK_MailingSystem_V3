#include "httpresponse.h"

HttpResponse::HttpResponse(QObject *parent)
  : QObject{parent}
{
}

HttpResponse::HttpResponse(const HttpResponse &other)
  : QObject{other.parent()}
{
  *this = other;
}

HttpResponse::~HttpResponse()
{
}


HttpResponse * HttpResponse::operator = (const HttpResponse &other)
{
  this->SetHttpCode(other.GetHttpCode());
  this->SetHttpVersion(other.GetHttpVersion());
  this->SetPageTitle(other.GetPageTitle());
  this->SetParams(other.GetParams());

  return this;
}


QByteArray HttpResponse::GetHttpVers_AsText(const HttpResponse &rsp)
{
  return _http_vers_str.value(rsp.GetHttpVersion());
}


QByteArray HttpResponse::GetHttpCode_AsText(const HttpResponse &rsp)
{
  return _http_codes_str.value(rsp.GetHttpCode());
}


QByteArray HttpResponse::LoadHtmlModel(const QString &model_path)
{
  QByteArray content;
  QFile file(model_path);

  if (file.open(QIODevice::ReadOnly)) {
      content = file.readAll();
      file.close();
    }

  return content;
}



void HttpResponse::DebugInfos()
{
  qDebug().noquote() << QString("--------[ HttpResponse Debug Informations - 0x%1 ]---------")
                        .arg((qint64)this, 8, 16, QChar('0'));

  qDebug().noquote() << QString("  + http code = %1")
                        .arg(_http_codes_str.value(this->GetHttpCode()));

  qDebug().noquote() << QString("  + http version = %2")
                        .arg(_http_vers_str.value(this->GetHttpVersion()));

  qDebug().noquote() << QString("  + page title = %1")
                        .arg(this->GetPageTitle());

  qDebug().noquote() << QString("  + content.size = %1")
                        .arg(_parsed_content.size());

  qDebug().noquote() << QString("  + params count = %1")
                        .arg(_params.size());

  QHash<MK_HTTP::PARAMS, QByteArray>::const_iterator it = _params.constBegin(), ite = _params.constEnd();

  for (; it != ite; it++)
    qDebug().noquote() << QString("  -> %1 = %2")
                          .arg(_http_params_str.value(it.key()), it.value());

  qDebug().noquote() << QString("--------[ HttpResponse Debug Inforamtions - 0x%1 - END ]--------\n")
                        .arg((qint64)this, 8, 16, QChar('0'));
}



void HttpResponse::AddVariable(const QByteArray &var_name, const QByteArray &var_content)
{
  /*qDebug().noquote() << QString("[HttpResponse::AddVariable] var_name = %1 | var_content = %2")
                        .arg(var_name, var_content);*/

  if (var_name.isEmpty() || _vars.contains(var_name)) {
      if (var_name.isEmpty())
        qDebug().noquote() << QString("[HttpResponse::AddVariable] var_name is empty");

      if (_vars.contains(var_name))
        qDebug().noquote() << QString("[HttpResponse::AddVariable] vars already contains %1")
                              .arg(var_name);
      return;
    }

  _vars.insert(var_name, var_content);

  /*qDebug().noquote() << QString("[HttpResponse::AddVariable] %1 inserted | vars.size = %2")
                        .arg(var_name).arg(_vars.size());*/
}


void HttpResponse::AddParam(const MK_HTTP::PARAMS &param_ref, const QByteArray &param_val)
{
  if (_params.contains(param_ref))
    return;

  _params.insert(param_ref, param_val);
}



bool HttpResponse::GenerateResponse(const QByteArray &model_name)
{
  qDebug().noquote() << QString("[HttpResponse::GenerateResponse] model_name = %1")
                        .arg(model_name);

  _result.clear();

  QString status_line = QString("%1 %2 %3\r\n")
      .arg(HttpResponse::GetHttpVers_AsText(*this))
      .arg((quint16)this->GetHttpCode())
      .arg(HttpResponse::GetHttpCode_AsText(*this));


  if (!model_name.isEmpty())
    this->ParseFromModel(model_name);


  /*if (!_parsed_content.isEmpty())
    this->AddParam(MK_HTTP::PARAMS::CONTENT_LENGTH,
                   QString("%1").arg(_parsed_content.size()).toUtf8());*/


  QHash<MK_HTTP::PARAMS,QByteArray>::const_iterator
      it = _params.constBegin(),
      ite = _params.constEnd();

  QString params_line;

  for (; it != ite; it++) {
      params_line.append( QString("%1 = %2\r\n")
                          .arg(_http_params_str.value(it.key()),
                               it.value()) );
    }

  _result.append(status_line.toUtf8());
  _result.append(params_line.toUtf8());

  if (!_parsed_content.isEmpty()) {
      _result.append(QString("%1 = %2\r\n\r\n")
                     .arg(_http_params_str.value(MK_HTTP::PARAMS::CONTENT_LENGTH))
                     .arg(_parsed_content.size())
                     .toUtf8());

      _result.append(_parsed_content);
    }

  qDebug() << "\n" << _result << "\n";

  return true;
}



void HttpResponse::ParseFromModel(const QByteArray &model_name)
{
  if (model_name.isEmpty())
    return;

  QHash<QByteArray, QByteArray>::const_iterator it = _vars.constBegin(), ite = _vars.constEnd();
  QByteArray content = _html_models.value(model_name);
  QRegularExpression reg;
  QRegularExpressionMatch match = reg.match(content);

  for (int n = 0; it != ite; it++, n++) {
      QString reg_pattern = QString("\\{\\{\%%1\%\\}\\}{1}").arg( _vars.key((*it)) );

      reg.setPattern(reg_pattern);

      QByteArrayList lines = content.split(QChar::SpecialCharacter::CarriageReturn);

      for (int n = 0; n < lines.size(); n++) {
          //QByteArray trimmed_line = lines.at(n).trimmed();

          //match = reg.match(trimmed_line);
          match = reg.match(lines.at(n));

          if (match.hasMatch()) {
              content = content.replace( QString("{{\%%1\%}}").arg(_vars.key((*it))).toUtf8(), (*it) );
            }
        }
    }

  content.replace("\r\n", "");

  _parsed_content = content;
}




void HttpResponse::SetHttpVersion(const MK_HTTP::VERSIONS &version) { _http_vers = version; }
void HttpResponse::SetHttpCode(const MK_HTTP::CODES &code) { _http_code = code; }
void HttpResponse::SetPageTitle(const QByteArray &title) { _page_title = title; }
void HttpResponse::SetParsedContent(const QByteArray &content) { _parsed_content = content; }
void HttpResponse::SetParams(const QHash<MK_HTTP::PARAMS, QByteArray> &params) { _params = params; }


const MK_HTTP::VERSIONS & HttpResponse::GetHttpVersion() const { return _http_vers; }
const MK_HTTP::CODES & HttpResponse::GetHttpCode() const { return _http_code; }
const QByteArray & HttpResponse::GetPageTitle() const { return _page_title; }
const QByteArray & HttpResponse::GetResult() const { return _result; }
const QHash<MK_HTTP::PARAMS, QByteArray> & HttpResponse::GetParams() const { return _params; }
