#ifndef EMAILSLISTUI_H
#define EMAILSLISTUI_H

#include <QWidget>
#include <QGridLayout>
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>

#include "emailsmanager.h"


class EmailsListUI : public QWidget
{
  Q_OBJECT

public:
  explicit EmailsListUI(QWidget *parent = nullptr);
  ~EmailsListUI();

  bool BuildUI();

  QVector<QSharedPointer<EmailObject*>> GetEmails_Selected() const;


signals:
  void SI_ReqDelEmail(QSharedPointer<EmailObject*> pml);


public slots:
  void SL_OnReqUpdate_Timers();
  void SL_OnReqUpdate_Item(QSharedPointer<EmailObject*> pml);
  void SL_AddEmail(QSharedPointer<EmailObject*> pml);
  void SL_DelEmail(QSharedPointer<EmailObject*> pml);


private:
  void OnBT_DelSelected();


private:
  QGridLayout   *_mlay = nullptr;
  QTableWidget *_table = nullptr;

  struct {
    QHBoxLayout *lay = nullptr;
    QPushButton *del_selected = nullptr;
  } _bts;
};

#endif // EMAILSLISTUI_H
