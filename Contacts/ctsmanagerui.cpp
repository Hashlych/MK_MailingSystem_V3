#include "ctsmanagerui.h"

CTSManagerUI::CTSManagerUI(QWidget *parent)
  : QWidget{parent}
{
}


/* ---------------- BuildUI ---------------- */
bool CTSManagerUI::BuildUI()
{
  if (!(_mlay = new QGridLayout()))
    return false;
  else
    this->setLayout(_mlay);


  // contacts list:
  _lb = new QLabel(tr("Contacts:"));
  _tree = new QTreeWidget();

  _tree->setColumnCount(2);
  _tree->setHeaderHidden(true);

  connect(_tree, &QTreeWidget::itemClicked, this, &CTSManagerUI::OnItemClicked);


  // Buttons 'add group' and 'delete selected':
  _bts.lay = new QHBoxLayout();
  _bts.add_grp = new QPushButton(tr("New Group"));
  _bts.del_selected = new QPushButton(tr("Delete Selected"));

  _bts.lay->addWidget(_bts.add_grp);
  _bts.lay->addWidget(_bts.del_selected);

  connect(_bts.add_grp, &QPushButton::clicked, this, &CTSManagerUI::SI_Req_NewGroup);
  connect(_bts.del_selected, &QPushButton::clicked, this, &CTSManagerUI::OnBT_DeleteSelected);


  // Buttons actions:
  _bts_actions.lay = new QHBoxLayout();
  _bts_actions.selector_all = new QPushButton(tr("Select All"));
  _bts_actions.expand_all = new QPushButton(tr("Expand All"));
  _bts_actions.expand_selected = new QPushButton(tr("Expand Selected"));
  _bts_actions.collapse_all = new QPushButton(tr("Collapse All"));
  _bts_actions.collapse_selected = new QPushButton(tr("Collapse Selected"));

  _bts_actions.lay->addWidget(_bts_actions.selector_all);
  _bts_actions.lay->addWidget(_bts_actions.expand_all);
  _bts_actions.lay->addWidget(_bts_actions.collapse_all);
  _bts_actions.lay->addWidget(_bts_actions.expand_selected);
  _bts_actions.lay->addWidget(_bts_actions.collapse_selected);

  connect(_bts_actions.selector_all, &QPushButton::clicked, this, &CTSManagerUI::OnBT_SelectorAll);
  connect(_bts_actions.expand_all, &QPushButton::clicked, this, &CTSManagerUI::OnBT_ExpandAll);
  connect(_bts_actions.expand_selected, &QPushButton::clicked, this, &CTSManagerUI::OnBT_ExpandSelected);
  connect(_bts_actions.collapse_all, &QPushButton::clicked, this, &CTSManagerUI::OnBT_CollapseAll);
  connect(_bts_actions.collapse_selected, &QPushButton::clicked, this, &CTSManagerUI::OnBT_CollapseSelected);

  _bts_actions.collapse_all->setHidden(true);
  _bts_actions.collapse_selected->setHidden(true);


  // add elements to main layout:
  _mlay->addLayout(_bts.lay, 0, 0, 1, 1, Qt::AlignLeft);
  _mlay->addWidget(_lb, 1, 0, 1, 1, Qt::AlignLeft);
  _mlay->addLayout(_bts_actions.lay, 2, 0, 1, 1, Qt::AlignLeft);
  _mlay->addWidget(_tree, 3, 0, 1, 2, Qt::AlignLeft);


  return true;
}


/* ---------------- GetSelected_Groups ---------------- */
QVector<QSharedPointer<CTGroup*>> CTSManagerUI::GetSelected_Groups() const
{
  QVector<QSharedPointer<CTGroup*>> selected_grps;

  for (int n = 0; n < _tree->topLevelItemCount(); n++) {

      if (_tree->topLevelItem(n)->checkState(0) == Qt::Checked) {
          QSharedPointer<CTGroup*> pgrp = _tree->topLevelItem(n)->data(0, Qt::UserRole)
              .value<QSharedPointer<CTGroup*>>();

          if (!pgrp.isNull())
            selected_grps.append(pgrp);
        }

    }

  return selected_grps;
}



