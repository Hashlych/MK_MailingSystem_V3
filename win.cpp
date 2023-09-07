#include "win.h"
#include "ui_win.h"



QHash<HTTPSV::CMDS, QByteArray> _http_cmds = {
  QPair<HTTPSV::CMDS, QByteArray>(HTTPSV::CMDS::NONE, ""),
  QPair<HTTPSV::CMDS, QByteArray>(HTTPSV::CMDS::GET , "GET"),
  QPair<HTTPSV::CMDS, QByteArray>(HTTPSV::CMDS::POST, "POST"),
  QPair<HTTPSV::CMDS, QByteArray>(HTTPSV::CMDS::HEAD, "HEAD"),
};

QHash<HTTPSV::VERSIONS, QByteArray> _http_versions = {
  QPair<HTTPSV::VERSIONS, QByteArray>(HTTPSV::VERSIONS::NONE, ""),
  QPair<HTTPSV::VERSIONS, QByteArray>(HTTPSV::VERSIONS::V1  , "HTTP/1.1"),
  QPair<HTTPSV::VERSIONS, QByteArray>(HTTPSV::VERSIONS::V2  , "HTTP/2"),
};

QHash<HTTPSV::CODES, QByteArray> _http_codes = {
  QPair<HTTPSV::CODES, QByteArray>(HTTPSV::CODES::NONE, "0 undefined"),
  QPair<HTTPSV::CODES, QByteArray>(HTTPSV::CODES::OK, "200 OK"),
  QPair<HTTPSV::CODES, QByteArray>(HTTPSV::CODES::NOT_MODIFIED, "304 Not Modified"),
  QPair<HTTPSV::CODES, QByteArray>(HTTPSV::CODES::NOT_FOUND, "404 Not Found"),
};


QHash<QByteArray, QByteArray> _html_models;



/* ---------------- Constructors ---------------- */
// Default:
Win::Win(QWidget *parent)
  : QMainWindow(parent)
  , _encryption(QAESEncryption(QAESEncryption::AES_256, QAESEncryption::ECB))
  , ui(new Ui::Win)
{
  ui->setupUi(this);

  // Set window properties:
  this->setWindowTitle("MK_MailingSystem");
  this->resize(1920, 1080);


  // Create managers:
  this->Create_LGSManager();
  this->Create_CTSManager();
  this->Create_EmailsManager();
  this->Create_DBManager();
  this->Create_HttpSVManager();


  // Build UI & Menus:
  if (!this->BuildUI() || !this->BuildMenus())
    this->close();

  QTimer::singleShot(20, this, &Win::SL_ConnectObjects);

  _tm_clock = new QTimer(this);
  _tm_clock->setInterval(1000);

  connect(_tm_clock, &QTimer::timeout, this, &Win::OnTM_Clock);

  _tm_clock->start();


  QTimer::singleShot(5000, this, &Win::Test);
}



/* ---------------- Destructor ---------------- */
Win::~Win()
{
  // -------- Stop and destroy threads -------- //
  // HttpServer:
  if (_http_sv.th && _http_sv.th->isRunning()) {
      _http_sv.th->quit();
      _http_sv.th->wait();

      delete _http_sv.th;
    }

  // Emails:
  if (_emails.th && _emails.th->isRunning()) {
      _emails.th->quit();
      _emails.th->wait();

      delete _emails.th;
      _emails.th = nullptr;
    }

  // Contacts:
  if (_cts.th && _cts.th->isRunning()) {
      _cts.th->quit();
      _cts.th->wait();

      delete _cts.th;
      _cts.th = nullptr;
    }

  // Database:
  if (_db.th && _db.th->isRunning()) {
      _db.th->quit();
      _db.th->wait();

      delete _db.th;
      _db.th = nullptr;
    }

  // Logs:
  if (_logs.th && _logs.th->isRunning()) {
      _logs.th->quit();
      _logs.th->wait();

      delete _logs.th;
      _logs.th = nullptr;
    }
  // -------- End - Stop and destroy threads -------- //

  if (_tm_clock && _tm_clock->isActive())
    _tm_clock->stop();


  delete ui;
}



