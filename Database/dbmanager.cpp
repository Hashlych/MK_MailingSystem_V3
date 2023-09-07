#include "dbmanager.h"

DBManager::DBManager(QObject *parent)
  : QObject{parent}
{
}


DBManager::~DBManager()
{
  if (_db.isOpen())
    _db.close();
}



void DBManager::SL_Initialize()
{
  if (!_init) {
      this->CreateQueries();

      _db.setDatabaseName("datas");

      if (!_db.open())
        return;

      this->CreateTables();

      _init = true;

      emit SI_AddLog(LOG_TYPES::INFO, "Database intialized.");
    }
}


void DBManager::SL_ConnectToLogsMG(LogsManager *mg)
{
  if (!mg)
    return;

  connect(this, &DBManager::SI_AddLog, mg, &LogsManager::SL_AddLog);
}



void DBManager::CreateQueries()
{
  // contacts:
  _queries.insert(DBQR_REFS::CT_INSERT,
                  QString("INSERT INTO contacts(group_id, firstName, lastName, email) "
                          "VALUES(:group_id, :firstName, :lastName, :email)") );

  _queries.insert(DBQR_REFS::CT_SAVE,
                  QString("UPDATE contacts SET group_id = :group_id, firstName = :firstName, lastName = :lastName, "
                          "email = :email WHERE id = :id") );

  _queries.insert(DBQR_REFS::CT_LOAD,
                  QString("SELECT id, group_id, firstName, lastName, email FROM contacts WHERE id = :id") );

  _queries.insert(DBQR_REFS::CT_LOAD_ALL,
                  QString("SELECT id, group_id, firstName, lastName, email FROM contacts ORDER BY lastName ASC") );

  _queries.insert(DBQR_REFS::CT_DEL,
                  QString("DELETE FROM contacts WHERE id = :id") );

  _queries.insert(DBQR_REFS::CT_CREATE_TBL,
                  QString("CREATE TABLE IF NOT EXISTS contacts("
                          "id INTEGER PRIMARY KEY,"
                          "group_id INTEGER,"
                          "firstName VARCHAR(256),"
                          "lastName VARCHAR(256),"
                          "email VARCHAR(256));") );


  // groups:
  _queries.insert(DBQR_REFS::GRP_INSERT,
                  QString("INSERT INTO groups(name, description) VALUES(:name, :description)") );

  _queries.insert(DBQR_REFS::GRP_SAVE,
                  QString("UPDATE groups SET name = :name, description = :desc WHERE id = :id") );

  _queries.insert(DBQR_REFS::GRP_LOAD,
                  QString("SELECT id, name, description FROM groups WHERE id = :id") );

  _queries.insert(DBQR_REFS::GRP_LOAD_ALL,
                  QString("SELECT id, name, description FROM groups ORDER BY NAME ASC") );

  _queries.insert(DBQR_REFS::GRP_DEL,
                  QString("DELETE FROM groups WHERE id = :id") );

  _queries.insert(DBQR_REFS::GRP_CREATE_TBL,
                  QString("CREATE TABLE IF NOT EXISTS groups("
                          "id INTEGER PRIMARY KEY,"
                          "name VARCHAR(256),"
                          "description TEXT);") );


  // emails:
  _queries.insert(DBQR_REFS::ML_INSERT,
                  QString("INSERT INTO emails(dt_exec, subject, content, groups) "
                          "VALUES(:dt_exec, :subject, :content, :groups)"));

  _queries.insert(DBQR_REFS::ML_SAVE,
                  QString("UPDATE emails SET dt_exec = :dt_exec, subject = :subject, "
                          "content = :content, groups = :groups WHERE id = :id"));

  _queries.insert(DBQR_REFS::ML_LOAD,
                  QString("SELECT id, dt_exec, subject, content, groups FROM emails WHERE id = :id"));

  _queries.insert(DBQR_REFS::ML_LOAD_ALL,
                  QString("SELECT id, dt_exec, subject, content, groups FROM emails ORDER BY id ASC"));

  _queries.insert(DBQR_REFS::ML_DEL,
                  QString("DELETE FROM emails WHERE id = :id"));

  _queries.insert(DBQR_REFS::ML_CREATE_TBL,
                  QString("CREATE TABLE IF NOT EXISTS emails("
                          "id INTEGER PRIMARY KEY,"
                          "dt_exec UNSIGNED INTEGER,"
                          "subject VARCHAR(256),"
                          "content TEXT,"
                          "groups TEXT);"));


  // identities:
  _queries.insert(DBQR_REFS::IDT_INSERT,
                  QString("INSERT INTO identities(name, addr, code, tokens) "
                          "VALUES(:name, :addr, :code, :tokens)"));

  _queries.insert(DBQR_REFS::IDT_SAVE,
                  QString("UPDATE identities SET name = :name, addr = :addr, code = :code, tokens = :tokens "
                          "WHERE id = :id"));

  _queries.insert(DBQR_REFS::IDT_LOAD,
                  QString("SELECT id, name, addr, code, tokens FROM identities WHERE id = :id"));

  _queries.insert(DBQR_REFS::IDT_LOAD_ALL,
                  QString("SELECT id, name, addr, code, tokens FROM identities ORDER BY id ASC"));

  _queries.insert(DBQR_REFS::IDT_DEL,
                  QString("DELETE FROM identities WHERE id = :id"));

  _queries.insert(DBQR_REFS::IDT_CREATE_TBL,
                  QString("CREATE TABLE IF NOT EXISTS identities("
                          "id INTEGER PRIMARY KEY,"
                          "name VARCHAR(256),"
                          "addr VARCHAR(256),"
                          "code VARCHAR(256),"
                          "tokens TEXT);"));
}


