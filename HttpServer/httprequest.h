#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QFile>
#include <QDateTime>

#include "http_defs.h"
#include "httpresponse.h"


class HttpRequest : public QObject
{
  Q_OBJECT

public:
  explicit HttpRequest(QObject *parent = nullptr);
  HttpRequest(const HTTPSV::CMDS &cmd, const HTTPSV::VERSIONS &version, const QByteArray &page,
              QObject *parent = nullptr);
  HttpRequest(const HttpRequest &other);
  ~HttpRequest();

  HttpRequest * operator = (const HttpRequest &other);


  void ExtractFromStatusLine(const QByteArray &status_line);
  HttpResponse BuildResponse();


  void SetCMD(const HTTPSV::CMDS &cmd);
  void SetVersion(const HTTPSV::VERSIONS &version);
  void SetPage(const QByteArray &page);

  const HTTPSV::CMDS & GetCMD() const;
  const HTTPSV::VERSIONS & GetVersion() const;
  const QByteArray & GetPage() const;


private:
  HTTPSV::CMDS      _cmd      = HTTPSV::CMDS::NONE;
  HTTPSV::VERSIONS  _version  = HTTPSV::VERSIONS::NONE;
  QByteArray        _page     = "";


};

#endif // HTTPREQUEST_H