/* ---------------- GetPartiallySelected_Groups ---------------- */
QVector<QSharedPointer<CTGroup*>> CTSManagerUI::GetPartiallySelected_Groups() const
{
  QVector<QSharedPointer<CTGroup*>> groups;

  for (int n = 0; n < _tree->topLevelItemCount(); n++) {

      if (_tree->topLevelItem(n)->checkState(0) == Qt::PartiallyChecked) {
          QSharedPointer<CTGroup*> pgrp = _tree->topLevelItem(n)->data(0, Qt::UserRole)
              .value<QSharedPointer<CTGroup*>>();

          if (!pgrp.isNull())
            groups.append(pgrp);
        }

    }

  return groups;
}


/* ---------------- GetSelected_ContactsFromGroup ---------------- */
QVector<QSharedPointer<Contact*>> CTSManagerUI::GetSelected_ContactsFromGroup(const QSharedPointer<CTGroup *> pgrp)
{
  QVector<QSharedPointer<Contact*>> cts;

  if (!pgrp.isNull()) {

      QTreeWidgetItem *grp_itm = this->GetItemFromGroup(pgrp);

      if (grp_itm) {
          for (int n = 0; n < grp_itm->childCount(); n++) {
              if (grp_itm->child(n)->checkState(0) == Qt::Checked) {
                  QSharedPointer<Contact*> pct = grp_itm->child(n)->data(0, Qt::UserRole)
                      .value<QSharedPointer<Contact*>>();

                  if (!pct.isNull())
                    cts.append(pct);
                }
            }
        }

    }


  return cts;
}



/* ---------------- GetItemFromGroup ---------------- */
QTreeWidgetItem * CTSManagerUI::GetItemFromGroup(const QSharedPointer<CTGroup *> pgrp)
{
  if (pgrp.isNull())
    return nullptr;

  CTGroup *grp = *pgrp.data(), *tmp_grp = nullptr;
  QTreeWidgetItem *itm = nullptr;

  for (int n = 0; n < _tree->topLevelItemCount(); n++) {
      itm = _tree->topLevelItem(n);
      tmp_grp = *(itm->data(0, Qt::UserRole).value<QSharedPointer<CTGroup*>>());

      if (tmp_grp && tmp_grp == grp) {
          return itm;
        }

      itm = nullptr;
      tmp_grp = nullptr;
    }

  return nullptr;
}



/* ---------------- GetItemFromContact ---------------- */
QTreeWidgetItem * CTSManagerUI::GetItemFromContact(const QSharedPointer<Contact *> pct)
{
  if (pct.isNull())
    return nullptr;

  Contact *ct = *pct.data();
  CTGroup *grp = dynamic_cast<CTGroup*>(ct->parent());

  if (!grp)
    return nullptr;

  QTreeWidgetItem *grp_itm = this->GetItemFromGroup(QSharedPointer<CTGroup*>::create(grp));

  if (grp_itm) {
      Contact *tmp_ct = nullptr;
      QTreeWidgetItem *itm = nullptr;

      for (int n = 0; n < grp_itm->childCount(); n++) {
          itm = grp_itm->child(n);
          tmp_ct = *(itm->data(0, Qt::UserRole).value<QSharedPointer<Contact*>>());

          if (tmp_ct && tmp_ct == ct) {
              return itm;
            }

          itm = nullptr;
          tmp_ct = nullptr;
        }
    }

  return nullptr;
}



/* ---------------- SL_CT_Add ---------------- */
void CTSManagerUI::SL_CT_Add(QSharedPointer<Contact*> pct)
{
  if (pct.isNull())
    return;

  Contact *ct = *pct.data();
  CTGroup *grp = dynamic_cast<CTGroup*>(ct->parent());

  if (!grp)
    return;


  //QTreeWidgetItem *grp_item = nullptr;
  QTreeWidgetItem *grp_item = this->GetItemFromGroup(QSharedPointer<CTGroup*>::create(grp));

  if (!grp_item)
    return;


  QTreeWidgetItem *item = new QTreeWidgetItem();

  item->setData(0, Qt::UserRole, QVariant::fromValue(pct));
  item->setCheckState(0, Qt::Unchecked);
  item->setText(1, ct->GetFullName());
  item->setToolTip(1, ct->GetEmail());

  grp_item->addChild(item);
  grp_item->sortChildren(1, Qt::AscendingOrder);
}



