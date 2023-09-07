#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QHash>
#include <QThread>
#include <QDateTime>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>

#include "Contacts/ctsmanager.h"
#include "Contacts/ctsmanagerie.h"
#include "Emails/emailsmanager.h"
#include "Emails/SMTPClient.h"
#include "Logs/logsmanager.h"
#include "QtAES/qaesencryption.h"


enum class DBQR_REFS {
  NONE            = 0x0000,

  CT_INSERT       = 0x0001,
  CT_SAVE         = 0x0002,
  CT_LOAD         = 0x0003,
  CT_LOAD_ALL     = 0x0004,
  CT_DEL          = 0x0005,
  CT_CREATE_TBL   = 0x0006,

  GRP_INSERT      = 0x0010,
  GRP_SAVE        = 0x0011,
  GRP_LOAD        = 0x0012,
  GRP_LOAD_ALL    = 0x0013,
  GRP_DEL         = 0x0014,
  GRP_CREATE_TBL  = 0x0015,

  ML_INSERT       = 0x0020,
  ML_SAVE         = 0x0021,
  ML_LOAD         = 0x0022,
  ML_LOAD_ALL     = 0x0023,
  ML_DEL          = 0x0024,
  ML_CREATE_TBL   = 0x0025,

  IDT_INSERT      = 0x0030,
  IDT_SAVE        = 0x0031,
  IDT_LOAD        = 0x0032,
  IDT_LOAD_ALL    = 0x0033,
  IDT_DEL         = 0x0034,
  IDT_CREATE_TBL  = 0x0035,
};



class DBManager : public QObject
{
  Q_OBJECT

public:
  explicit DBManager(QObject *parent = nullptr);
  ~DBManager();


  void Set_CTSMG_Thread(QThread *th);
  void Set_MLSMG_Thread(QThread *th);
  void Set_Ref_CTSManager(QSharedPointer<CTSManager*> ref);
  void Set_Ref_SMTPClient(QSharedPointer<SMTPClient*> ref);
  void Set_Ref_Encryption(QAESEncryption *ref);

  QThread * Get_CTSMG_Thread() const;
  QThread * Get_MLSMG_Thread() const;
  QSharedPointer<CTSManager*> Get_Ref_CTSManager() const;
  QSharedPointer<SMTPClient*> Get_Ref_SMTPClient() const;
  QAESEncryption * Get_Ref_Encryption() const;


signals:
  void SI_AddLog(const LOG_TYPES &type, const QByteArray &text, const QDateTime &dt = QDateTime::currentDateTime());

  // contacts:
  void SI_CT_Inserted(Contact *ct, const quint32 &grp_id = 0);
  void SI_CT_Saved(QSharedPointer<Contact*> ct);
  void SI_CT_Loaded(Contact *ct, const quint32 &grp_id);
  void SI_CT_Deleted(QSharedPointer<Contact*> ct);

  // groups:
  void SI_GRP_Inserted(CTGroup *grp);
  void SI_GRP_Saved(QSharedPointer<CTGroup*> grp);
  void SI_GRP_Loaded(CTGroup *grp);
  void SI_GRP_Deleted(QSharedPointer<CTGroup*> grp);

  // emails:
  void SI_ML_Inserted(EmailObject *ml);
  void SI_ML_Saved(QSharedPointer<EmailObject*> ml);
  void SI_ML_Loaded(EmailObject *ml);
  void SI_ML_Deleted(QSharedPointer<EmailObject*> ml);

  // identities:
  void SI_IDT_Inserted(Identity *idt);
  void SI_IDT_Saved(QSharedPointer<Identity*> idt);
  void SI_IDT_Loaded(Identity *idt);
  void SI_IDT_Deleted(QSharedPointer<Identity*> idt);


public slots:
  void SL_Initialize();
  void SL_ConnectToLogsMG(LogsManager *mg);

  // contacts:
  void SL_CT_Insert(const QByteArray &firstName, const QByteArray &lastName, const QByteArray &email,
                    const QSharedPointer<CTGroup*> grp);
  void SL_CT_Insert(const S_CT_Datas ct_datas);

  void SL_CT_Save(QSharedPointer<Contact*> ct);
  void SL_CT_Load(const quint32 &id);
  void SL_CT_LoadAll();
  void SL_CT_Delete(QSharedPointer<Contact*> ct);

  void SL_CTS_Insert(QVector<S_CT_Datas> cts_datas);


  // groups:
  void SL_GRP_Insert(const QByteArray &name, const QByteArray &desc);
  void SL_GRP_Insert(const S_GRP_Datas grp_datas);

  void SL_GRP_Save(QSharedPointer<CTGroup*> grp);
  void SL_GRP_Load(const quint32 &id);
  void SL_GRP_LoadAll();
  void SL_GRP_Delete(QSharedPointer<CTGroup*> grp);


  // emails:
  void SL_ML_Insert(const QByteArray &subject, const QByteArray &content, const quint64 &dt_exec,
                    QVector<QSharedPointer<CTGroup*>> groups);
  void SL_ML_Save(QSharedPointer<EmailObject*> ml);
  void SL_ML_Load(const quint32 &id);
  void SL_ML_LoadAll();
  void SL_ML_Delete(QSharedPointer<EmailObject*> ml);


  // identities:
  void SL_IDT_Insert(const QByteArray &name, const QByteArray &addr, const QByteArray &code,
                     const QByteArray &tkn_refres);
  void SL_IDT_Insert(const S_IDT_Datas idt_datas);
  void SL_IDT_Save(QSharedPointer<Identity*> idt);
  void SL_IDT_Load(const quint32 &id);
  void SL_IDT_LoadAll();
  void SL_IDT_Delete(QSharedPointer<Identity*> idt);


private:
  void CreateQueries();
  void CreateTables();

  qint16 ContactExists(const S_CT_Datas &ct_datas);


  bool  _init = false;
  QThread *_cts_mg_thread = nullptr,
          *_mls_mg_thread = nullptr;
  QSharedPointer<CTSManager*> _p_cts_mg;
  QHash<DBQR_REFS,QString> _queries;
  QAESEncryption *_encryption = nullptr;
  QSharedPointer<SMTPClient*> _p_smtpcl;

  QSqlDatabase  _db     = QSqlDatabase::addDatabase("QSQLITE");
  QSqlQuery     _query  = QSqlQuery(_db);
};

#endif // DBMANAGER_H
