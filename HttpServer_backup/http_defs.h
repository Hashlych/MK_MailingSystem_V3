#ifndef HTTP_DEFS_H
#define HTTP_DEFS_H

#include <QObject>


namespace MK_HTTP {

  enum class VERSIONS {
    NONE = 0,
    V1_1,
    V2_0,
  };

  enum class CMDS {
    NONE = 0,
    GET,
    POST,
    HEAD,
  };

  enum class CODES {
    NONE          = 0,
    OK            = 200,
    NOT_FOUND     = 404,
    NOT_MODIFIED  = 304,
    BAD_REQUEST   = 400,
  };

  enum class PARAMS {
    NONE = 0,
    ACCEPT_LANGUAGE,
    ACCEPT_ENCODING,
    ACCEPT,
    USER_AGENT,
    DATE,
    SERVER,
    LAST_MODIFIED,
    CONTENT_TYPE,
    CONTENT_LENGTH,
    CONNECTION,
    KEEP_ALIVE,
    HOST,
  };

}


typedef struct S_GML_Params {
  QByteArray state, code, scope;
} S_GML_Params;


#endif // HTTP_DEFS_H
