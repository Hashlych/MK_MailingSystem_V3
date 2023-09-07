#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QObject>
#include <QHash>
#include <QFile>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "http_defs.h"

extern QHash<MK_HTTP::VERSIONS, QByteArray> _http_vers_str;
extern QHash<MK_HTTP::CODES, QByteArray>    _http_codes_str;
extern QHash<MK_HTTP::CMDS, QByteArray>     _http_cmds_str;
extern QHash<MK_HTTP::PARAMS, QByteArray>   _http_params_str;
extern QHash<QByteArray, QByteArray>        _html_models;


class HttpResponse : public QObject
{
  Q_OBJECT

public:
  explicit HttpResponse(QObject *parent = nullptr);
  HttpResponse(const HttpResponse &other);
  ~HttpResponse();

  HttpResponse * operator = (const HttpResponse &other);

  static QByteArray GetHttpVers_AsText(const HttpResponse &rsp);
  static QByteArray GetHttpCode_AsText(const HttpResponse &rsp);

  static QByteArray LoadHtmlModel(const QString &model_path);


  void DebugInfos();
  void AddVariable(const QByteArray &var_name, const QByteArray &var_content);
  void AddParam(const MK_HTTP::PARAMS &param_ref, const QByteArray &param_val);

  bool GenerateResponse(const QByteArray &model_name);


  void SetHttpVersion(const MK_HTTP::VERSIONS &version);
  void SetHttpCode(const MK_HTTP::CODES &code);
  void SetPageTitle(const QByteArray &title);
  void SetParsedContent(const QByteArray &content);
  void SetParams(const QHash<MK_HTTP::PARAMS, QByteArray> &params);

  const MK_HTTP::VERSIONS & GetHttpVersion() const;
  const MK_HTTP::CODES    & GetHttpCode() const;
  const QByteArray        & GetPageTitle() const;
  const QByteArray        & GetResult() const;
  const QHash<MK_HTTP::PARAMS, QByteArray> & GetParams() const;


private:
  void ParseFromModel(const QByteArray &model_name);

  MK_HTTP::VERSIONS _http_vers = MK_HTTP::VERSIONS::NONE;
  MK_HTTP::CODES    _http_code = MK_HTTP::CODES::NONE;
  QByteArray  _page_title     = "",
              _parsed_content = "",
              _result         = "";

  QHash<QByteArray, QByteArray>      _vars;
  QHash<MK_HTTP::PARAMS, QByteArray> _params;
};

#endif // HTTPRESPONSE_H
