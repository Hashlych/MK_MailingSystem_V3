#include "identitiesui.h"


IdentitiesUI::IdentitiesUI(QWidget *parent)
  : QWidget{parent}
{
}


IdentitiesUI::~IdentitiesUI()
{
}



// BuildUI:
bool IdentitiesUI::BuildUI()
{
  // Main layout:
  if (!(_mlay = new QGridLayout()))
    return false;
  else
    this->setLayout(_mlay);


  // List:
  _list = new QListWidget();

  _list->setEditTriggers(QListWidget::NoEditTriggers);


  // Buttons:
  _buttons.lay = new QHBoxLayout();
  _buttons.add = new QPushButton(tr("Add"));

  connect(_buttons.add, &QPushButton::clicked, this, &IdentitiesUI::SL_OnBT_Add);

  _buttons.lay->addWidget(_buttons.add);


  // Create form:
  _create_form = new IdentityCreateForm();

  if (!_create_form->BuildUI())
    return false;

  _create_form->setWindowModality(Qt::ApplicationModal);
  _create_form->setHidden(true);


  // Add widgets to main layout:
  _mlay->addLayout(_buttons.lay, 0, 0, 1, 1, Qt::AlignRight);
  _mlay->addWidget(_list, 1, 0, 1, 1);


  return true;
}



// SL_AddIdentityToUI:
void IdentitiesUI::SL_AddIdentityToUI(QSharedPointer<Identity*> &idt)
{
  if (idt.isNull())
    return;

  QListWidgetItem *itm = new QListWidgetItem();

  itm->setData(Qt::UserRole, QVariant::fromValue(idt));
  itm->setText((*idt.data())->GetName());
  itm->setCheckState(Qt::Unchecked);
  itm->setToolTip((*idt.data())->GetAddress());

  _list->addItem(itm);
}



// SL_OnBT_Add:
void IdentitiesUI::SL_OnBT_Add()
{
  _create_form->show();

  /*QString name = QInputDialog::getText(nullptr, tr("New Identity"),
                                       tr("Name:"), QLineEdit::EchoMode::Normal,
                                       tr("example_name"));

  if (name.isEmpty())
    return;


  QString addr = QInputDialog::getText(nullptr, tr("New Identity"),
                                       tr("Email address:"), QLineEdit::EchoMode::Normal,
                                       tr("example.address@gmail.com"));

  if (addr.isEmpty())
    return;


  emit SI_DBReq_Insert(S_IDT_Datas
                       {0,
                        name.toUtf8(),
                        addr.toUtf8(),
                        "",
                        {"","","",0}
                       });*/
}