void DBManager::CreateTables()
{
  QStringList tables = _db.tables();

  if (!tables.contains("contacts", Qt::CaseSensitive)) {
      if (_query.prepare(_queries.value(DBQR_REFS::CT_CREATE_TBL)))
        _query.exec();

      _query.clear();
    }


  if (!tables.contains("groups", Qt::CaseSensitive)) {
      if (_query.prepare(_queries.value(DBQR_REFS::GRP_CREATE_TBL))) {
          if (_query.exec()) {
              _query.clear();

              if (!_p_smtpcl.isNull() && _query.prepare(_queries.value(DBQR_REFS::GRP_INSERT))) {
                  QByteArray
                      cl_key = (*_p_smtpcl.data())->GetCLKey(),
                      en_name = _encryption->encode("Default", cl_key),
                      en_desc = _encryption->encode("This is the default group.", cl_key);

                  _query.bindValue(":name", en_name);
                  _query.bindValue(":description", en_desc);

                  _query.exec();
                }

              _query.clear();
            }
        }
    }

  if (!tables.contains("emails", Qt::CaseSensitive)) {
      if (_query.prepare(_queries.value(DBQR_REFS::ML_CREATE_TBL)))
        _query.exec();

      _query.clear();
    }

  if (!tables.contains("identities", Qt::CaseSensitive)) {
      if (_query.prepare(_queries.value(DBQR_REFS::IDT_CREATE_TBL)))
        _query.exec();

      _query.clear();
    }
}



/* -------- SL_CT_Insert -------- */
void DBManager::SL_CT_Insert(const QByteArray &firstName, const QByteArray &lastName, const QByteArray &email,
                             const QSharedPointer<CTGroup*> grp)
{
  if (!_db.isOpen() || !_cts_mg_thread)
    return;

  _query.clear();


  // check if contact already exists in DB:
  if (_query.prepare("SELECT COUNT(id) FROM contacts WHERE group_id = :group_id AND "
                     "((firstName = :firstName AND lastName = :lastName) OR (email = :email))")) {

      // encrypt datas:
      QByteArray cl_key = (*_p_smtpcl.data())->GetCLKey();
      QByteArray
          en_firstName = _encryption->encode(firstName, cl_key),
          en_lastName = _encryption->encode(lastName, cl_key),
          en_email = _encryption->encode(email, cl_key);

      qDebug().noquote() << QString("DBManager::SL_CT_Insert | firstName = %1 | en_firstName = %2")
                            .arg(firstName, en_firstName.toBase64());

      qDebug().noquote() << QString("DBManager::SL_CT_Insert | lastName = %1 | en_lastName = %2")
                            .arg(lastName, en_lastName.toBase64());

      qDebug().noquote() << QString("DBManager::SL_CT_Insert | email = %1 | en_email = %2")
                            .arg(email, en_email.toBase64());

      // bind values for query:
      _query.bindValue(":firstName", en_firstName);
      _query.bindValue(":lastName", en_lastName);
      _query.bindValue(":email", en_email);

      int grp_id = 0;

      if (!grp.isNull())
        grp_id = (*grp.data())->GetID();

      _query.bindValue(":group_id", grp_id);

      if (_query.exec() && _query.next()) {

          // contact already exists:
          if (_query.value(0).toInt() > 0)
            return;
          else {
              // contact doesn't exist, insert:
              _query.clear();

              if (_query.prepare(_queries.value(DBQR_REFS::CT_INSERT))) {
                  _query.bindValue(":group_id", grp_id);
                  _query.bindValue(":firstName", en_firstName);
                  _query.bindValue(":lastName", en_lastName);
                  _query.bindValue(":email", en_email);

                  if (_query.exec()) {
                      Contact *ct = new Contact(firstName, lastName, email, _query.lastInsertId().toUInt(), nullptr);

                      ct->moveToThread(_cts_mg_thread);

                      if (!grp.isNull())
                        ct->setParent(*grp.data());

                      emit SI_CT_Inserted(ct);
                    }
                }
            }

        }

    }
}



qint16 DBManager::ContactExists(const S_CT_Datas &ct_datas)
{
  if (!_db.isOpen() || _p_cts_mg.isNull() ||
      ct_datas.firstName.isEmpty() || ct_datas.lastName.isEmpty() || ct_datas.email.isEmpty())
    return -1;


  // prep & exec query:
  if (_query.prepare("SELECT COUNT(id) FROM contacts WHERE (firstName = :firstName AND lastName = :lastName) OR "
                     "email = :email")) {

      // encrypt datas:
      QByteArray
          en_firstName  = _encryption->encode(ct_datas.firstName, (*_p_smtpcl.data())->GetCLKey()),
          en_lastName   = _encryption->encode(ct_datas.lastName , (*_p_smtpcl.data())->GetCLKey()),
          en_email      = _encryption->encode(ct_datas.email    , (*_p_smtpcl.data())->GetCLKey());

      _query.bindValue(":firstName" , en_firstName);
      _query.bindValue(":lastName"  , en_lastName );
      _query.bindValue(":email"     , en_email    );

      if (_query.exec() && _query.next()) {
          if (_query.value(0).toInt() > 0)
            return 1;
          else
            return 0;
        } else {
          emit SI_AddLog(LOG_TYPES::ERR,
                         QString("Database error: %1").arg(_query.lastError().text()).toUtf8());
          return -2;
        } // end if query exec

    } else {
      emit SI_AddLog(LOG_TYPES::ERR,
                     QString("Database error: %1").arg(_query.lastError().text()).toUtf8());
      return -3;
    } // end if query prep

  return 0;
}



