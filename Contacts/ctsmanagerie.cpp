#include "ctsmanagerie.h"
#include "ctsmanager.h"


CTSManagerIE::CTSManagerIE(const QByteArray &defaultLocation, QObject *parent)
  : QObject(parent),
    _def_location(defaultLocation)
{
}


CTSManagerIE::~CTSManagerIE()
{
  /*if (!_buffer_contacts.isEmpty()) {
      for (int n = 0; n < _buffer_contacts.size(); n++) {
          delete _buffer_contacts[n];
          _buffer_contacts[n] = nullptr;
        }

      _buffer_contacts.clear();
    }

  if (!_buffer_groups.isEmpty()) {
      for (int n = 0; n < _buffer_groups.size(); n++) {
          delete _buffer_groups[n];
          _buffer_groups[n] = nullptr;
        }

      _buffer_groups.clear();
    }*/
}



// ImportContacts //
void CTSManagerIE::ImportContacts(QSharedPointer<CTGroup*> grp, const QByteArray &filePath)
{
  // column_01 : firstName
  // column_02 : lastName
  // column_03 : email
  // column_04 : groupName (optional)

  QFile file(filePath);
  QRegularExpression rg("^[\\\"]{0,1}([a-zA-Z0-9éèçà'\\s]{0,})[\\\"]{0,1},"
                        "[\\\"]{0,1}([a-zA-Z0-9éèçà'\\s]{0,})[\\\"]{0,1},"
                        "[\\\"]{0,1}([a-zA-Z0-9@_.]{0,})[\\\"]{0,1},"
                        "[\\\"]{0,1}([a-zA-Z0-9éèçà@_.'\\s]{0,})[\\\"]{0,1}\\\n{0,}$");
  QRegularExpressionMatch match;

  // open file:
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      emit SI_AddLog(LOG_TYPES::ERR,
                     QString("Failed to import contacts. Cannot open file : <em>%1</em>")
                     .arg(filePath).toUtf8());
      return;
    }

  //CTSManager *cts_mg = qobject_cast<CTSManager*>(this->parent());

  /*qDebug().noquote() << QString("CTSManagerIE::ImportContacts | cts_mg = 0x%1 (%2)")
                        .arg((qint64)cts_mg, 8, 16, QChar('0')).arg(cts_mg->objectName());

  qDebug().noquote() << QString("CTSManagerIE::ImportContacts | grp = 0x%1")
                        .arg((qint64)grp, 8, 16, QChar('0'));*/

  QByteArray line;
  bool firstLine = true;
  QVector<S_CT_Datas> contacts;

  // read file content line by line:
  while (!file.atEnd()) {
      line = file.readLine();

      // ignore first line (columns headers):
      if (firstLine) {
          firstLine = false;
          continue;
        }


      if ((match = rg.match(line)).hasMatch()) {
          S_CT_Datas ct_datas = {0,                                       // ID (0 = need DB insert)
                                 match.captured(1).trimmed().toUtf8(),    // firstName
                                 match.captured(2).trimmed().toUtf8(),    // lastName
                                 match.captured(3).trimmed().toUtf8(),    // email
                                 "Default"};                              // group name

          /*qDebug().noquote() << QString("CTSManagerIE::ImportContacts | rg.captureCount = %1")
                                .arg(rg.captureCount());*/

          if (rg.captureCount() == 4) {
              QByteArray grp_str = match.captured(4).trimmed().toUtf8();

              if (grp_str.isEmpty())
                ct_datas.grpName = "Default";
              else
                ct_datas.grpName = grp_str;
          } else if (rg.captureCount() == 3) {
              if (!grp.isNull()) {
                  ct_datas.grpName = (*grp.data())->GetName();
                }
            }

          contacts.append(ct_datas);

          // encrypt datas:

          // create contact object:
          //QSharedPointer<Contact*> ct = QSharedPointer<Contact*>::create(new Contact(firstName, lastName, email, 0, nullptr));

          /*if (!grp.isNull()) {

              if ((*grp.data())->ContactExists(ct)) {
                  ct.clear();
                  continue;
                } else {
                  (*ct.data())->moveToThread((*grp.data())->thread());
                  (*ct.data())->setParent(*grp.data());
                }

            } else {
              if (cts_mg) {
                  QVector<QSharedPointer<CTGroup*>> grps = cts_mg->GetGroups();
                  QVector<QSharedPointer<CTGroup*>>::const_iterator it = grps.constBegin(), ite = grps.constEnd();

                  if (grpName.isEmpty())
                    grpName = "Default";

                  for (; it != ite; it++) {
                      if ((*(*it).data())->GetName().compare(grpName, Qt::CaseSensitive) == 0) {
                          grp = (*it);
                          break;
                        }
                    }

                  if (!grp.isNull() && (*grp.data())->ContactExists(ct)) {
                      ct.clear();
                      continue;
                    } else {
                      (*ct.data())->moveToThread((*grp.data())->thread());
                      (*ct.data())->setParent(*grp.data());
                    }
                }
            }

          if (ct)
            _buffer_contacts.append(ct);*/
        }
    }

  file.close();

  // Send results to CTGroupManager:
  emit ContactsImported(contacts);
}



