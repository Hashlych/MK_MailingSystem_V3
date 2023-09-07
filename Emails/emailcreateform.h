#ifndef EMAILCREATEFORM_H
#define EMAILCREATEFORM_H

#include <QWidget>
#include <QGridLayout>
#include <QDateTimeEdit>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTreeWidget>

#include "emailsmanager.h"


class EmailCreateForm : public QWidget
{
  Q_OBJECT

public:
  explicit EmailCreateForm(QSharedPointer<CTSManager*> p_cts_mg = QSharedPointer<CTSManager*>(),
                           QWidget *parent = nullptr);

  bool BuildUI();


signals:
  void SI_ReqEmailCreate(const QByteArray &subject, const QByteArray &content, const quint64 &dt_exec,
                         QVector<QSharedPointer<CTGroup*>> groups);


private slots:
  void OnBT_Ok();


private:
  QSharedPointer<CTSManager*> _p_cts_mg;


  QGridLayout *_mlay = nullptr;

  struct {
    QLabel *lb = nullptr;
    QLineEdit *fd = nullptr;
  } _subject;

  struct {
    QLabel *lb = nullptr;
    QDateTimeEdit *fd = nullptr;
  } _dt_exec;

  struct {
    QLabel *lb = nullptr;
    QTextEdit *fd = nullptr;
  } _content;

  struct {
    QLabel *lb = nullptr;
    QTreeWidget *fd = nullptr;
  } _groups;

  struct {
    QHBoxLayout *lay = nullptr;
    QPushButton *ok = nullptr, *cancel = nullptr;
  } _bts;

};

#endif // EMAILCREATEFORM_H