/* ---------------- SL_CT_Insert ---------------- */
void DBManager::SL_CT_Insert(const S_CT_Datas ct_datas)
{
  if (ct_datas.id > 0 || ct_datas.firstName.isEmpty() || ct_datas.lastName.isEmpty() || ct_datas.email.isEmpty() ||
      !_db.isOpen() || !_cts_mg_thread || _p_cts_mg.isNull())
    return;

  _query.clear();


  QSharedPointer<CTGroup*> pct = (*(_p_cts_mg.data()))->GetGroupByName(ct_datas.grpName);
  CTSManager *ctsmg = *_p_cts_mg.data();

  if (!ctsmg) {
      emit SI_AddLog(LOG_TYPES::ERR,
                     QString("Abort contact insertion into database. Reason: invalid contacts manager").toUtf8());
      return;
    }


  // check if contact already exists in DB:
  if (this->ContactExists(ct_datas) != 0) {
      emit SI_AddLog(LOG_TYPES::ERR,
                     QString("Failed to insert contact into database : <em>%1 %2</em> (<em>%3</em>)")
                     .arg(ct_datas.firstName, ct_datas.lastName, ct_datas.email).toUtf8());
      return;
    }


  QSharedPointer<CTGroup*> pgrp = ctsmg->GetGroupByName(ct_datas.grpName);
  CTGroup *grp = nullptr;

  if (!pgrp.isNull())
    grp = *pgrp.data();
  else {
      grp = *(ctsmg->GetDefaultGroup().data());

      emit SI_AddLog(LOG_TYPES::ERR,
                     QString("pgrp is null. setting grp to default group (0x%1 : %2)")
                     .arg((qint64)grp, 8, 16, QChar('0')).arg(grp->GetName()).toUtf8());
    }

  // encrypt datas:
  QByteArray
      en_firstName  = _encryption->encode(ct_datas.firstName, (*_p_smtpcl.data())->GetCLKey()),
      en_lastName   = _encryption->encode(ct_datas.lastName , (*_p_smtpcl.data())->GetCLKey()),
      en_email      = _encryption->encode(ct_datas.email    , (*_p_smtpcl.data())->GetCLKey());

  // prep & exec query:
  if (_query.prepare(_queries.value(DBQR_REFS::CT_INSERT))) {
      _query.bindValue(":firstName" , en_firstName);
      _query.bindValue(":lastName"  , en_lastName );
      _query.bindValue(":email"     , en_email    );
      _query.bindValue(":group_id"  , grp->GetID());

      if (_query.exec()) {
          Contact *ct = new Contact(ct_datas.firstName, ct_datas.lastName, ct_datas.email,
                                    _query.lastInsertId().toUInt(), nullptr);

          ct->moveToThread(_cts_mg_thread);
          ct->setParent(grp);

          emit SI_CT_Inserted(ct);

        } else {
          emit SI_AddLog(LOG_TYPES::ERR,
                         QString("Error while inserting contact <em>%1 %2</em> (<em>%3</em>). [error: %4]")
                         .arg(ct_datas.firstName, ct_datas.lastName, ct_datas.email,
                              _query.lastError().text()).toUtf8());
        } // end if query exec

    } // end if query prep

}



/* ---------------- SL_CTS_Insert ---------------- */
void DBManager::SL_CTS_Insert(QVector<S_CT_Datas> cts_datas)
{
  if (cts_datas.isEmpty() || !_db.isOpen() || !_cts_mg_thread || _p_cts_mg.isNull())
    return;

  for (int n = 0; n < cts_datas.size(); n++)
    this->SL_CT_Insert(cts_datas.at(n));
}



/* -------- SL_CT_Save -------- */
void DBManager::SL_CT_Save(QSharedPointer<Contact*> pct)
{
  if (!_db.isOpen() || pct.isNull())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::CT_SAVE))) {

      Contact *ct = *pct.data();

      if (!ct || ct->GetID() == 0) {
          ct = nullptr;
          return;
        }

      CTGroup *grp = dynamic_cast<CTGroup*>(ct->parent());
      quint32 grp_id = 0;

      if (grp)
        grp_id = grp->GetID();

      // encrypt datas:
      QByteArray
          en_firstName = _encryption->encode(ct->GetFirstName(), (*_p_smtpcl.data())->GetCLKey()),
          en_lastName = _encryption->encode(ct->GetLastName(), (*_p_smtpcl.data())->GetCLKey()),
          en_email = _encryption->encode(ct->GetEmail(), (*_p_smtpcl.data())->GetCLKey());

      _query.bindValue(":id", ct->GetID());
      _query.bindValue(":group_id", grp_id);
      _query.bindValue(":firstName", en_firstName);
      _query.bindValue(":lastName", en_lastName);
      _query.bindValue(":email", en_email);

      if (_query.exec())
        emit SI_CT_Saved(pct);
      else
        emit SI_AddLog(LOG_TYPES::ERR,
                       QString("Failed to save contact : <em>%1</em>").arg(_query.lastError().text()).toUtf8());
    } else
    emit SI_AddLog(LOG_TYPES::ERR,
                   QString("Failed to save contact : <em>%1</em>").arg(_query.lastError().text()).toUtf8());
}



