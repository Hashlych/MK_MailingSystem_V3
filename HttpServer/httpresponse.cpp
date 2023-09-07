#include "httpresponse.h"


// Constructors
// Default:
HttpResponse::HttpResponse(QObject *parent)
  : QObject{parent}
{
}


// Variant 01:
HttpResponse::HttpResponse(const HTTPSV::CODES &code, const HTTPSV::VERSIONS &version, QObject *parent)
  : QObject{parent},
    _code(code), _version(version)
{
}


// Copy:
HttpResponse::HttpResponse(const HttpResponse &other)
  : QObject{other.parent()}
{
}



// Destructor:
HttpResponse::~HttpResponse()
{
}



// Operators
HttpResponse * HttpResponse::operator = (const HttpResponse &other)
{
  this->SetCode(other.GetCode());
  this->SetVersion(other.GetVersion());
  this->SetBody(other.GetBody());
  this->SetContentType(other.GetContentType());
  this->SetContentCharset(other.GetContentCharset());
  this->SetContentTransfertEncoding(other.GetContentTransfertEncoding());
  this->SetAcceptRanges(other.GetAcceptRanges());
  this->SetBodyLength(other.GetBodyLength());
  this->SetPacketDatas(other.GetPacketDatas());

  return this;
}



// ParseFromModel:
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
          match = reg.match(lines.at(n));

          if (match.hasMatch())
            content = content.replace( QString("{{\%%1\%}}").arg(_vars.key((*it))).toUtf8(), (*it) );
        }
    }

  content.replace("\r\n", "");

  _body = content;
  _body_length = _body.size();
}



// AddVariable:
void HttpResponse::AddVariable(const QByteArray &var_ref, const QByteArray &var_val)
{
  if (var_ref.isEmpty() || _vars.contains(var_ref))
    return;

  _vars.insert(var_ref, var_val);
}



// Mutators:
void HttpResponse::SetCode(const HTTPSV::CODES &code) { _code = code; }
void HttpResponse::SetVersion(const HTTPSV::VERSIONS &version) { _version = version; }
void HttpResponse::SetBody(const QByteArray &body) { _body = body; }
void HttpResponse::SetPacketDatas(const QByteArray &packetDatas) { _packet_datas = packetDatas; }
void HttpResponse::SetContentType(const QByteArray &type) { _ct_type = type; }
void HttpResponse::SetContentCharset(const QByteArray &charset) { _ct_charset = charset; }
void HttpResponse::SetContentTransfertEncoding(const QByteArray &encoding) { _ct_tf_encoding = encoding; }
void HttpResponse::SetAcceptRanges(const QByteArray &ranges) { _accept_ranges = ranges; }
void HttpResponse::SetBodyLength(const quint64 &length) { _body_length = length; }


// Accessors:
const HTTPSV::CODES & HttpResponse::GetCode() const { return _code; }
const HTTPSV::VERSIONS & HttpResponse::GetVersion() const { return _version; }
const QByteArray & HttpResponse::GetBody() const { return _body; }
const QByteArray & HttpResponse::GetPacketDatas() const { return _packet_datas; }
const QByteArray & HttpResponse::GetContentType() const { return _ct_type; }
const QByteArray & HttpResponse::GetContentCharset() const { return _ct_charset; }
const QByteArray & HttpResponse::GetContentTransfertEncoding() const { return _ct_tf_encoding; }
const QByteArray & HttpResponse::GetAcceptRanges() const { return _accept_ranges; }
const quint64 & HttpResponse::GetBodyLength() const { return _body_length; }