/* ---------------- SL_CT_Del ---------------- */
void CTSManagerUI::SL_CT_Del(QSharedPointer<Contact*> pct)
{
  if (pct.isNull())
    return;

  Contact *ct = *pct.data();
  CTGroup *grp = dynamic_cast<CTGroup*>(ct->parent()),
      *tmp_grp = nullptr;
  QTreeWidgetItem *tmp_itm = nullptr;

  for (int n = 0; n < _tree->topLevelItemCount(); n++) {
      tmp_itm = _tree->topLevelItem(n);
      tmp_grp = *(tmp_itm->data(0, Qt::UserRole).value<QSharedPointer<CTGroup*>>());

      if (tmp_grp && tmp_grp == grp) {

          Contact *tmp_ct = nullptr;

          for (int i = 0; i < tmp_itm->childCount(); i++) {
              tmp_ct = *(tmp_itm->child(i)->data(0, Qt::UserRole).value<QSharedPointer<Contact*>>());

              if (tmp_ct && tmp_ct == ct) {
                  QTreeWidgetItem *ct_itm = tmp_itm->takeChild(i);

                  delete ct;
                  ct = nullptr;

                  delete ct_itm;
                  ct_itm = nullptr;

                  tmp_itm->sortChildren(1, Qt::AscendingOrder);

                  if (this->GetSelected_ContactsFromGroup(QSharedPointer<CTGroup*>::create(tmp_grp)).size() == 0)
                    tmp_itm->setCheckState(0, Qt::Unchecked);

                  break;
                }

              tmp_ct = nullptr;
            }

          break;
        }

      tmp_grp = nullptr;
      tmp_itm = nullptr;
    }
}



/* ---------------- SL_GRP_Add ---------------- */
void CTSManagerUI::SL_GRP_Add(QSharedPointer<CTGroup*> pgrp)
{
  if (pgrp.isNull())
    return;

  CTGroup *grp = *pgrp.data();

  QTreeWidgetItem *item = new QTreeWidgetItem();

  item->setData(0, Qt::UserRole, QVariant::fromValue(pgrp));
  item->setCheckState(0, Qt::Unchecked);
  item->setText(1, grp->GetName());
  item->setToolTip(1, grp->GetDescription());

  _tree->addTopLevelItem(item);
  _tree->sortByColumn(1, Qt::SortOrder::AscendingOrder);
}



/* ---------------- SL_GRP_Del ---------------- */
void CTSManagerUI::SL_GRP_Del(QSharedPointer<CTGroup*> pgrp)
{
  if (pgrp.isNull())
    return;

  CTGroup *grp = *pgrp.data(), *tmp_grp = nullptr;

  for (int n = 0; n < _tree->topLevelItemCount(); n++) {
      tmp_grp = *(_tree->topLevelItem(n)->data(0, Qt::UserRole).value<QSharedPointer<CTGroup*>>().data());

      if (tmp_grp && tmp_grp == grp) {
          QTreeWidgetItem *itm = _tree->takeTopLevelItem(n);

          delete grp;
          grp = nullptr;

          delete itm;
          itm = nullptr;

          if (_tree->topLevelItemCount() > 0)
            _tree->sortByColumn(1, Qt::AscendingOrder);

          return;
        }

      tmp_grp = nullptr;
    }
}