/* -------- SL_CT_Load -------- */
void DBManager::SL_CT_Load(const quint32 &id)
{
  if (!_db.isOpen() || !_cts_mg_thread)
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::CT_LOAD))) {
      _query.bindValue(":id", id);

      if (_query.exec() && _query.next()) {
          // decrypt datas:
          QByteArray
              de_firstName = _encryption->removePadding(
                _encryption->decode(_query.value("firstName").toByteArray(),
                                    (*_p_smtpcl.data())->GetCLKey()) ),
              de_lastName = _encryption->removePadding(
                _encryption->decode(_query.value("lastName").toByteArray(),
                                    (*_p_smtpcl.data())->GetCLKey()) ),
              de_email = _encryption->removePadding(
                _encryption->decode(_query.value("email").toByteArray(),
                                    (*_p_smtpcl.data())->GetCLKey()) );

          Contact *ct = new Contact(de_firstName, de_lastName, de_email, _query.value("id").toUInt(), nullptr);

          ct->moveToThread(_cts_mg_thread);

          emit SI_CT_Loaded(ct, _query.value("group_id").toUInt());
        } else
        emit SI_AddLog(LOG_TYPES::ERR,
                       QString("Failed to load contact : <em>%1</em>").arg(_query.lastError().text()).toUtf8());
    } else
    emit SI_AddLog(LOG_TYPES::ERR,
                   QString("Failed to load contact : <em>%1</em>").arg(_query.lastError().text()).toUtf8());
}


/* -------- SL_CT_LoadAll -------- */
void DBManager::SL_CT_LoadAll()
{
  if (!_db.isOpen() || !_cts_mg_thread)
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::CT_LOAD_ALL))) {
      if (_query.exec()) {
          while (_query.next()) {
              // decrypt datas:
              QByteArray
                  de_firstName = _encryption->removePadding(_encryption->decode(_query.value("firstName").toByteArray(),
                                                                                (*_p_smtpcl.data())->GetCLKey())),
                  de_lastName = _encryption->removePadding(_encryption->decode(_query.value("lastName").toByteArray(),
                                                                                (*_p_smtpcl.data())->GetCLKey())),
                  de_email = _encryption->removePadding(_encryption->decode(_query.value("email").toByteArray(),
                                                                                (*_p_smtpcl.data())->GetCLKey()));

              Contact *ct = new Contact(de_firstName, de_lastName, de_email, _query.value("id").toUInt(), nullptr);

              ct->moveToThread(_cts_mg_thread);

              emit SI_CT_Loaded(ct, _query.value("group_id").toUInt());
            }
        } else
        emit SI_AddLog(LOG_TYPES::ERR,
                       QString("Failed to load contacts : <em>%1</em>").arg(_query.lastError().text()).toUtf8());
    } else
    emit SI_AddLog(LOG_TYPES::ERR,
                   QString("Failed to load contacts : <em>%1</em>").arg(_query.lastError().text()).toUtf8());
}


/* -------- SL_CT_Delete -------- */
void DBManager::SL_CT_Delete(QSharedPointer<Contact*> pct)
{
  if (!_db.isOpen() || pct.isNull())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::CT_DEL))) {
      Contact *ct = *pct.data();

      if (!ct || ct->GetID() == 0) {
          ct = nullptr;
          return;
        }

      _query.bindValue(":id", ct->GetID());

      if (_query.exec())
        emit SI_CT_Deleted(pct);
      else
        emit SI_AddLog(LOG_TYPES::ERR,
                       QString("Failed to delete contact : <em>%1</em>").arg(_query.lastError().text()).toUtf8());
    } else
    emit SI_AddLog(LOG_TYPES::ERR,
                   QString("Failed to delete contact : <em>%1</em>").arg(_query.lastError().text()).toUtf8());
}





/* -------- SL_GRP_Insert -------- */
void DBManager::SL_GRP_Insert(const QByteArray &name, const QByteArray &desc)
{
  if (!_db.isOpen() || !_cts_mg_thread)
    return;

  _query.clear();


  if (_query.prepare("SELECT COUNT(id) FROM groups WHERE name = :name")) {

      // encrypt datas:
      QByteArray
          cl_key = (*_p_smtpcl.data())->GetCLKey(),
          en_name = _encryption->encode(name, cl_key),
          en_desc = _encryption->encode(desc, cl_key);

      _query.bindValue(":name", en_name);

      if (_query.exec() && _query.next()) {

          if (_query.value(0).toInt() > 0) {
              emit SI_AddLog(LOG_TYPES::ERR,
                             QString("Failed to insert group : %1 (<em>already exists</em>)")
                             .arg(name).toUtf8());
              return;
            } else {
              _query.clear();

              if (_query.prepare(_queries.value(DBQR_REFS::GRP_INSERT))) {
                  _query.bindValue(":name", en_name);
                  _query.bindValue(":desc", en_desc);

                  if (_query.exec()) {
                      CTGroup *grp = new CTGroup(name, desc, _query.lastInsertId().toUInt(), nullptr);

                      grp->moveToThread(_cts_mg_thread);

                      emit SI_GRP_Inserted(grp);
                    } else // end if exec query insert
                    emit SI_AddLog(LOG_TYPES::ERR,
                                   QString("Failed to insert group : %1> (<em>%2</em>)")
                                   .arg(name)
                                   .arg(_query.lastError().text()).toUtf8());
                } else // end if prep query insert
                emit SI_AddLog(LOG_TYPES::ERR,
                               QString("Failed to insert group : %1> (<em>%2</em>)")
                               .arg(name)
                               .arg(_query.lastError().text()).toUtf8());
            } // end of if do not exists

        } else // end of exec query check already exists
        emit SI_AddLog(LOG_TYPES::ERR,
                       QString("Failed to insert group : %1> (<em>%2</em>)")
                       .arg(name)
                       .arg(_query.lastError().text()).toUtf8());

    } else // end of prep query check already exists
    emit SI_AddLog(LOG_TYPES::ERR,
                   QString("Failed to insert group : %1 (<em>%2</em>)")
                   .arg(name)
                   .arg(_query.lastError().text()).toUtf8());
}


