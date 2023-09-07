#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QHash>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>

#include "http_defs.h"
#include "httpresponse.h"


class HttpRequest : public QObject
{
  Q_OBJECT

public:
  explicit HttpRequest(QObject *parent = nullptr);


  void DebugInfos();

  static HttpResponse BuildResponse(const HttpRequest &req);

  void AddParam(const MK_HTTP::PARAMS &param_ref, const QByteArray &param_val);


  void SetCommand(const MK_HTTP::CMDS &cmd);
  void SetHttpVersion(const MK_HTTP::VERSIONS &vers);
  void SetPage(const QByteArray &page);
  void SetGMLParams(const S_GML_Params &params);

  const MK_HTTP::CMDS & GetCommand() const;
  const MK_HTTP::VERSIONS & GetHttpVersion() const;
  const QByteArray & GetPage() const;
  const S_GML_Params & GetGMLParams() const;


private:
  MK_HTTP::CMDS _cmd = MK_HTTP::CMDS::NONE;
  MK_HTTP::VERSIONS _http_vers = MK_HTTP::VERSIONS::NONE;
  QByteArray _page;
  QHash<MK_HTTP::PARAMS, QByteArray> _params;
  S_GML_Params _gml_params;
};


#endif // HTTPREQUEST_H
