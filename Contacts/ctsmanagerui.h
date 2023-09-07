#ifndef CTSMANAGERUI_H
#define CTSMANAGERUI_H

#include <QWidget>
#include <QGridLayout>
#include <QMessageBox>

#include <QLabel>
#include <QTreeWidget>
#include <QPushButton>

#include "ctsmanager.h"


class CTSManagerUI : public QWidget
{
  Q_OBJECT

public:
  explicit CTSManagerUI(QWidget *parent = nullptr);

  bool BuildUI();


  QVector<QSharedPointer<CTGroup*>> GetSelected_Groups() const;
  QVector<QSharedPointer<CTGroup*>> GetPartiallySelected_Groups() const;
  QVector<QSharedPointer<Contact*>> GetSelected_ContactsFromGroup(const QSharedPointer<CTGroup*> pgrp);
  QVector<QSharedPointer<Contact*>> GetSelected_AllContacts();


signals:
  void SI_Req_NewGroup();
  void SI_Req_DelGroup(QSharedPointer<CTGroup*> pgrp);
  void SI_Req_DelContact(QSharedPointer<Contact*> pct);


public slots:
  void SL_CT_Add(QSharedPointer<Contact*> pct);
  void SL_CT_Del(QSharedPointer<Contact*> pct);

  void SL_GRP_Add(QSharedPointer<CTGroup*> pgrp);
  void SL_GRP_Del(QSharedPointer<CTGroup*> pgrp);


private slots:
  void OnItemClicked(QTreeWidgetItem *item, int col);
  void OnBT_DeleteSelected();

  void OnBT_SelectorAll();
  void OnBT_ExpandAll();
  void OnBT_ExpandSelected();
  void OnBT_CollapseAll();
  void OnBT_CollapseSelected();


private:
  QTreeWidgetItem * GetItemFromGroup(const QSharedPointer<CTGroup*> pgrp);
  QTreeWidgetItem * GetItemFromContact(const QSharedPointer<Contact*> pct);


  QGridLayout *_mlay  = nullptr;
  QLabel      *_lb    = nullptr;
  QTreeWidget *_tree  = nullptr;

  struct {
    QHBoxLayout *lay = nullptr;
    QPushButton *add_grp = nullptr, *del_selected = nullptr;
  } _bts;

  struct {
    QHBoxLayout *lay = nullptr;
    QPushButton *selector_all       = nullptr,
                *expand_all         = nullptr,
                *expand_selected    = nullptr,
                *collapse_all       = nullptr,
                *collapse_selected  = nullptr;

    Qt::CheckState curr_chk_state = Qt::Unchecked;
  } _bts_actions;
};

#endif // CTSMANAGERUI_H
