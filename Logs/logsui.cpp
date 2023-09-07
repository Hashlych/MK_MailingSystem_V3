#include "logsui.h"

LogsUI::LogsUI(QWidget *parent)
  : QWidget{parent}
{
}


bool LogsUI::BuildUI()
{
  if (!(_mlay = new QGridLayout()))
    return false;
  else
    this->setLayout(_mlay);

  _lb = new QLabel(tr("Logs:"));
  _display = new QTextEdit();

  _display->setReadOnly(true);


  _mlay->addWidget(_lb, 0, 0, 1, 1);
  _mlay->addWidget(_display, 1, 0, 1, 2);


  return true;
}



void LogsUI::SL_AddToDisplay(QVector<QSharedPointer<LogObject*>> logs)
{
  int nmax = logs.size();
  QSharedPointer<LogObject*> plg;
  QByteArrayList types_str;

  types_str.append("NONE");
  types_str.append("<span style=\"color: rgb(0, 175, 0);\">INFO</span>");
  types_str.append("<span style=\"color: rgb(175, 175, 25);\">WARNING</span>");
  types_str.append("<span style=\"color: rgb(175, 0, 0);\">ERROR</span>");
  types_str.append("<span style=\"color: rgb(25, 175, 175);\">DEBUG</span>");

  for (int n = 0; n < nmax; n++) {
      plg = logs.takeFirst();

      if (!plg.isNull()) {
          _display->append( QString("[ %1 ][%2] : %3")
                            .arg( (*plg.data())->GetDT().toString("yyyy/MM/dd hh:mm:ss.zzz") )
                            .arg( types_str.at((qint32)(*plg.data())->GetType()),
                                  (*plg.data())->GetText() ) );

          delete (*plg.data());
          plg.clear();
        }
    }
}
