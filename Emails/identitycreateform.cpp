#include "identitycreateform.h"


#ifdef QT_DEBUG
#include <QDebug>
#endif


IdentityCreateForm::IdentityCreateForm(QWidget *parent)
  : QWidget{parent}
{
}


IdentityCreateForm::~IdentityCreateForm()
{
}



bool IdentityCreateForm::BuildUI()
{
  if (!(_mlay = new QGridLayout()))
    return false;
  else
    this->setLayout(_mlay);


  // name:
  _name.lb = new QLabel(tr("Name:"));
  _name.fd = new QLineEdit();

  // addr:
  _addr.lb = new QLabel(tr("Email address:"));
  _addr.fd = new QLineEdit();

  // buttons:
  _buttons.lay = new QHBoxLayout();
  _buttons.ok = new QPushButton(tr("Ok"));
  _buttons.cancel = new QPushButton(tr("Cancel"));

  connect(_buttons.ok, &QPushButton::clicked, this, &IdentityCreateForm::SL_OnBT_Ok);
  connect(_buttons.cancel, &QPushButton::clicked, this, &IdentityCreateForm::SL_OnBT_Cancel);

  _buttons.lay->addWidget(_buttons.cancel);
  _buttons.lay->addWidget(_buttons.ok);


  // add widgets to main layout:
  _mlay->addWidget(_name.lb, 0, 0, 1, 1);
  _mlay->addWidget(_name.fd, 0, 1, 1, 1);

  _mlay->addWidget(_addr.lb, 1, 0, 1, 1);
  _mlay->addWidget(_addr.fd, 1, 1, 1, 1);

  _mlay->addLayout(_buttons.lay, 2, 1, 1, 1, Qt::AlignRight);

  return true;
}



void IdentityCreateForm::ResetFields()
{
  _name.fd->clear();
  _addr.fd->clear();
}



void IdentityCreateForm::SL_OnBT_Ok()
{
  qDebug().noquote() << QString("[IdentityCreateForm::SL_OnBT_Ok]");

  QByteArray
      name = _name.fd->text().toUtf8(),
      addr = _addr.fd->text().toUtf8();

  if (!name.isEmpty() && !addr.isEmpty()) {

      S_IDT_Datas idt_datas =
      {
        0,
        name,
        addr,
        "", // code
        { // tokens:
          "","","",0
        }
      };

      emit SI_StartGMailProcedure(idt_datas);

      this->hide();
      this->ResetFields();
    }
}



void IdentityCreateForm::SL_OnBT_Cancel()
{
  qDebug().noquote() << QString("[IdentityCreateForm::SL_OnBT_Cancel]");

  this->hide();
  this->ResetFields();
}




void IdentityCreateForm::SetGMailProcedureRunning(const bool &running) { _gmail_procedure = running; }

const bool & IdentityCreateForm::GMailProcedureRunning() const { return _gmail_procedure; }
