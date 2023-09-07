#ifndef IDENTITYCREATEFORM_H
#define IDENTITYCREATEFORM_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

#include "Emails/identitiesmanager.h"


class IdentityCreateForm : public QWidget
{
  Q_OBJECT

public:
  explicit IdentityCreateForm(QWidget *parent = nullptr);
  ~IdentityCreateForm();

  bool BuildUI();
  void ResetFields();


  void SetGMailProcedureRunning(const bool &running);

  const bool & GMailProcedureRunning() const;


signals:
  void SI_StartGMailProcedure(S_IDT_Datas &idt_datas);


private slots:
  void SL_OnBT_Ok();
  void SL_OnBT_Cancel();


private:
  bool _gmail_procedure = false;

  QGridLayout *_mlay = nullptr;

  struct {
    QLabel    *lb = nullptr;
    QLineEdit *fd = nullptr;
  } _name, _addr;

  struct {
    QHBoxLayout *lay = nullptr;
    QPushButton *ok = nullptr, *cancel = nullptr;
  } _buttons;
};

#endif // IDENTITYCREATEFORM_H