/* -------- SL_GRP_Save -------- */
void DBManager::SL_GRP_Insert(const S_GRP_Datas grp_datas)
{
  if (grp_datas.id > 0 || grp_datas.name.isEmpty() ||
      !_db.isOpen() || !_cts_mg_thread)
    return;

  _query.clear();


  if (_query.prepare("SELECT COUNT(id) FROM groups WHERE name = :name")) {

      // Encrypt datas:
      QByteArray
          cl_key = (*_p_smtpcl.data())->GetCLKey(),
          en_name = _encryption->encode(grp_datas.name, cl_key),
          en_desc = _encryption->encode(grp_datas.desc, cl_key);

      _query.bindValue(":name", en_name);

      if (_query.exec() && _query.next()) {

          if (_query.value(0).toInt() > 0)
            return;
          else {
              _query.clear();

              if (_query.prepare(_queries.value(DBQR_REFS::GRP_INSERT))) {
                  _query.bindValue(":name", en_name);
                  _query.bindValue(":desc", en_desc);

                  if (_query.exec()) {
                      CTGroup *grp = new CTGroup(grp_datas.name, grp_datas.desc, _query.lastInsertId().toUInt(), nullptr);

                      grp->moveToThread(_cts_mg_thread);

                      emit SI_GRP_Inserted(grp);
                    }
                }
            }

        }
    }
}



/* -------- SL_GRP_Save -------- */
void DBManager::SL_GRP_Save(QSharedPointer<CTGroup*> pgrp)
{
  if (!_db.isOpen() || pgrp.isNull())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::GRP_SAVE))) {
      CTGroup *grp = *pgrp.data();

      if (!grp || grp->GetID() == 0) {
          grp = nullptr;
          return;
        }

      // Encrypt datas:
      QByteArray
          cl_key = (*_p_smtpcl.data())->GetCLKey(),
          en_name = _encryption->encode(grp->GetName(), cl_key),
          en_desc = _encryption->encode(grp->GetDescription(), cl_key);

      _query.bindValue(":id", grp->GetID());
      _query.bindValue(":name", en_name);
      _query.bindValue(":description", en_desc);

      if (_query.exec())
        emit SI_GRP_Saved(pgrp);
    }
}


/* -------- SL_GRP_Load -------- */
void DBManager::SL_GRP_Load(const quint32 &id)
{
  if (!_db.isOpen() || id == 0)
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::GRP_LOAD))) {
      _query.bindValue(":id", id);

      if (_query.exec() && _query.next()) {
          // Decrypt datas:
          QByteArray
              cl_key = (*_p_smtpcl.data())->GetCLKey(),
              de_name = _encryption->removePadding(_encryption->decode(_query.value("name").toByteArray(), cl_key)),
              de_desc = _encryption->removePadding(_encryption->decode(_query.value("description").toByteArray(), cl_key));

          CTGroup *grp = new CTGroup(de_name, de_desc, _query.value("id").toUInt(), nullptr);

          grp->moveToThread(_cts_mg_thread);

          emit SI_GRP_Loaded(grp);
        }
    }
}


/* -------- SL_GRP_LoadAll -------- */
void DBManager::SL_GRP_LoadAll()
{
  if (!_db.isOpen() || !_cts_mg_thread)
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::GRP_LOAD_ALL))) {
      if (_query.exec()) {

          while (_query.next()) {
              // Decrypt datas:
              QByteArray
                  cl_key = (*_p_smtpcl.data())->GetCLKey(),
                  de_name = _encryption->removePadding(_encryption->decode(_query.value("name").toByteArray(), cl_key)),
                  de_desc = _encryption->removePadding(_encryption->decode(_query.value("description").toByteArray(), cl_key));

              CTGroup *grp = new CTGroup(de_name, de_desc, _query.value("id").toUInt(), nullptr);

              grp->moveToThread(_cts_mg_thread);

              emit SI_GRP_Loaded(grp);
            }

        }
    }
}


/* -------- SL_GRP_Delete -------- */
void DBManager::SL_GRP_Delete(QSharedPointer<CTGroup*> pgrp)
{
  if (!_db.isOpen() || pgrp.isNull())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::GRP_DEL))) {
      CTGroup *grp = *pgrp.data();

      if (!grp || grp->GetID() == 0) {
          grp = nullptr;
          return;
        }

      _query.bindValue(":id", grp->GetID());

      if (_query.exec())
        emit SI_GRP_Deleted(pgrp);
    }
}





