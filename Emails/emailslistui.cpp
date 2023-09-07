#include "emailslistui.h"

EmailsListUI::EmailsListUI(QWidget *parent)
  : QWidget{parent}
{
}

EmailsListUI::~EmailsListUI()
{
}



void EmailsListUI::SL_OnReqUpdate_Timers()
{
  for (int n = 0; n < _table->rowCount(); n++) {
      QTableWidgetItem *itm = _table->item(n, 4);
      EmailObject *ml = (*_table->item(n, 0)->data(Qt::UserRole).value<QSharedPointer<EmailObject*>>());

      if (ml->HasBeenSent())
        continue;

      qint64 msecs_left = ml->GetDT_Exec() - QDateTime::currentMSecsSinceEpoch();

      if (msecs_left < 0)
        msecs_left = 0;

      qint64 days = 0, hours = 0, mins = 0, secs = 0, base_secs = 0;

      secs = (msecs_left / 1000) % 60;
      base_secs = (msecs_left / 1000);

      mins = (base_secs / 60) % 60;
      hours = (base_secs / 3600) % 24;
      days = (base_secs / 86400) % 30;

      QColor color(0, 0, 0);

      if (days == 0 && hours == 0 && (mins >= 1 && mins < 5) && (secs > 0 && secs <= 59))
        color.setRgb(135, 65, 135);
      else if (days == 0 && hours == 0 && mins == 0 && (secs >= 10 && secs <= 59))
        color.setRgb(135, 135, 65);
      else if (days == 0 && hours == 0 && mins == 0 && (secs >= 0 && secs <= 9))
        color.setRgb(0, 175, 0);


      if (ml->IsProcessing() && msecs_left == 0) {
          itm->setText(QString("Sending.."));
          itm->setForeground(QBrush(QColor(0, 125, 0)));
        } else {
          itm->setText(QString("%1d %2h%3m %4s")
                       .arg(days, 2, 10, QChar('0'))
                       .arg(hours, 2, 10, QChar('0'))
                       .arg(mins, 2, 10, QChar('0'))
                       .arg(secs, 2, 10, QChar('0')));
          itm->setForeground(QBrush(color));
        }
    }
}


void EmailsListUI::SL_OnReqUpdate_Item(QSharedPointer<EmailObject*> pml)
{
  if (!pml.isNull()) {
      for (int n = 0; n < _table->rowCount(); n++) {
          QTableWidgetItem *itm = _table->item(n, 0);
          QSharedPointer<EmailObject*> tmp = itm->data(Qt::UserRole).value<QSharedPointer<EmailObject*>>();

          EmailObject *tmp_ml = (*tmp.data()), *ml = (*pml.data());

          if (tmp_ml == ml) {
              qDebug().noquote() << QString("EmailsListUI::SL_OnReqUpdate_Item | item found");

              _table->item(n, 4)->setText(tr("Sent"));
              _table->item(n, 4)->setForeground(QBrush(QColor(0, 200, 0)));

              return;
            }
        }

      qDebug().noquote() << QString("EmailsListUI::SL_OnReqUpdate_Item | item not found");
    }
}


bool EmailsListUI::BuildUI()
{
  if (!(_mlay = new QGridLayout()))
    return false;
  else
    this->setLayout(_mlay);

  _table = new QTableWidget();
  QStringList header_labels;

  header_labels << tr("Select") << tr("Subject") << tr("Schedule to") << tr("Groups") << tr("Time remaining");

  _table->setColumnCount(header_labels.size());
  _table->setHorizontalHeaderLabels(header_labels);

  _table->setShowGrid(false);
  _table->setEditTriggers(QTableWidget::NoEditTriggers);
  _table->setSelectionMode(QTableWidget::NoSelection);

  _table->setColumnWidth(0, (1*1920)/100);
  _table->setColumnWidth(1, (5*1920)/100);
  _table->setColumnWidth(2, (10*1920)/100);
  _table->setColumnWidth(3, (25*1920)/100);
  _table->setColumnWidth(4, (10*1920)/100);

  _bts.lay = new QHBoxLayout();
  _bts.del_selected = new QPushButton(tr("Delete Selected"));

  connect(_bts.del_selected, &QPushButton::clicked, this, &EmailsListUI::OnBT_DelSelected);

  _bts.lay->addWidget(_bts.del_selected);


  _mlay->addLayout(_bts.lay, 0, 0, 1, 1);
  _mlay->addWidget(_table, 1, 0, 1, 1);


  return true;
}