// ImportGroups //
void CTSManagerIE::ImportGroups(const QByteArray &filePath)
{
  // column_01 : name
  // column_02 : description

  QFile file(filePath);
  QRegularExpression rg("^[\\\"]{0,1}([a-zA-Z0-9éèçà'\\s_]{0,})[\\\"]{0,1},[\\\"]{0,1}([a-zA-Z0-9éèçà'\\s_]{0,})[\\\"]{0,1}\\\n$");
  QRegularExpressionMatch match;

  // open file:
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

      emit SI_AddLog(LOG_TYPES::ERR,
                     QString("Failed to import groups. Cannot open file : <em>%1</em>")
                     .arg(filePath).toUtf8());
      return;
    }


  QByteArray line;
  bool firstLine = true;
  QVector<S_GRP_Datas> groups;

  // read file content line by line:
  while (!file.atEnd()) {
      line = file.readLine();

      // ignore first line (columns headers):
      if (firstLine) {
          firstLine = false;
          continue;
        }


      if ((match = rg.match(line)).hasMatch()) {
          S_GRP_Datas grp_datas = {0,                                       // ID
                                   match.captured(1).trimmed().toUtf8(),    // name
                                   match.captured(2).trimmed().toUtf8()};   // description

          groups.append(grp_datas);

          // encrypt datas:

          // create group object:
          /*QSharedPointer<CTGroup*> grp = QSharedPointer<CTGroup*>::create(new CTGroup(name, description, 0, nullptr));

          if (grp)
            _buffer_groups.append(grp);*/
        }
    }

  file.close();


  // Send results to CTGroupManager:
  emit GroupsImported(groups);
}



void CTSManagerIE::ExportContacts(QVector<QSharedPointer<Contact*>> cts, const QByteArray &filePath)
{
  if (filePath.isEmpty())
    return;


  QFile f(filePath);

  if (!f.open(QIODevice::WriteOnly)) {
      emit SI_AddLog(LOG_TYPES::ERR,
                     QString("Failed to export contacts. Cannot open file : <em>%1</em>")
                     .arg(filePath).toUtf8());
      return;
    }


  f.write(QString("\"First Name\",\"Last Name\",\"Email\",\"Group\"\n").toUtf8());


  int count = cts.size();
  QSharedPointer<Contact*> ct = nullptr;
  QSharedPointer<CTGroup*> grp = nullptr;

  for (int n = 0; n < count; n++) {
      ct = cts.takeFirst();
      grp = QSharedPointer<CTGroup*>::create( dynamic_cast<CTGroup*>((*ct.data())->parent()) );

      f.write(QString("\"%1\",\"%2\",\"%3\",")
              .arg((*ct.data())->GetFirstName(),
                   (*ct.data())->GetLastName(),
                   (*ct.data())->GetEmail()).toUtf8() );

      if (!grp.isNull())
        f.write(QString("\"%1\"")
                .arg((*grp.data())->GetName()).toUtf8() );
      else
        f.write(QString("\"\"").toUtf8());

      if ((n+1) < count)
        f.write("\n");
    }


  f.close();
}


void CTSManagerIE::ExportGroups(QVector<QSharedPointer<CTGroup *> > grps, const QByteArray &filePath)
{
  if (filePath.isEmpty())
    return;
}



// Mutators:
void CTSManagerIE::SetDefaultLocation(const QByteArray &path) { _def_location = path; }


// Accessors:
const QByteArray & CTSManagerIE::GetDefaultLocation() const { return _def_location; }