/* -------- SL_ML_Insert -------- */
void DBManager::SL_ML_Insert(const QByteArray &subject, const QByteArray &content, const quint64 &dt_exec,
                             QVector<QSharedPointer<CTGroup*>> groups)
{
  if (!_db.isOpen() || !_mls_mg_thread || subject.isEmpty())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::ML_INSERT))) {

      QByteArray groups_str;
      QVector<QSharedPointer<CTGroup*>>::const_iterator it = groups.constBegin(), ite = groups.constEnd();

      for (; it != ite; it++) {
          groups_str.append( QString("%1").arg((*(*it).data())->GetID()).toUtf8() );

          if ((it+1) != ite)
            groups_str.append(";");
        }

      // Encrypt datas:
      QByteArray
          cl_key = (*_p_smtpcl.data())->GetCLKey(),
          en_subject = _encryption->encode(subject, cl_key),
          en_content = _encryption->encode(content, cl_key),
          en_groups = _encryption->encode(groups_str, cl_key);

      _query.bindValue(":dt_exec", dt_exec);
      _query.bindValue(":subject", en_subject);
      _query.bindValue(":content", en_content);
      _query.bindValue(":groups", en_groups);

      if (_query.exec()) {
          EmailObject *ml = new EmailObject(subject, content, dt_exec, _query.lastInsertId().toUInt(), nullptr);

          ml->SetGroups(groups);
          ml->moveToThread(_mls_mg_thread);

          emit SI_ML_Inserted(ml);
        } else
        qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
}


/* -------- SL_ML_Save -------- */
void DBManager::SL_ML_Save(QSharedPointer<EmailObject*> pml)
{
  if (!_db.isOpen() || pml.isNull())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::ML_SAVE))) {
      EmailObject *ml = *pml.data();

      QByteArray groups_str;
      QVector<QSharedPointer<CTGroup*>> groups = ml->GetGroups();
      QVector<QSharedPointer<CTGroup*>>::const_iterator it = groups.constBegin(), ite = groups.constEnd();

      for (; it != ite; it++) {
          groups_str.append( QString("%1").arg((*(*it).data())->GetID()).toUtf8() );

          if ((it+1) != ite)
            groups_str.append(";");
        }

      // Encrypt datas:
      QByteArray
          cl_key = (*_p_smtpcl.data())->GetCLKey(),
          en_subject = _encryption->encode(ml->GetSubject(), cl_key),
          en_content = _encryption->encode(ml->GetContent(), cl_key),
          en_groups = _encryption->encode(groups_str, cl_key);

      _query.bindValue(":id", ml->GetID());
      _query.bindValue(":dt_exec", ml->GetDT_Exec());
      _query.bindValue(":subject", en_subject);
      _query.bindValue(":content", en_content);
      _query.bindValue(":groups", en_groups);

      if (_query.exec())
        emit SI_ML_Saved(pml);
      else
        qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
}



/* -------- SL_ML_Load -------- */
void DBManager::SL_ML_Load(const quint32 &id)
{
  if (!_db.isOpen() || !_mls_mg_thread || id == 0)
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::ML_LOAD))) {
      _query.bindValue(":id", id);

      if (_query.exec()) {
          // Decrypt datas:
          QByteArray
              cl_key = (*_p_smtpcl.data())->GetCLKey(),
              de_groups_str = _encryption->removePadding(_encryption->decode(_query.value("groups").toByteArray(), cl_key)),
              de_subject = _encryption->removePadding(_encryption->decode(_query.value("subject").toByteArray(), cl_key)),
              de_content = _encryption->removePadding(_encryption->decode(_query.value("content").toByteArray(), cl_key));

          QByteArrayList groups_str_exp = de_groups_str.split(';');
          QVector<QSharedPointer<CTGroup*>> grps;

          while (!groups_str_exp.isEmpty())
            grps.append( (*_p_cts_mg.data())->GetGroupByID(groups_str_exp.takeFirst().toUInt()) );

          EmailObject *ml = new EmailObject(de_subject,
                                            de_content,
                                            _query.value("dt_exec").toULongLong(),
                                            _query.value("id").toUInt(), nullptr);

          ml->SetGroups(grps);
          ml->moveToThread(_mls_mg_thread);

          emit SI_ML_Loaded(ml);
        } else
        qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
}



/* -------- SL_ML_LoadAll -------- */
void DBManager::SL_ML_LoadAll()
{
  if (!_db.isOpen() || !_mls_mg_thread)
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::ML_LOAD_ALL))) {
      if (_query.exec()) {
          while (_query.next()) {
              // Decrypt datas:
              QByteArray
                  cl_key = (*_p_smtpcl.data())->GetCLKey(),
                  de_groups_str = _encryption->removePadding(_encryption->decode(_query.value("groups").toByteArray(), cl_key)),
                  de_subject = _encryption->removePadding(_encryption->decode(_query.value("subject").toByteArray(), cl_key)),
                  de_content = _encryption->removePadding(_encryption->decode(_query.value("content").toByteArray(), cl_key));

              QByteArrayList grps_str_list = de_groups_str.split(';');
              QVector<QSharedPointer<CTGroup*>> grps;

              while (!grps_str_list.isEmpty())
                grps.append( (*_p_cts_mg.data())->GetGroupByID(grps_str_list.takeFirst().toUInt()) );

              EmailObject *ml = new EmailObject(de_subject,
                                                de_content,
                                                _query.value("dt_exec").toULongLong(),
                                                _query.value("id").toUInt(), nullptr);

              ml->SetGroups(grps);
              ml->moveToThread(_mls_mg_thread);

              emit SI_ML_Loaded(ml);
            }
        } else
        qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
}



