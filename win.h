#ifndef WIN_H
#define WIN_H

#include <QMainWindow>
#include <QGridLayout>
#include <QThread>
#include <QTimer>
#include <QMenu>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QDateTimeEdit>
#include <QProcess>

#include "QtAES/qaesencryption.h"

#include "Database/dbmanager.h"

#include "Contacts/ctsmanager.h"
#include "Contacts/ctsmanagerie.h"
#include "Contacts/ctsmanagerui.h"

#include "Emails/identitiesmanager.h"
#include "Emails/identitiesui.h"
#include "Emails/emailsmanager.h"
#include "Emails/emailslistui.h"
#include "Emails/emailcreateform.h"
#include "Emails/mlsprocessor.h"
#include "Emails/SMTPClient.h"

#include "Logs/logsmanager.h"
#include "Logs/logsui.h"

#include "HttpServer/http_defs.h"
#include "HttpServer/httpserver.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Win; }
QT_END_NAMESPACE


class Win : public QMainWindow
{
  Q_OBJECT

public:
  Win(QWidget *parent = nullptr);
  ~Win();


signals:
  // Trigger DBManager initialization:
  void SI_InitDB();
  void SI_Init_MLSProcessor();
  void SI_Init_SMTP(QAESEncryption *encrytion);
  void SI_Init_MLSManager();
  void SI_Init_LGSManager();
  void SI_Init_HttpSVManager();

  void SI_ReqUpdate_MLSList_Timers();
  void SI_ReqUpdate_MLSList_Item(QSharedPointer<EmailObject*> pml);

  void SI_AddLog(const LOG_TYPES &type, const QByteArray &text,
                 const QDateTime &dt_msecs = QDateTime::currentDateTime());

  // not used:
  void SI_CTGroup_AddContact(const QByteArray &firstName, const QByteArray &lastName, const QByteArray &email,
                             const quint32 &id);

  // not used:
  void SI_CTSMG_AddGroup(const QByteArray &name, const QByteArray &desc, const quint32 &id);


  // Request insert contact into database:
  void SI_DB_CT_Insert(const QByteArray &firstName, const QByteArray &lastName, const QByteArray &email,
                       const QSharedPointer<CTGroup*> grp);

  // Request load all contacts from database:
  void SI_DB_CT_LoadAll();


  // Request insert group into database:
  void SI_DB_GRP_Insert(const QByteArray &name, const QByteArray &desc);

  // Request load all groups from database:
  void SI_DB_GRP_LoadAll();


  void SI_DB_ML_Insert(const QByteArray &subject, const QByteArray &content, const quint64 &dt_exec,
                       QVector<QSharedPointer<CTGroup*>> groups);
  void SI_DB_ML_LoadAll();
  void SI_DB_ML_Delete(QSharedPointer<EmailObject*> pml);


  void SI_ReqImportContacts(QSharedPointer<CTGroup*> pgrp, const QByteArray &path);
  void SI_ReqExportContacts(QVector<QSharedPointer<Contact*>> cts, const QByteArray &path);

  void SI_ReqImportGroups(const QByteArray &path);
  void SI_ReqExportGroups(QVector<QSharedPointer<CTGroup*>> grps, const QByteArray &path);


  void SI_MLS_AddEmail(const QByteArray &subject, const QByteArray &content, const quint64 &dt_exec = 0,
                       const quint32 &id = 0);
  void SI_MLS_AddEmail(EmailObject *ml);


  void SI_DB_IDT_LoadAll();


private slots:
  // debugging purposes:
  void SL_CTGroup_ContactAdded(const QSharedPointer<Contact*> &ct);
  void SL_CTSMG_GroupAdded(const QSharedPointer<CTGroup*> &grp);
  void SL_ReqNT_SendEmail(QSharedPointer<EmailObject*> pml);

  void SL_MailSent(QSharedPointer<MailPKT*> ml_pkt);

  void Test();

  void OnTM_Clock();


  void SL_OnReq_CTSUI_NewGroup();

  void SL_MNAC_ImportContacts();
  void SL_MNAC_ExportContacts();
  void SL_MNAC_ImportGroups();
  void SL_MNAC_ExportGroups();
  void SL_MNAC_NewEmail();

  void OnBT_ImportContacts_Ok();
  void OnBT_ImportContacts_Cancel();

  void OnBT_MLS_NewEmail_Ok();
  void OnBT_MLS_NewEmail_Cancel();


  void OnCTSManagerIE_ContactsExported(QVector<QSharedPointer<Contact*>> cts);
  void OnCTSManagerIE_GroupsExported(QVector<QSharedPointer<CTGroup*>> grps);


  void SL_MLS_EmailAdded(QSharedPointer<EmailObject*> ml);

  void SL_ConnectObjects();
  void SL_InitialLoadDB();


private:
  bool BuildUI();
  bool BuildMenus();

  // create managers:
  void Create_DBManager();
  void Create_CTSManager();
  void Create_EmailsManager();
  void Create_LGSManager();
  void Create_HttpSVManager();

  // connect signals to slots for managers:
  void Connect_SI_SL_DBManager();
  void Connect_SI_SL_CTSManager();
  void Connect_SI_SL_EmailsManager();
  void Connect_SI_SL_HttpSVManager();

  void Connect_DBManager();


  // database manager:
  struct {
    QThread   *th = nullptr;
    DBManager *mg = nullptr;
  } _db;

  // contacts manager:
  struct {
    QThread       *th = nullptr;
    CTSManager    *mg = nullptr;
    CTSManagerIE  *ie = nullptr;
  } _cts;

  // emails manager:
  struct {
    QThread           *th     = nullptr;
    EmailsManager     *mg     = nullptr;
    MLSProcessor      *mlproc = nullptr;
    SMTPClient        *smtpcl = nullptr;
    IdentitiesManager *idts   = nullptr;
  } _emails;

  // http server manager:
  struct {
    QThread     *th = nullptr;
    HttpServer  *mg = nullptr;
  } _http_sv;

  // logs manager:
  struct {
    QThread     *th = nullptr;
    LogsManager *mg = nullptr;
  } _logs;


  // main menu:
  struct {
    QMenu *root = nullptr;
    QAction
      *quit_app     = nullptr,
      *new_email    = nullptr;

    struct {
      QMenu *rt = nullptr;
      QAction *ac_import = nullptr, *ac_export = nullptr;
    } cts_menu, grps_menu;

  } _mn_main;


  QAESEncryption _encryption;
  QTimer         *_tm_clock = nullptr;


  Ui::Win *ui;
  QGridLayout   *_mlay    = nullptr;
  CTSManagerUI  *_cts_ui  = nullptr;
  EmailsListUI  *_mls_ui  = nullptr;
  LogsUI        *_logs_ui = nullptr;
  IdentitiesUI  *_idts_ui = nullptr;

  QLabel        *_lb_clock = nullptr;
};

#endif // WIN_H
