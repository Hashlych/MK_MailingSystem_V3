#ifndef HTTP_DEFS_H
#define HTTP_DEFS_H

#include <QObject>
#include <QHash>

#define G_CL_ID     ""
#define G_CL_SECRET ""


namespace HTTPSV {

  enum class CMDS {
    NONE = 0,
    GET,
    POST,
    HEAD,
  };


  enum class CODES {
    NONE          = 0,
    OK            = 200,
    NOT_MODIFIED  = 304,
    NOT_FOUND     = 404,
  };


  enum class VERSIONS {
    NONE = 0,
    V1,
    V2,
  };

}


extern QHash<HTTPSV::CMDS     , QByteArray>  _http_cmds;
extern QHash<HTTPSV::CODES    , QByteArray>  _http_codes;
extern QHash<HTTPSV::VERSIONS , QByteArray>  _http_versions;

extern QHash<QByteArray       , QByteArray>  _html_models;


#endif // HTTP_DEFS_H
