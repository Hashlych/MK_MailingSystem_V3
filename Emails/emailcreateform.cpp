#include "emailcreateform.h"

EmailCreateForm::EmailCreateForm(QSharedPointer<CTSManager*> p_cts_mg, QWidget *parent)
  : QWidget{parent},
    _p_cts_mg(p_cts_mg)
{
}



bool EmailCreateForm::BuildUI()
{
  if (!(_mlay = new QGridLayout()))
    return false;
  else
    this->setLayout(_mlay);

  // subject:
  _subject.lb = new QLabel(tr("Subject:"));
  _subject.fd = new QLineEdit();

  _subject.fd->setMaxLength(96);


  // dt exec:
  _dt_exec.lb = new QLabel(tr("Execute on"));
  _dt_exec.fd = new QDateTimeEdit();

  _dt_exec.fd->setDateTimeRange(QDateTime::currentDateTime(), QDateTime::currentDateTime().addYears(10));
  _dt_exec.fd->setCalendarPopup(true);


  // content:
  _content.lb = new QLabel(tr("Content:"));
  _content.fd = new QTextEdit();


  // groups:
  _groups.lb = new QLabel(tr("Groups:"));
  _groups.fd = new QTreeWidget();

  _groups.fd->setEditTriggers(QTreeWidget::NoEditTriggers);
  _groups.fd->setSelectionMode(QTreeWidget::NoSelection);

  {
    QStringList labels;
    labels << tr("Select") << tr("Name");

    _groups.fd->setColumnCount(labels.size());
    _groups.fd->setHeaderLabels(labels);
  }

  if (!_p_cts_mg.isNull()) {
      CTSManager *cts_mg = *_p_cts_mg.data();
      QVector<QSharedPointer<CTGroup*>> grps = cts_mg->GetGroups();

      for (int n = 0; n < grps.size(); n++) {
          QTreeWidgetItem *itm = new QTreeWidgetItem();

          itm->setData(0, Qt::UserRole, QVariant::fromValue<QSharedPointer<CTGroup*>>(grps.at(n)));
          itm->setCheckState(0, Qt::Unchecked);
          itm->setText(1, (*grps.at(n).data())->GetName());
          itm->setToolTip(1, (*grps.at(n).data())->GetDescription());

          _groups.fd->addTopLevelItem(itm);
        }

      _groups.fd->sortByColumn(1, Qt::AscendingOrder);
    }


  // buttons:
  _bts.lay = new QHBoxLayout();
  _bts.ok = new QPushButton(tr("Create"));
  _bts.cancel = new QPushButton(tr("Cancel"));

  connect(_bts.ok, &QPushButton::clicked, this, &EmailCreateForm::OnBT_Ok);
  connect(_bts.cancel, &QPushButton::clicked, this, &EmailCreateForm::close);

  _bts.lay->addWidget(_bts.cancel);
  _bts.lay->addWidget(_bts.ok);


  _mlay->addWidget(_subject.lb, 0, 0, 1, 1);
  _mlay->addWidget(_subject.fd, 0, 1, 1, 1);

  _mlay->addWidget(_dt_exec.lb, 1, 0, 1, 1);
  _mlay->addWidget(_dt_exec.fd, 1, 1, 1, 1);

  _mlay->addWidget(_groups.lb, 2, 0, 1, 1);
  _mlay->addWidget(_groups.fd, 3, 0, 1, 2);

  _mlay->addWidget(_content.lb, 4, 0, 1, 1);
  _mlay->addWidget(_content.fd, 5, 0, 1, 2);

  _mlay->addLayout(_bts.lay, 6, 1, 1, 1, Qt::AlignRight);


  return true;
}



void EmailCreateForm::OnBT_Ok()
{
  QVector<QSharedPointer<CTGroup*>> selected_grps;
  QTreeWidgetItem *itm = nullptr;

  for (int n = 0; n < _groups.fd->topLevelItemCount(); n++) {
      itm = _groups.fd->topLevelItem(n);

      if (itm->checkState(0) == Qt::Checked)
        selected_grps.append( itm->data(0, Qt::UserRole).value<QSharedPointer<CTGroup*>>() );

      itm = nullptr;
    }

  emit SI_ReqEmailCreate(_subject.fd->text().toUtf8(), _content.fd->toHtml().toUtf8(),
                         _dt_exec.fd->dateTime().toMSecsSinceEpoch(), selected_grps);

  this->close();
}