void EmailsListUI::SL_AddEmail(QSharedPointer<EmailObject*> pml)
{
  if (pml.isNull())
    return;

  EmailObject *ml = *pml.data();


  QTableWidgetItem
      *itm_select = new QTableWidgetItem(),
      *itm_subject = new QTableWidgetItem(ml->GetSubject()),
      *itm_dt_exec = new QTableWidgetItem(QDateTime::fromMSecsSinceEpoch(ml->GetDT_Exec())
                                          .toString("dd/MM/yyyy hh:mm:ss")),
      *itm_tm_left = new QTableWidgetItem();

  qint64 msecs_left = ml->GetDT_Exec() - QDateTime::currentMSecsSinceEpoch();

  if (msecs_left < 0)
    msecs_left = 0;

  qint64 days = 0, hours = 0, mins = 0, secs = 0, base_secs = 0;

  secs = (msecs_left / 1000) % 60;
  base_secs = (msecs_left / 1000);

  mins = (base_secs / 60) % 60;
  hours = (base_secs / 3600) % 24;
  days = (base_secs / 86400) % 30;

  QColor color(0, 0, 0);

  if (days == 0 && hours == 0 && mins <= 1)
    color.setRgb(175, 0, 0);
  else if (days == 0 && hours == 0 && mins == 0 && (secs > 0 && secs <= 30))
    color.setRgb(135, 135, 65);
  else if (days == 0 && hours == 0 && mins == 0 && secs == 0)
    color.setRgb(0, 175, 0);

  itm_tm_left->setText(QString("%1d %2h%3m %4s")
                       .arg(days, 2, 10, QChar('0'))
                       .arg(hours, 2, 10, QChar('0'))
                       .arg(mins, 2, 10, QChar('0'))
                       .arg(secs, 2, 10, QChar('0')) );
  itm_tm_left->setForeground(QBrush(color));
  itm_tm_left->setTextAlignment(Qt::AlignRight);


  itm_select->setData(Qt::UserRole, QVariant::fromValue(pml));
  itm_select->setCheckState(Qt::Unchecked);


  QVector<QSharedPointer<CTGroup*>> grps = ml->GetGroups();
  QVector<QSharedPointer<CTGroup*>>::const_iterator it = grps.constBegin(), ite = grps.constEnd();
  QString grps_text;
  QTextEdit *grps_output = new QTextEdit();

  int n = 0;

  for (; it != ite; it++) {
      grps_text.append((*(*it).data())->GetName());

      if (n == 5) {
          grps_text.append("<br />");
          n = 0;
        } else {
          grps_text.append(" ; ");
          n++;
        }
    }

  grps_output->setReadOnly(true);
  grps_output->append(grps_text);


  int row = _table->rowCount();

  _table->insertRow(row);
  _table->setItem(row, 0, itm_select);
  _table->setItem(row, 1, itm_subject);
  _table->setItem(row, 2, itm_dt_exec);
  _table->setCellWidget(row, 3, grps_output);
  _table->setItem(row, 4, itm_tm_left);

  //_table->resizeColumnToContents(4);
  _table->sortByColumn(2, Qt::AscendingOrder);
}



void EmailsListUI::SL_DelEmail(QSharedPointer<EmailObject*> pml)
{
  if (pml.isNull())
    return;

  EmailObject *ml = *pml.data(), *tmp_ml = nullptr;
  QTableWidgetItem *tmp_itm = nullptr;

  for (int n = 0; n < _table->rowCount(); n++) {
      tmp_itm = _table->item(n, 0);
      tmp_ml = *(tmp_itm->data(Qt::UserRole).value<QSharedPointer<EmailObject*>>().data());

      if (tmp_ml && tmp_ml == ml) {
          _table->removeRow(n);

          delete ml;
          ml = nullptr;

          break;
        }

      tmp_itm = nullptr;
      tmp_ml = nullptr;
    }
}



void EmailsListUI::OnBT_DelSelected()
{
  QVector<QSharedPointer<EmailObject*>> selected = this->GetEmails_Selected();

  QMessageBox msgb(QMessageBox::Warning, tr("Delete emails"), "", QMessageBox::No | QMessageBox::Yes);
  msgb.setText(tr("You are about to delete selected emails. Continue ?"));
  msgb.setInformativeText(QString(tr("count: %1")).arg(selected.size()));

  int rep = msgb.exec();

  if (rep == QMessageBox::No)
    return;

  for (int n = 0; n < selected.size(); n++) {
      emit SI_ReqDelEmail(selected.at(n));
    }
}



QVector<QSharedPointer<EmailObject*>> EmailsListUI::GetEmails_Selected() const
{
  QVector<QSharedPointer<EmailObject*>> pmls;
  QTableWidgetItem *itm = nullptr;

  for (int n = 0; n < _table->rowCount(); n++) {
      itm = _table->item(n, 0);

      if (itm->checkState() == Qt::Checked)
        pmls.append(itm->data(Qt::UserRole).value<QSharedPointer<EmailObject*>>());

      itm = nullptr;
    }

  return pmls;
}
