#ifndef IDENTITIESUI_H
#define IDENTITIESUI_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>

#include <QInputDialog>

#include "Emails/identitiesmanager.h"
#include "Emails/identitycreateform.h"


class IdentitiesUI : public QWidget
{
  Q_OBJECT

public:
  explicit IdentitiesUI(QWidget *parent = nullptr);
  ~IdentitiesUI();

  bool BuildUI();


signals:
  void SI_DBReq_Insert(const S_IDT_Datas &idt_datas);


public slots:
  void SL_AddIdentityToUI(QSharedPointer<Identity*> &idt);


private slots:
  void SL_OnBT_Add();


private:
  QGridLayout *_mlay = nullptr;
  QListWidget *_list = nullptr;

  struct {
    QHBoxLayout *lay = nullptr;
    QPushButton *add = nullptr;
  } _buttons;

  IdentityCreateForm *_create_form = nullptr;
};

#endif // IDENTITIESUI_H