/* -------- SL_ML_Delete -------- */
void DBManager::SL_ML_Delete(QSharedPointer<EmailObject*> pml)
{
  if (!_db.isOpen() || pml.isNull())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::ML_DEL))) {
      _query.bindValue(":id", (*pml.data())->GetID());

      if (_query.exec())
        emit SI_ML_Deleted(pml);
      else
        qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_ML_Insert | sql error: %1").arg(_query.lastError().text());
}





/* -------- SL_IDT_Insert -------- */
void DBManager::SL_IDT_Insert(const QByteArray &name, const QByteArray &addr, const QByteArray &code,
                              const QByteArray &tokens)
{
  if (!_db.isOpen())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::IDT_INSERT))) {

      // Encrypt datas:
      QByteArray
          cl_key = (*_p_smtpcl.data())->GetCLKey(),
          en_name = _encryption->encode(name, cl_key),
          en_addr = _encryption->encode(addr, cl_key),
          en_code = _encryption->encode(code, cl_key),
          en_tokens = _encryption->encode(tokens, cl_key);

      _query.bindValue(":name", en_name);
      _query.bindValue(":addr", en_addr);
      _query.bindValue(":code", en_code);
      _query.bindValue(":tokens", en_tokens);

      if (_query.exec()) {
          Identity *idt = new Identity(name, addr, nullptr);

          QByteArrayList tmpl = tokens.split(';');
          QByteArray
              tkn_access = "",
              tkn_refresh = "",
              tkn_type = "";

          qint32 tkn_expires_in = 0;

          if (tmpl.size() == 4) {
              tkn_access = tmpl.at(0);
              tkn_refresh = tmpl.at(1);
              tkn_type = tmpl.at(2);
              tkn_expires_in = tmpl.at(3).toInt();
            }


          idt->SetID(_query.lastInsertId().toUInt());
          idt->SetCode(code);
          idt->SetTokens(tkn_access, tkn_refresh, tkn_type, tkn_expires_in);

          idt->moveToThread(_mls_mg_thread);

          emit SI_IDT_Inserted(idt);
        } else
        qDebug().noquote() << QString("DBManager::SL_IDT_Insert | sql error %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_IDT_Insert | sql error %1").arg(_query.lastError().text());
}



/* -------- SL_IDT_Insert -------- */
void DBManager::SL_IDT_Insert(const S_IDT_Datas idt_datas)
{
  if (!_db.isOpen())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::IDT_INSERT))) {

      // Encrypt datas:
      QByteArrayList tokens = {
        idt_datas.tokens.access,
        idt_datas.tokens.refresh,
        idt_datas.tokens.type,
        QString("%1").arg(idt_datas.tokens.expires_in).toUtf8()
      };

      QByteArray
          cl_key = (*_p_smtpcl.data())->GetCLKey(),
          en_name = _encryption->encode(idt_datas.name, cl_key),
          en_addr = _encryption->encode(idt_datas.addr, cl_key),
          en_code = _encryption->encode(idt_datas.code, cl_key),
          en_tokens = _encryption->encode(tokens.join(';'), cl_key);

      _query.bindValue(":name", en_name);
      _query.bindValue(":addr", en_addr);
      _query.bindValue(":code", en_code);
      _query.bindValue(":tokens", en_tokens);

      if (_query.exec()) {
          Identity *idt = new Identity(idt_datas.name, idt_datas.addr, nullptr);

          idt->SetID(_query.lastInsertId().toUInt());
          idt->SetCode(idt_datas.code);
          idt->SetTokens(idt_datas.tokens.access, idt_datas.tokens.refresh, idt_datas.tokens.type,
                         idt_datas.tokens.expires_in);

          idt->moveToThread(_mls_mg_thread);

          emit SI_IDT_Inserted(idt);
        } else
        qDebug().noquote() << QString("DBManager::SL_IDT_Insert | sql error %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_IDT_Insert | sql error %1").arg(_query.lastError().text());
}



/* -------- SL_IDT_Save -------- */
void DBManager::SL_IDT_Save(QSharedPointer<Identity*> idt)
{
  if (!_db.isOpen())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::IDT_SAVE))) {

      // Encrypt datas:
      QByteArrayList tokens = {
        (*idt.data())->GetToken_Access(),
        (*idt.data())->GetToken_Refresh(),
        (*idt.data())->GetToken_Type(),
        QString("%1").arg((*idt.data())->GetToken_ExpiresIn()).toUtf8()
      };

      QByteArray
          cl_key = (*_p_smtpcl.data())->GetCLKey(),
          en_name = _encryption->encode((*idt.data())->GetName(), cl_key),
          en_addr = _encryption->encode((*idt.data())->GetAddress(), cl_key),
          en_code = _encryption->encode((*idt.data())->GetCode(), cl_key),
          en_tokens = _encryption->encode(tokens.join(';'), cl_key);

      _query.bindValue(":name", en_name);
      _query.bindValue(":addr", en_addr);
      _query.bindValue(":code", en_code);
      _query.bindValue(":tokens", en_tokens);

      if (_query.exec())
        emit SI_IDT_Saved(idt);
      else
        qDebug().noquote() << QString("DBManager::SL_IDT_Save | sql error %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_IDT_Save | sql error %1").arg(_query.lastError().text());
}