/* ---------------- OnItemClicked ---------------- */
void CTSManagerUI::OnItemClicked(QTreeWidgetItem *item, int col)
{
  if (!item)
    return;


  if (item && col == 0) {
        // retrieve data from item (either a CTGroup or a Contact):
        QSharedPointer<CTGroup*> grp  = item->data(0, Qt::UserRole).value<QSharedPointer<CTGroup*>>();
        QSharedPointer<Contact*> ct   = item->data(0, Qt::UserRole).value<QSharedPointer<Contact*>>();

        // data == group:
        if (!grp.isNull()) {

            // check all contacts within group:
            if (item->checkState(0) == Qt::Checked) {
                for (int n = 0; n < item->childCount(); n++) {
                    item->child(n)->setCheckState(0, Qt::Checked);
                  }
              }
            // unchecked all contacts within group:
            else if (item->checkState(0) == Qt::Unchecked) {
                for (int n = 0; n < item->childCount(); n++) {
                    item->child(n)->setCheckState(0, Qt::Unchecked);
                  }
              }

            // no need to go further:
            return;
          }


        // data == contact:
        if (!ct.isNull()) {
            // parent item:
            QTreeWidgetItem *pItm = item->parent();

            if (!pItm)
              return;

            // count contacts checked and unchecked:
            int itm_checked = 0, itm_unchecked = 0;

            for (int n = 0; n < pItm->childCount(); n++) {
                if (pItm->child(n)->checkState(0) == Qt::Checked)
                  itm_checked++;
                else if (pItm->child(n)->checkState(0) == Qt::Unchecked)
                  itm_unchecked++;
              }

            // if all contacts are checked - check parent
            // if all contacts are unchecked - uncheck parent
            // if both - partially check parent
            if (itm_checked > 0 && itm_unchecked > 0)
              pItm->setCheckState(0, Qt::PartiallyChecked);
            else if (itm_checked == 0 && itm_unchecked > 0)
              pItm->setCheckState(0, Qt::Unchecked);
            else if (itm_checked > 0 && itm_unchecked == 0)
              pItm->setCheckState(0, Qt::Checked);
          }
      }
}



/* ---------------- OnBT_DeleteSelected ---------------- */
void CTSManagerUI::OnBT_DeleteSelected()
{
  QVector<QSharedPointer<CTGroup*>>
      selected_grps = this->GetSelected_Groups(),
      partiallySelected_grps = this->GetPartiallySelected_Groups();

  QVector<QSharedPointer<Contact*>> selected_cts;
  QVector<QSharedPointer<CTGroup*>>::const_iterator it, ite;
  QSharedPointer<CTGroup*> default_group = nullptr;


  // retrieve default group:
  it  = selected_grps.constBegin();
  ite = selected_grps.constEnd();

  for (; it != ite; it++) {
      if ( (*(*it).data())->GetName().compare("Default", Qt::CaseSensitive) == 0) {
          default_group = (*it);
          break;
        }
    }


  // get and add selected contacts from default group:
  selected_cts.append(this->GetSelected_ContactsFromGroup(default_group));


  // get selected contacts from partially checked groups:
  it  = partiallySelected_grps.constBegin();
  ite = partiallySelected_grps.constEnd();

  for (; it != ite; it++)
    selected_cts.append(this->GetSelected_ContactsFromGroup( (*it) ));


  {
    QMessageBox msgb(QMessageBox::Warning, tr("Attention required"),
                     tr("You are about to delete selected items. Continue ?"),
                     QMessageBox::No | QMessageBox::Yes);

    int rep = msgb.exec();

    if (rep == QMessageBox::No)
      return;
  }


  // ask user if delete groups or just the contacts within it:
  QMessageBox msgb(QMessageBox::Warning, tr("Attention required"),
                   tr("Do you want to delete selected groups as well ?"
                      "<br />(otherwise, deletes only the contacts "
                      "inside selected groups)"),
                   QMessageBox::No | QMessageBox::Yes);

  int rep = msgb.exec();
  bool req_del_grps = false;

  if (rep == QMessageBox::Yes) {
      req_del_grps = true;
      for (int n = 0; n < selected_grps.size(); n++) {
          if ( (*selected_grps.at(n).data()) != (*default_group.data()) )
            selected_cts.append((*selected_grps.at(n).data())->GetContactsList());
        }
    } else if (rep == QMessageBox::No) {
      for (int n = 0; n < selected_grps.size(); n++) {
          if ( (*selected_grps.at(n).data()) != (*default_group.data()) )
            selected_cts.append(this->GetSelected_ContactsFromGroup(selected_grps.at(n)));
        }
    }

  // request delete selected contacts:
  for (int n = 0; n < selected_cts.size(); n++)
    emit SI_Req_DelContact(selected_cts.at(n));

  if (req_del_grps) {
      for (int n = 0; n < selected_grps.size(); n++) {
          if ( (*selected_grps.at(n).data()) != (*default_group.data()) )
            emit SI_Req_DelGroup(selected_grps.at(n));
        }
    }

  /*for (int n = 0; n < selected_cts.size(); n++) {
      qDebug().noquote() << QString("selected_cts.at(%1) = 0x%2 | %3")
                            .arg(n, 3, 10, QChar('0'))
                            .arg((qint64)(*selected_cts.at(n).data()), 8, 16, QChar('0'))
                            .arg((*selected_cts.at(n).data())->GetFullName());
    }*/
}




