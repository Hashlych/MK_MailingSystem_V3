#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QObject>
#include <QHash>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "http_defs.h"


class HttpResponse : public QObject
{
  Q_OBJECT

public:
  explicit HttpResponse(QObject *parent = nullptr);
  HttpResponse(const HTTPSV::CODES &code, const HTTPSV::VERSIONS &version, QObject *parent = nullptr);
  HttpResponse(const HttpResponse &other);
  ~HttpResponse();

  HttpResponse * operator = (const HttpResponse &other);


  void AddVariable(const QByteArray &var_ref, const QByteArray &var_val);
  void ParseFromModel(const QByteArray &model_name);


  void SetCode(const HTTPSV::CODES &code);
  void SetVersion(const HTTPSV::VERSIONS &version);
  void SetBody(const QByteArray &body);
  void SetPacketDatas(const QByteArray &packetDatas);
  void SetContentType(const QByteArray &type);
  void SetContentCharset(const QByteArray &charset);
  void SetContentTransfertEncoding(const QByteArray &encoding);
  void SetAcceptRanges(const QByteArray &ranges);
  void SetBodyLength(const quint64 &length);

  const HTTPSV::CODES & GetCode() const;
  const HTTPSV::VERSIONS & GetVersion() const;
  const QByteArray & GetBody() const;
  const QByteArray & GetPacketDatas() const;
  const QByteArray & GetContentType() const;
  const QByteArray & GetContentCharset() const;
  const QByteArray & GetContentTransfertEncoding() const;
  const QByteArray & GetAcceptRanges() const;
  const quint64 & GetBodyLength() const;


private:
  HTTPSV::CODES     _code           = HTTPSV::CODES::NONE;
  HTTPSV::VERSIONS  _version        = HTTPSV::VERSIONS::NONE;
  QByteArray        _body           = "",
                    _packet_datas   = "",
                    _ct_type        = "",
                    _ct_charset     = "",
                    _ct_tf_encoding = "",
                    _accept_ranges  = "";
  quint64           _body_length    = 0;

  QHash<QByteArray, QByteArray> _vars;
};

#endif // HTTPRESPONSE_H