/* -------- SL_IDT_Load -------- */
void DBManager::SL_IDT_Load(const quint32 &id)
{
  if (!_db.isOpen())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::IDT_LOAD))) {
      _query.bindValue(":id", id);

      if (_query.exec() && _query.next()) {
          QByteArray
              cl_key = (*_p_smtpcl.data())->GetCLKey(),
              de_name = _encryption->removePadding(
                _encryption->decode(_query.value("name").toByteArray(), cl_key)),
              de_addr = _encryption->removePadding(
                _encryption->decode(_query.value("addr").toByteArray(), cl_key)),
              de_code = _encryption->removePadding(
                _encryption->decode(_query.value("code").toByteArray(), cl_key)),
              de_tokens = _encryption->removePadding(
                _encryption->decode(_query.value("tokens").toByteArray(), cl_key));

          QByteArrayList tmpl = de_tokens.split(';');
          QByteArray
              tkn_access = "",
              tkn_refresh = "",
              tkn_type = "";

          qint32 tkn_expires_in = 0;

          if (tmpl.size() == 4) {
              tkn_access = tmpl.at(0);
              tkn_refresh = tmpl.at(1);
              tkn_type = tmpl.at(2);
              tkn_expires_in = tmpl.at(3).toInt();
            }


          Identity *idt = new Identity(nullptr);

          idt->SetID(id);
          idt->SetName(de_name);
          idt->SetAddress(de_addr);
          idt->SetCode(de_code);

          idt->SetTokens(tkn_access, tkn_refresh, tkn_type, tkn_expires_in);

          idt->moveToThread(_mls_mg_thread);

          emit SI_IDT_Loaded(idt);
        } else
        qDebug().noquote() << QString("DBManager::SL_IDT_Load | sql error %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_IDT_Load | sql error %1").arg(_query.lastError().text());
}



/* -------- SL_IDT_LoadAll -------- */
void DBManager::SL_IDT_LoadAll()
{
  if (!_db.isOpen())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::IDT_LOAD_ALL))) {
      if (_query.exec()) {
          quint32 id = 0;

          QByteArray
              cl_key = (*_p_smtpcl.data())->GetCLKey(),
              de_name, de_addr, de_code, de_tokens;

           while (_query.next()) {
               id = _query.value("id").toUInt();

               cl_key = (*_p_smtpcl.data())->GetCLKey(),
               de_name = _encryption->removePadding(
                 _encryption->decode(_query.value("name").toByteArray(), cl_key)),
               de_addr = _encryption->removePadding(
                 _encryption->decode(_query.value("addr").toByteArray(), cl_key)),
               de_code = _encryption->removePadding(
                 _encryption->decode(_query.value("code").toByteArray(), cl_key)),
               de_tokens = _encryption->removePadding(
                 _encryption->decode(_query.value("tokens").toByteArray(), cl_key));

               QByteArrayList tmpl = de_tokens.split(';');
               QByteArray
                   tkn_access = "",
                   tkn_refresh = "",
                   tkn_type = "";

               qint32 tkn_expires_in = 0;

               if (tmpl.size() == 4) {
                   tkn_access = tmpl.at(0);
                   tkn_refresh = tmpl.at(1);
                   tkn_type = tmpl.at(2);
                   tkn_expires_in = tmpl.at(3).toInt();
                 }

               Identity *idt = new Identity(nullptr);

               idt->SetID(id);
               idt->SetName(de_name);
               idt->SetAddress(de_addr);
               idt->SetCode(de_code);

               idt->SetTokens(tkn_access, tkn_refresh, tkn_type, tkn_expires_in);

               idt->moveToThread(_mls_mg_thread);

               emit SI_IDT_Loaded(idt);

               id = 0;
               de_name.clear();
               de_addr.clear();
               de_code.clear();
               tkn_refresh.clear();
             }
        } else
        qDebug().noquote() << QString("DBManager::SL_IDT_LoadAll | sql error %1").arg(_query.lastError().text());
    } else
    qDebug().noquote() << QString("DBManager::SL_IDT_LoadAll | sql error %1").arg(_query.lastError().text());
}



/* -------- SL_IDT_Delete -------- */
void DBManager::SL_IDT_Delete(QSharedPointer<Identity*> idt)
{
  if (!_db.isOpen())
    return;

  _query.clear();


  if (_query.prepare(_queries.value(DBQR_REFS::IDT_DEL))) {
      _query.bindValue(":id", (*idt.data())->GetID());

      if (_query.exec())
        emit SI_IDT_Deleted(idt);
      else
        qDebug().noquote() << QString("DBManager::SL_IDT_Delete | sql error %1").arg(_query.lastError().text());

    } else
    qDebug().noquote() << QString("DBManager::SL_IDT_Delete | sql error %1").arg(_query.lastError().text());
}





void DBManager::Set_CTSMG_Thread(QThread *th) { _cts_mg_thread = th; }
void DBManager::Set_MLSMG_Thread(QThread *th) { _mls_mg_thread = th; }
void DBManager::Set_Ref_CTSManager(QSharedPointer<CTSManager *> ref) { _p_cts_mg = ref; }
void DBManager::Set_Ref_SMTPClient(QSharedPointer<SMTPClient *> ref) { _p_smtpcl = ref; }
void DBManager::Set_Ref_Encryption(QAESEncryption *ref) { _encryption = ref; }


QThread * DBManager::Get_CTSMG_Thread() const { return _cts_mg_thread; }
QThread * DBManager::Get_MLSMG_Thread() const { return _mls_mg_thread; }
QSharedPointer<CTSManager*> DBManager::Get_Ref_CTSManager() const { return _p_cts_mg; }
QSharedPointer<SMTPClient*> DBManager::Get_Ref_SMTPClient() const { return _p_smtpcl; }
QAESEncryption * DBManager::Get_Ref_Encryption() const { return _encryption; }