void CTSManagerUI::OnBT_SelectorAll()
{
  if (_bts_actions.curr_chk_state == Qt::Unchecked) {
      _bts_actions.curr_chk_state = Qt::Checked;
      _bts_actions.selector_all->setText(tr("Deselect All"));
  } else if (_bts_actions.curr_chk_state == Qt::Checked) {
      _bts_actions.curr_chk_state = Qt::Unchecked;
      _bts_actions.selector_all->setText(tr("Select All"));
    }

  for (int n = 0; n < _tree->topLevelItemCount(); n++) {
      _tree->topLevelItem(n)->setCheckState(0, _bts_actions.curr_chk_state);

      for (int i = 0; i < _tree->topLevelItem(n)->childCount(); i++)
        _tree->topLevelItem(n)->child(i)->setCheckState(0, _bts_actions.curr_chk_state);
    }
}



void CTSManagerUI::OnBT_ExpandAll()
{
  for (int n = 0; n < _tree->topLevelItemCount(); n++) {
      if (!_tree->topLevelItem(n)->isExpanded())
        _tree->topLevelItem(n)->setExpanded(true);
    }

  _bts_actions.expand_all->setHidden(true);
  _bts_actions.collapse_all->setHidden(false);
}



void CTSManagerUI::OnBT_ExpandSelected()
{
  for (int n = 0; n < _tree->topLevelItemCount(); n++) {
      if (_tree->topLevelItem(n)->checkState(0) == Qt::Checked && !_tree->topLevelItem(n)->isExpanded())
        _tree->topLevelItem(n)->setExpanded(true);
    }

  _bts_actions.expand_selected->setHidden(true);
  _bts_actions.collapse_selected->setHidden(false);
}


void CTSManagerUI::OnBT_CollapseAll()
{
  for (int n = 0; n < _tree->topLevelItemCount(); n++) {
      if (_tree->topLevelItem(n)->isExpanded())
        _tree->topLevelItem(n)->setExpanded(false);
    }

  _bts_actions.expand_all->setHidden(false);
  _bts_actions.collapse_all->setHidden(true);
}


void CTSManagerUI::OnBT_CollapseSelected()
{
  for (int n = 0; n < _tree->topLevelItemCount(); n++) {
      if (_tree->topLevelItem(n)->checkState(0) == Qt::Checked && _tree->topLevelItem(n)->isExpanded())
        _tree->topLevelItem(n)->setExpanded(false);
    }

  _bts_actions.expand_selected->setHidden(false);
  _bts_actions.collapse_selected->setHidden(true);
}



QVector<QSharedPointer<Contact*>> CTSManagerUI::GetSelected_AllContacts()
{
  QVector<QSharedPointer<CTGroup*>>
      selected_grps = this->GetSelected_Groups(),
      partiallySelected_grps = this->GetPartiallySelected_Groups();

  QVector<QSharedPointer<Contact*>> selected_cts;
  QVector<QSharedPointer<CTGroup*>>::const_iterator it, ite;
  QSharedPointer<CTGroup*> default_group = nullptr;


  // retrieve default group:
  it  = selected_grps.constBegin();
  ite = selected_grps.constEnd();

  for (; it != ite; it++) {
      if ( (*(*it).data())->GetName().compare("Default", Qt::CaseSensitive) == 0 && !default_group) {
          default_group = (*it);
        } else {
          selected_cts.append(this->GetSelected_ContactsFromGroup((*it)));
        }
    }


  // get and add selected contacts from default group:
  selected_cts.append(this->GetSelected_ContactsFromGroup(default_group));


  // get selected contacts from partially checked groups:
  it  = partiallySelected_grps.constBegin();
  ite = partiallySelected_grps.constEnd();

  for (; it != ite; it++)
    selected_cts.append(this->GetSelected_ContactsFromGroup( (*it) ));

  return selected_cts;
}