/* ---------------- BuildUI ---------------- */
bool Win::BuildUI()
{
  // Main layout:
  if (!(_mlay = new QGridLayout()))
    return false;
  else
    this->centralWidget()->setLayout(_mlay);


  // Contacts UI:
  if ( !(_cts_ui = new CTSManagerUI()) || !_cts_ui->BuildUI() )
    return false;

  // Emails UI:
  if ( !(_mls_ui = new EmailsListUI()) || !_mls_ui->BuildUI() )
    return false;

  // Logs UI:
  if ( !(_logs_ui = new LogsUI()) || !_logs_ui->BuildUI() )
    return false;

  // Identities UI:
  if ( !(_idts_ui = new IdentitiesUI()) || !_idts_ui->BuildUI() )
    return false;

  // Clock UI:
  if ( !(_lb_clock = new QLabel()) )
    return false;

  _lb_clock->setText( QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss") );


  // Add widgets to main layout:
  _mlay->addWidget(_lb_clock, 0, 0, 1, 1);

  _mlay->addWidget(_cts_ui, 1, 0, 1, 1);
  _mlay->addWidget(_idts_ui, 2, 0, 1, 1);

  _mlay->addWidget(_mls_ui, 1, 1, 2, 1);

  _mlay->addWidget(_logs_ui, 3, 0, 1, 2);


  return true;
}



/* ---------------- BuildMenus ---------------- */
bool Win::BuildMenus()
{
  // ---- Main Menu ---- //

  // build objects:
  _mn_main.root         = new QMenu(tr("Menu"));

  _mn_main.quit_app     = new QAction(tr("&Quit")           , _mn_main.root);
  _mn_main.new_email    = new QAction(tr("Create Email")    , _mn_main.root);


  _mn_main.cts_menu.rt = new QMenu(tr("Contacts"));
  _mn_main.cts_menu.ac_import = new QAction(tr("Import"), _mn_main.cts_menu.rt);
  _mn_main.cts_menu.ac_export = new QAction(tr("Export"), _mn_main.cts_menu.rt);

  _mn_main.grps_menu.rt = new QMenu(tr("Groups"));
  _mn_main.grps_menu.ac_import = new QAction(tr("Import"), _mn_main.grps_menu.rt);
  _mn_main.grps_menu.ac_export = new QAction(tr("Export"), _mn_main.grps_menu.rt);


  // set actions shortcuts:
  _mn_main.quit_app->setShortcut(QKeySequence("CTRL+Q"));


  // connect actions to slots:
  connect(_mn_main.quit_app   , &QAction::triggered, this, &Win::close);
  connect(_mn_main.new_email, &QAction::triggered, this, &Win::SL_MNAC_NewEmail);

  // (sub-menu: contacts)
  connect(_mn_main.cts_menu.ac_import, &QAction::triggered, this, &Win::SL_MNAC_ImportContacts);
  connect(_mn_main.cts_menu.ac_export, &QAction::triggered, this, &Win::SL_MNAC_ExportContacts);

  // (sub-menu: groups)
  connect(_mn_main.grps_menu.ac_import, &QAction::triggered, this, &Win::SL_MNAC_ImportGroups);
  connect(_mn_main.grps_menu.ac_export, &QAction::triggered, this, &Win::SL_MNAC_ExportGroups);


  // add actions to menu:
  // (sub-menu: contacts)
  _mn_main.cts_menu.rt->addAction(_mn_main.cts_menu.ac_import);
  _mn_main.cts_menu.rt->addAction(_mn_main.cts_menu.ac_export);

  // (sub-menu: groups)
  _mn_main.grps_menu.rt->addAction(_mn_main.grps_menu.ac_import);
  _mn_main.grps_menu.rt->addAction(_mn_main.grps_menu.ac_export);

  // (main menu)
  _mn_main.root->addAction(_mn_main.new_email);

  _mn_main.root->addSeparator();

  _mn_main.root->addMenu(_mn_main.cts_menu.rt);
  _mn_main.root->addMenu(_mn_main.grps_menu.rt);

  _mn_main.root->addSeparator();

  _mn_main.root->addAction(_mn_main.quit_app);

  // add main menu to menu bar:
  this->menuBar()->addMenu(_mn_main.root);

  return true;
}



void Win::OnTM_Clock()
{
  _lb_clock->setText( QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss") );

  // request update time remaining for emails list:
  emit SI_ReqUpdate_MLSList_Timers();
}



/* ---------------- SL_ConnectObjects ---------------- */
void Win::SL_ConnectObjects()
{
  // ---- Trigger objects initialization ---- //
  connect(this, &Win::SI_InitDB             , _db.mg          , &DBManager::SL_Initialize     );
  connect(this, &Win::SI_Init_SMTP          , _emails.smtpcl  , &SMTPClient::Init             );
  connect(this, &Win::SI_Init_MLSProcessor  , _emails.mlproc  , &MLSProcessor::SL_Initialize  );
  connect(this, &Win::SI_Init_MLSManager    , _emails.mg      , &EmailsManager::SL_Initialize );
  connect(this, &Win::SI_Init_LGSManager    , _logs.mg        , &LogsManager::SL_Initialize   );


  // ---- Transmit signals from Win to EmailsListUI ---- //
  // request UI to update emails timers display:
  connect(this, &Win::SI_ReqUpdate_MLSList_Timers, _mls_ui, &EmailsListUI::SL_OnReqUpdate_Timers);

  connect(this, &Win::SI_ReqUpdate_MLSList_Item, _mls_ui, &EmailsListUI::SL_OnReqUpdate_Item);

  // ---- End Transmit signals from Win to EmailsListUI ---- //



  // ---- Logs ---- //
  // send logs from LogsManager to display:
  connect(_logs.mg, &LogsManager::SI_ReqDisplayLogs, _logs_ui, &LogsUI::SL_AddToDisplay);

  connect(this    , &Win::SI_AddLog           , _logs.mg, &LogsManager::SL_AddLog);
  connect(_db.mg  , &DBManager::SI_AddLog     , _logs.mg, &LogsManager::SL_AddLog);
  connect(_cts.mg , &CTSManager::SI_AddLog    , _logs.mg, &LogsManager::SL_AddLog);
  connect(_cts.ie , &CTSManagerIE::SI_AddLog  , _logs.mg, &LogsManager::SL_AddLog);

  // ---- End Logs ---- //



  // ---- HttpServer ---- //
  connect(this, &Win::SI_Init_HttpSVManager, _http_sv.mg, &HttpServer::SL_Initialize);
  // ---- End HttpServer ---- //



  // ---- Requests from Win to DB ---- //

  // Contacts (insert):
  connect(this, &Win::SI_DB_CT_Insert, _db.mg,
          QOverload<const QByteArray&,
                    const QByteArray&,
                    const QByteArray&,
                    const QSharedPointer<CTGroup*>>::of(&DBManager::SL_CT_Insert) );

  // Contacts (LoadAll):
  connect(this, &Win::SI_DB_CT_LoadAll, _db.mg, &DBManager::SL_CT_LoadAll );

  // Emails (Insert):
  connect(this, &Win::SI_DB_ML_Insert , _db.mg, &DBManager::SL_ML_Insert  );

  // Emails (LoadAll):
  connect(this, &Win::SI_DB_ML_LoadAll, _db.mg, &DBManager::SL_ML_LoadAll );

  // Emails (Delete):
  connect(this, &Win::SI_DB_ML_Delete , _db.mg, &DBManager::SL_ML_Delete  );

  // Groups (Insert):
  connect(this, &Win::SI_DB_GRP_Insert, _db.mg,
          QOverload<const QByteArray&,
                    const QByteArray&>::of(&DBManager::SL_GRP_Insert) );

  // Groups (LoadAll):
  connect(this, &Win::SI_DB_GRP_LoadAll , _db.mg, &DBManager::SL_GRP_LoadAll);

  // Identities (LoadAll):
  connect(this, &Win::SI_DB_IDT_LoadAll, _db.mg, &DBManager::SL_IDT_LoadAll);

  // ---- End Requests to DB ---- //



  // ---- Responses from DB ---- //

  // Contacts (Inserted):
  connect(_db.mg, &DBManager::SI_CT_Inserted, _cts.mg, &CTSManager::SL_GRP_AddContact);

  // Contacts (Loaded):
  connect(_db.mg, &DBManager::SI_CT_Loaded  , _cts.mg, &CTSManager::SL_GRP_AddContact);

  // Contacts (Deleted):
  connect(_db.mg, &DBManager::SI_CT_Deleted , _cts.mg, &CTSManager::SL_GRP_DelContact);

  // Emails (Inserted):
  connect(_db.mg, &DBManager::SI_ML_Inserted, _emails.mg,
          QOverload<EmailObject*>::of(&EmailsManager::SL_AddEmail) );

  // Emails (Loaded):
  connect(_db.mg, &DBManager::SI_ML_Loaded, _emails.mg,
          QOverload<EmailObject*>::of(&EmailsManager::SL_AddEmail) );

  // Emails (Deleted):
  connect(_db.mg, &DBManager::SI_ML_Deleted, _emails.mg, &EmailsManager::SL_DelEmail);

  // Groups (Inserted):
  connect(_db.mg, &DBManager::SI_GRP_Inserted, _cts.mg,
          QOverload<CTGroup*>::of(&CTSManager::SL_AddGroup) );

  // Groups (Loaded):
  connect(_db.mg, &DBManager::SI_GRP_Loaded, _cts.mg,
          QOverload<CTGroup*>::of(&CTSManager::SL_AddGroup) );

  // Groups (Deleted):
  connect(_db.mg, &DBManager::SI_GRP_Deleted, _cts.mg, &CTSManager::SL_RemoveGroup);


  // Identities (Inserted):
  connect(_db.mg, &DBManager::SI_IDT_Inserted, _emails.idts,
          QOverload<Identity*>::of(&IdentitiesManager::SL_AddIdentity));

  // Identities (Loaded):
  connect(_db.mg, &DBManager::SI_IDT_Loaded, _emails.idts,
          QOverload<Identity*>::of(&IdentitiesManager::SL_AddIdentity));


  // ---- End Responses from DB ---- //



  // ---- Transmit DB requests from Contacts UI to DB manager ---- //
  // Groups (delete):
  connect(_cts_ui, &CTSManagerUI::SI_Req_DelGroup   , _db.mg, &DBManager::SL_GRP_Delete     );

  // Contacts (delete):
  connect(_cts_ui, &CTSManagerUI::SI_Req_DelContact , _db.mg, &DBManager::SL_CT_Delete      );

  // Email (delete):
  connect(_mls_ui, &EmailsListUI::SI_ReqDelEmail, _db.mg, &DBManager::SL_ML_Delete);

  // ---- End Transmit DB requests from Contacts UI to DB manager ---- //



  // ---- Transmit CTSManagerIE requests to DB manager ---- //
  // Contacts (Imported):
  connect(_cts.ie, &CTSManagerIE::ContactsImported,
          _db.mg, QOverload<QVector<S_CT_Datas>>::of(&DBManager::SL_CTS_Insert));

  // Contacts (Exported):
  connect(_cts.ie, &CTSManagerIE::ContactsExported, this, &Win::OnCTSManagerIE_ContactsExported );

  // Groups (Exported):
  connect(_cts.ie, &CTSManagerIE::GroupsExported  , this, &Win::OnCTSManagerIE_GroupsExported   );

  // ---- End - Transmit CTSManagerIE requests to DB manager ---- //



  // ---- Transmit CTSManager signals to CTSManagerUI ---- //
  // Contacts (Added):
  connect(_cts.mg, &CTSManager::SI_GRP_ContactAdded   , _cts_ui, &CTSManagerUI::SL_CT_Add);

  // Contacts (Deleted):
  connect(_cts.mg, &CTSManager::SI_GRP_ContactDeleted , _cts_ui, &CTSManagerUI::SL_CT_Del);

  // ---- End - Transmit CTSManager signals to CTSManagerUI ---- //


  // ---- Transmit Win signals to CTSManagerIE ---- //
  connect(this, &Win::SI_ReqImportContacts, _cts.ie, &CTSManagerIE::ImportContacts);
  connect(this, &Win::SI_ReqExportContacts, _cts.ie, &CTSManagerIE::ExportContacts);

  // ---- End - Transmit Win signals to CTSManagerIE ---- //



  // ---- Transmit CTSManager signals to CTSManagerUI ---- //
  // Group (Added):
  connect(_cts.mg, &CTSManager::SI_GroupAdded   , _cts_ui, &CTSManagerUI::SL_GRP_Add  );

  // Group (Removed):
  connect(_cts.mg, &CTSManager::SI_GroupRemoved , _cts_ui, &CTSManagerUI::SL_GRP_Del  );

  // ---- End - Transmit CTSManager signals to CTSManagerUI ---- //



  // ---- Transmit Win signals to CTSManagerIE ---- //
  // Groups (Import):
  connect(this, &Win::SI_ReqImportGroups, _cts.ie, &CTSManagerIE::ImportGroups);

  // Groups (Export):
  connect(this, &Win::SI_ReqExportGroups, _cts.ie, &CTSManagerIE::ExportGroups);

  // ---- End - Transmit Win signals to CTSManagerIE ---- //



  // ---- Transmit EmailsManager signals to EmailsList UI ---- //
  // Emails (Added):
  connect(_emails.mg, &EmailsManager::SI_EmailAdded   , _mls_ui, &EmailsListUI::SL_AddEmail);

  // Emails (Deleted):
  connect(_emails.mg, &EmailsManager::SI_EmailDeleted , _mls_ui, &EmailsListUI::SL_DelEmail);

  // ---- End - Transmit EmailsManager signals to EmailsList UI ---- //



  // ---- Transmist IdentitiesManager signals to IdentitiesUI ---- //
  // Added:
  connect(_emails.idts, &IdentitiesManager::SI_IdentityAdded, _idts_ui, &IdentitiesUI::SL_AddIdentityToUI);
  // ---- END - Transmist IdentitiesManager signals to IdentitiesUI ---- //



  // ---- Transmist IdentitiesUI signals to DBManager ---- //
  connect(_idts_ui, &IdentitiesUI::SI_DBReq_Insert, _db.mg,
          QOverload<const S_IDT_Datas>::of(&DBManager::SL_IDT_Insert));
  // ---- END - Transmist IdentitiesUI signals to DBManager ---- //



  // ---- Transmit CTSManagerUI signals to Win ---- //
  // Group (New):
  connect(_cts_ui, &CTSManagerUI::SI_Req_NewGroup   , this  , &Win::SL_OnReq_CTSUI_NewGroup );

  // ---- End - Transmit CTSManagerUI signals to Win ---- //



  // ---- Transmit MLSProcessor signals to Win ---- //
  // Emails (Send):
  //connect(_emails.mlproc, &MLSProcessor::SI_ReqNT_SendEmail, this, &Win::SL_ReqNT_SendEmail);

  // ---- End - Transmit MLSProcessor signals to Win ---- //



  // ---- Transmit MLSProcessor signals to SMTPClient ---- //
  // Emails (Send):
  //connect(_emails.mlproc, &MLSProcessor::SI_ReqNT_SendEmail, _emails.smtpcl, &SMTPClient::SL_AddEmailObject);

  // ---- End - Transmit MLSProcessor signals to SMTPClient ---- //



  // ---- Transmit SMTPClient signals to Win ---- //
  connect(_emails.smtpcl, &SMTPClient::SI_MailSent, this, &Win::SL_MailSent);
  connect(_emails.mg, &EmailsManager::SI_ReqNT_SendEmail, _emails.smtpcl, &SMTPClient::SL_AddEmailObject);

  // ---- End - Transmit SMTPClient signals to Win ---- //


  // Initialize managers:
  _db.mg->Set_CTSMG_Thread(_cts.th);
  _db.mg->Set_MLSMG_Thread(_emails.th);
  _db.mg->Set_Ref_CTSManager(QSharedPointer<CTSManager*>::create(_cts.mg));
  _db.mg->Set_Ref_SMTPClient(QSharedPointer<SMTPClient*>::create(_emails.smtpcl));
  _db.mg->Set_Ref_Encryption(&_encryption);

  QTimer::singleShot(25 , this, &Win::SI_Init_LGSManager                          );
  QTimer::singleShot(50 , this, std::bind(&Win::SI_Init_SMTP, this, &_encryption) );
  QTimer::singleShot(100, this, &Win::SI_InitDB                                   );
  QTimer::singleShot(150, this, &Win::SI_Init_MLSProcessor                        );
  QTimer::singleShot(200, this, &Win::SI_Init_MLSManager                          );
  QTimer::singleShot(250, this, &Win::SI_Init_HttpSVManager                       );
  QTimer::singleShot(300, _http_sv.mg, &HttpServer::SL_Start                      );

  QTimer::singleShot(500, this, &Win::SL_InitialLoadDB);
}



/* ---------------- SL_InitialLoadDB ---------------- */
void Win::SL_InitialLoadDB()
{
  // Load Contacts, Groups, Emails from DB:
  QTimer::singleShot(50 , this, &Win::SI_DB_GRP_LoadAll );
  QTimer::singleShot(150, this, &Win::SI_DB_CT_LoadAll  );
  QTimer::singleShot(250, this, &Win::SI_DB_ML_LoadAll  );
  QTimer::singleShot(350, this, &Win::SI_DB_IDT_LoadAll );
}



/* ---------------- Connect_DBManager ---------------- */
void Win::Connect_DBManager()
{
  QTimer::singleShot(1, _db.mg, std::bind(&DBManager::SL_ConnectToLogsMG, _db.mg, _logs.mg));

  QTimer::singleShot(50, _db.mg, &DBManager::SL_Initialize);
}



/* ---------------- Create_DBManager ---------------- */
void Win::Create_DBManager()
{
  if (_db.th || _db.mg)
    return;

  _db.th = new QThread();
  _db.mg = new DBManager();

  if (_db.th && _db.mg) {
      _db.th->setObjectName("DB_thread");
      _db.mg->setObjectName("DB_manager");

      connect(_db.th, &QThread::finished, _db.mg, &DBManager::deleteLater);

      _db.mg->moveToThread(_db.th);
      _db.th->start(QThread::LowPriority);
    } else {
      if (_db.th) { delete _db.th; _db.th = nullptr; }
      if (_db.mg) { delete _db.mg; _db.mg = nullptr; }
    }
}



/* ---------------- Create_CTSManager ---------------- */
void Win::Create_CTSManager()
{
  if (_cts.th || _cts.mg || _cts.ie)
    return;

  _cts.th = new QThread();
  _cts.mg = new CTSManager();
  _cts.ie = new CTSManagerIE();

  if (_cts.th && _cts.mg && _cts.ie) {
      _cts.th->setObjectName("Contacts_thread");
      _cts.mg->setObjectName("Contacts_manager");
      _cts.ie->setObjectName("Contacts_ImportExport");

      connect(_cts.th, &QThread::finished, _cts.mg, &CTGroup::deleteLater);

      _cts.mg->moveToThread(_cts.th);
      _cts.ie->moveToThread(_cts.th);

      _cts.ie->setParent(_cts.mg);

      _cts.th->start(QThread::LowPriority);
    } else {
      if (_cts.th) { delete _cts.th; _cts.th = nullptr; }
      if (_cts.mg) { delete _cts.mg; _cts.mg = nullptr; }
      if (_cts.ie) { delete _cts.ie; _cts.ie = nullptr; }
    }
}



/* ---------------- Create_EmailsManager ---------------- */
void Win::Create_EmailsManager()
{
  if (_emails.th || _emails.mg || _emails.mlproc || _emails.smtpcl)
    return;

  _emails.th = new QThread();
  _emails.mg = new EmailsManager();
  _emails.mlproc = new MLSProcessor(QSharedPointer<EmailsManager*>::create(_emails.mg), nullptr);
  _emails.smtpcl = new SMTPClient("smtp.gmail.com", 465, 30000, 60000, nullptr);
  _emails.idts = new IdentitiesManager();

  if (_emails.th && _emails.mg && _emails.mlproc && _emails.smtpcl && _emails.idts) {
      _emails.th->setObjectName("Emails_thread");
      _emails.mg->setObjectName("Emails_manager");
      _emails.mlproc->setObjectName("Emails_processor");
      _emails.smtpcl->setObjectName("Emails_smtp");
      _emails.idts->setObjectName("Emails_identities_mg");

      connect(_emails.th, &QThread::finished, _emails.mg    , &EmailsManager::deleteLater );
      connect(_emails.th, &QThread::finished, _emails.mlproc, &MLSProcessor::deleteLater  );
      connect(_emails.th, &QThread::finished, _emails.smtpcl, &SMTPClient::deleteLater    );
      connect(_emails.th, &QThread::finished, _emails.idts   , &Identity::deleteLater      );

      _emails.mg->moveToThread(_emails.th);
      _emails.mlproc->moveToThread(_emails.th);
      _emails.smtpcl->moveToThread(_emails.th);
      _emails.idts->moveToThread(_emails.th);

      _emails.th->start(QThread::LowPriority);
    } else {
      if (_emails.th) { delete _emails.th; _emails.th = nullptr; }
      if (_emails.mg) { delete _emails.mg; _emails.mg = nullptr; }
      if (_emails.mlproc) { delete _emails.mlproc; _emails.mlproc = nullptr; }
      if (_emails.smtpcl) { delete _emails.smtpcl; _emails.smtpcl = nullptr; }
      if (_emails.idts) { delete _emails.idts; _emails.idts = nullptr; }
    }
}



void Win::Create_LGSManager()
{
  if (_logs.th || _logs.mg)
    return;

  _logs.th = new QThread();
  _logs.mg = new LogsManager();

  if (_logs.th && _logs.mg) {
      _logs.th->setObjectName("Logs_thread");
      _logs.mg->setObjectName("Logs_manager");

      connect(_logs.th, &QThread::finished, _logs.mg, &LogsManager::deleteLater);

      _logs.mg->moveToThread(_logs.th);

      _logs.th->start(QThread::LowPriority);
    } else {
      if (_logs.th) { delete _logs.th; _logs.th = nullptr; }
      if (_logs.mg) { delete _logs.mg; _logs.mg = nullptr; }
    }
}



void Win::Create_HttpSVManager()
{
  if (_http_sv.th || _http_sv.mg)
    return;

  _http_sv.th = new QThread();
  _http_sv.mg = new HttpServer(QHostAddress::LocalHost, 45062, nullptr);

  if (_http_sv.th && _http_sv.mg) {
      _http_sv.th->setObjectName("HttpServer_thread");
      _http_sv.mg->setObjectName("HttpServer_manager");

      connect(_http_sv.th, &QThread::finished, _http_sv.mg, &HttpServer::deleteLater);

      _http_sv.mg->moveToThread(_http_sv.th);

      _http_sv.th->start(QThread::LowPriority);
    } else {
      if (_http_sv.th) delete _http_sv.th;
      if (_http_sv.mg) delete _http_sv.mg;
    }
}



/* ---------------- SL_OnReq_CTSUI_NewGroup ---------------- */
void Win::SL_OnReq_CTSUI_NewGroup()
{
  QByteArray name = QInputDialog::getText(nullptr, tr("New Group"), tr("Name:"),
                                          QLineEdit::EchoMode::Normal, "example").toUtf8();

  if (name.isEmpty())
    return;

  QByteArray desc = QInputDialog::getMultiLineText(nullptr, tr("New Group"), tr("Description (optional):")).toUtf8();

  emit SI_DB_GRP_Insert(name, desc);
}



/* ---------------- SL_CTGroup_ContactAdded ---------------- */
void Win::SL_CTGroup_ContactAdded(const QSharedPointer<Contact*> &/*contact*/)
{
}


/* ---------------- SL_CTSMG_GroupAdded ---------------- */
void Win::SL_CTSMG_GroupAdded(const QSharedPointer<CTGroup*> &group)
{
  if (!group.isNull()) {
      CTGroup *grp = *group.data();

      if (!grp->parent())
        grp->setParent(_cts.mg);
    }
}



/* ---------------- SL_ReqNT_SendEmail ---------------- */
void Win::SL_ReqNT_SendEmail(QSharedPointer<EmailObject*> /*pml*/)
{
}



void Win::SL_MailSent(QSharedPointer<MailPKT*> ml_pkt)
{
  if (!ml_pkt.isNull()) {
      MailPKT *pkt = *ml_pkt.data();

      qDebug().noquote() << QString("Win::SL_MailSent | pkt = 0x%1")
                            .arg((qint64)pkt, 8, 16, QChar('0'));
      qDebug().noquote() << QString("Win::SL_MailSent | pkt.subject = %1")
                            .arg(pkt->GetSubject());
      qDebug().noquote() << QString("Win::SL_MailSent | pkt.from = %1")
                            .arg(pkt->GetFrom().GetAddress());
      qDebug().noquote() << QString("Win::SL_MailSent | pkt.rctps.size = %1")
                            .arg(pkt->GetToRcpts().size(), 3, 10, QChar('0'));
      qDebug().noquote() << QString("Win::SL_MailSent | pkt.part_id = %1")
                            .arg(pkt->GetPartID());
      qDebug().noquote() << QString("Win::SL_MailSent | pkt.parts_total = %1")
                            .arg(pkt->GetPartsTotal());

      EmailObject *ml = dynamic_cast<EmailObject*>(pkt->parent());

      if (ml) {
          qDebug().noquote() << QString("Win::SL_MailSent | mail = 0x%1 | ID: %2 | Subject: %3\n")
                                .arg((qint64)ml, 8, 16, QChar('0'))
                                .arg(ml->GetID(), 3, 10, QChar('0'))
                                .arg(ml->GetSubject());

          if ( (pkt->GetPartID()+1) == pkt->GetPartsTotal() ) {
              ml->SetSent(true);
              ml->SetProcessing(false);

              QSharedPointer<EmailObject*> pml = QSharedPointer<EmailObject*>::create(ml);

              QTimer::singleShot(10, this, std::bind(&Win::SI_ReqUpdate_MLSList_Item, this, pml));

              QTimer::singleShot(3000, this, std::bind(&Win::SI_DB_ML_Delete, this,
                                                       QSharedPointer<EmailObject*>::create(ml)));
            }
      } else
        qDebug().noquote() << QString("Win::SL_MailSent | mail = 0x%1 | ID: %2 | Subject: %3\n")
                              .arg((qint64)nullptr, 8, 16, QChar('0'))
                              .arg(0, 3, 10, QChar('0'))
                              .arg("");

      delete pkt;
      pkt = nullptr;
    }
}




/* ---------------- SL_MNAC_ImportContacts ---------------- */
void Win::SL_MNAC_ImportContacts()
{
  QString
      filter = "CSV File (*.csv);;Text File (*.txt)",
      fpath = QFileDialog::getOpenFileName(nullptr, tr("Select a file"),
                                           QDir::homePath(), filter);

  if (fpath.isEmpty())
    return;


  QWidget *w = new QWidget(this, Qt::Dialog);

  w->setWindowModality(Qt::ApplicationModal);
  w->setObjectName("ContactsIE_SelectGrpWin");

  QGridLayout *lay = new QGridLayout();

  w->setLayout(lay);

  QLabel    *lb         = new QLabel(tr("Select group:")),
            *lb_path    = new QLabel();
  QComboBox *grps_list  = new QComboBox();

  lb_path->setObjectName("ContactsIE_SelectGrp_Path");
  lb_path->setText(fpath);

  grps_list->setObjectName("ContactsIE_SelectGrp_Grp");
  grps_list->addItem("-- auto group --", QVariant::fromValue(nullptr));

  QVector<QSharedPointer<CTGroup*>> grps = _cts.mg->GetGroups();
  QVector<QSharedPointer<CTGroup*>>::const_iterator it = grps.constBegin(), ite = grps.constEnd();

  for (; it != ite; it++) {
      QVariant datas;
      datas.setValue((*it));

      grps_list->addItem((*(*it).data())->GetName(), datas);
    }

  lay->addWidget(lb_path  , 0, 0, 1, 1);
  lay->addWidget(lb       , 1, 0, 1, 1);
  lay->addWidget(grps_list, 1, 1, 1, 1);


  QPushButton *bt_ok = new QPushButton(tr("Import")), *bt_cancel = new QPushButton(tr("Cancel"));
  QHBoxLayout *bt_lay = new QHBoxLayout();

  connect(bt_ok     , &QPushButton::clicked, this, &Win::OnBT_ImportContacts_Ok     );
  connect(bt_cancel , &QPushButton::clicked, this, &Win::OnBT_ImportContacts_Cancel );

  bt_lay->addWidget(bt_cancel);
  bt_lay->addWidget(bt_ok);

  lay->addLayout(bt_lay, 2, 1, 1, 1);

  w->show();
}


/* ---------------- OnBT_ImportContacts_Ok ---------------- */
void Win::OnBT_ImportContacts_Ok()
{
  QPushButton *bt = qobject_cast<QPushButton*>(this->sender());

  if (bt) {
      QWidget *w = dynamic_cast<QWidget*>(bt->parent());

      if (w) {
          QMainWindow *mainWin = dynamic_cast<QMainWindow*>(w->parent());

          if (mainWin) {
              if (w->objectName().compare("ContactsIE_SelectGrpWin") == 0) {
                  QByteArray path;

                  for (int n = 0; n < w->children().size(); n++) {
                      if (w->children().at(n)->objectName().compare("ContactsIE_SelectGrp_Grp", Qt::CaseSensitive) == 0) {
                          QComboBox *cbx = dynamic_cast<QComboBox*>(w->children().at(n));
                          QSharedPointer<CTGroup*> grp = nullptr;

                          if (cbx)
                            grp = cbx->itemData(cbx->currentIndex(), Qt::UserRole).value<QSharedPointer<CTGroup*>>();

                          emit SI_ReqImportContacts(grp, path);
                          break;
                        } else if (w->children().at(n)->objectName().compare("ContactsIE_SelectGrp_Path", Qt::CaseSensitive) == 0) {
                          path = dynamic_cast<QLabel*>(w->children().at(n))->text().toUtf8();
                        }
                    }

                  w->close();
                }
            }
        }
    }
}



/* ---------------- OnBT_ImportContacts_Cancel ---------------- */
void Win::OnBT_ImportContacts_Cancel()
{
  QPushButton *bt = qobject_cast<QPushButton*>(this->sender());

  if (bt) {
      QWidget *w = dynamic_cast<QWidget*>(bt->parent());

      if (w) {
          QMainWindow *mainWin = dynamic_cast<QMainWindow*>(w->parent());

          if (mainWin) {
              if (w->objectName().compare("ContactsIE_SelectGrpWin") == 0) {
                  w->close();
                }
            }
        }
    }
}



/* ---------------- SL_MNAC_ExportContacts ---------------- */
void Win::SL_MNAC_ExportContacts()
{
  QString     filter  = "CSV File (*.csv);;Text File (*.txt)";
  QByteArray  path    = QFileDialog::getSaveFileName(nullptr, tr("Export contacts"),
                                                     QDir::homePath(), filter).toUtf8();

  if (path.isEmpty())
    return;

  emit SI_ReqExportContacts(_cts_ui->GetSelected_AllContacts(), path);
}



/* ---------------- SL_MNAC_ImportGroups ---------------- */
void Win::SL_MNAC_ImportGroups()
{
  qDebug().noquote() << QString("Win::SL_MNAC_ImportGroups | TO DO");
}



/* ---------------- SL_MNAC_ExportGroups ---------------- */
void Win::SL_MNAC_ExportGroups()
{
  qDebug().noquote() << QString("Win::SL_MNAC_ExportGroups | TO DO");
}



/* ---------------- SL_MNAC_NewEmail ---------------- */
void Win::SL_MNAC_NewEmail()
{
  EmailCreateForm *win = new EmailCreateForm(QSharedPointer<CTSManager*>::create(_cts.mg), nullptr);

  win->BuildUI();
  win->setWindowTitle(tr("Create Email"));
  win->setWindowModality(Qt::ApplicationModal);
  win->setObjectName("MLS_CreateEmailWin");

  connect(win, &EmailCreateForm::SI_ReqEmailCreate, _db.mg, &DBManager::SL_ML_Insert);

  win->show();
}


/* ---------------- OnBT_MLS_NewEmail_Ok ---------------- */
void Win::OnBT_MLS_NewEmail_Ok()
{
  QPushButton *bt = qobject_cast<QPushButton*>(this->sender());

  if (bt) {
      QWidget *w = dynamic_cast<QWidget*>(bt->parent());

      if (w) {
          QMainWindow *mainWin = dynamic_cast<QMainWindow*>(w->parent());

          if (mainWin) {
              if (w->objectName().compare("MLS_CreateEmailWin") == 0) {
                  QByteArray subject, content;
                  quint64 dt_exec = 0;
                  QVector<QSharedPointer<CTGroup*>> grps;

                  for (int n = 0; n < w->children().size(); n++) {

                      if (w->children().at(n)->objectName().compare("MLS_CreateEmail_Subject", Qt::CaseSensitive) == 0) {
                          QLineEdit *ln = dynamic_cast<QLineEdit*>(w->children().at(n));

                          if (ln)
                            subject = ln->text().toUtf8();
                        } else if (w->children().at(n)->objectName().compare("MLS_CreateEmail_Groups", Qt::CaseSensitive) == 0) {
                          QTreeWidget *lst = dynamic_cast<QTreeWidget*>(w->children().at(n));

                          if (lst) {
                              for (int n = 0; n < lst->topLevelItemCount(); n++) {
                                  if (lst->topLevelItem(n)->checkState(0) == Qt::Checked) {
                                      grps.append(lst->topLevelItem(n)->data(0, Qt::UserRole).value<QSharedPointer<CTGroup*>>());
                                    }
                                }
                            }
                        } else if (w->children().at(n)->objectName().compare("MLS_CreateEmail_Content", Qt::CaseSensitive) == 0) {
                          QTextEdit *content_txt = dynamic_cast<QTextEdit*>(w->children().at(n));

                          if (content_txt)
                            content = content_txt->toHtml().toUtf8();
                        } else if (w->children().at(n)->objectName().compare("MLS_CreateEmail_DTExec", Qt::CaseSensitive) == 0) {
                          QDateTimeEdit *edit_dt = dynamic_cast<QDateTimeEdit*>(w->children().at(n));

                          if (edit_dt)
                            dt_exec = edit_dt->dateTime().toMSecsSinceEpoch();
                        }

                      }

                  emit SI_DB_ML_Insert(subject, content, dt_exec, grps);

                  w->close();
                }
            }
        }
    }
}



/* ---------------- OnBT_MLS_NewEmail_Cancel ---------------- */
void Win::OnBT_MLS_NewEmail_Cancel()
{
  QPushButton *bt = qobject_cast<QPushButton*>(this->sender());

  if (bt) {
      QWidget *w = dynamic_cast<QWidget*>(bt->parent());

      if (w) {
          QMainWindow *mainWin = dynamic_cast<QMainWindow*>(w->parent());

          if (mainWin) {
              if (w->objectName().compare("MLS_CreateEmailWin") == 0) {
                  w->close();
                }
            }
        }
    }
}



/* ---------------- OnCTSManagerIE_ContactsExported ---------------- */
void Win::OnCTSManagerIE_ContactsExported(QVector<QSharedPointer<Contact*>> cts)
{
  qDebug().noquote() << QString("Win::OnCTSManagerIE_ContactsExported | cts.size = %1").arg(cts.size(), 4, 10, QChar('0'));
}


/* ---------------- OnCTSManagerIE_GroupsExported ---------------- */
void Win::OnCTSManagerIE_GroupsExported(QVector<QSharedPointer<CTGroup*>> grps)
{
  qDebug().noquote() << QString("Win::OnCTSManagerIE_GroupsExported | grps.size = %1").arg(grps.size(), 4, 10, QChar('0'));
}



/* ---------------- SL_MLS_EmailAdded ---------------- */
void Win::SL_MLS_EmailAdded(QSharedPointer<EmailObject*> /*pml*/)
{
}





void Win::Test()
{
  /*QUrl url("https://account.google.com/o/oauth2/v2/auth");

  //QNetworkAccessManager *mg = new QNetworkAccessManager(this);
  QNetworkRequest req(url);
  QUrlQuery url_query;

  url_query.addQueryItem("scope", "email%20profile");
  url_query.addQueryItem("response_type", "code");
  url_query.addQueryItem("state", QString("security_token%1")
                         .arg(QDateTime::currentMSecsSinceEpoch(), 8, 16, QChar('0')));
  url_query.addQueryItem("redirect_uri",
                         QUrl(QHostAddress(QHostAddress::LocalHost).toString()
                              .append(":45062")).toString(QUrl::FullyEncoded));
  url_query.addQueryItem("client_id", G_CL_ID);

  req.setHeader(QNetworkRequest::ContentTypeHeader,
                 "application/x-www-form-urlencoded");

  QSharedPointer<QNetworkAccessManager*> netmg = _http_sv.mg->GetNetworkAccessMG();
  QNetworkAccessManager *mg = *netmg.data();

  if (mg && mg->post(req, url_query.toString(QUrl::FullyEncoded).toUtf8()))
    qDebug().noquote() << QString("[Win::Test] POST request sent to %1")
                          .arg(url.toString());*/

  /*
  QString base_url = "https://accounts.google.com/o/oauth2/v2/",
          action = "auth",
          scope = "email%20profile",
          response_type = "code",
          sec_token_hex = QString("%1").arg(QDateTime::currentMSecsSinceEpoch(), 8, 16, QChar('0')),
          sec_token = QString("security_token%1").arg(sec_token_hex),
          redirect_uri = QUrl("http://127.0.0.1:45062").toString(QUrl::FullyEncoded),
          client_id = G_CL_ID;

  QString url = QString("%1%2?scope=%3&response_type=%4&state=%5&redirect_uri=%6&client_id=%7")
      .arg(base_url, action, scope, response_type, sec_token, redirect_uri, client_id);

  QStringList arguments;
  arguments << url;

  qint64 pid = 0;
  QProcess::startDetached(QString("C:/Program Files/Mozilla Firefox/firefox.exe"), arguments, "", &pid);
  */
}
