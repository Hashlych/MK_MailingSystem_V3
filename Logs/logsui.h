#ifndef LOGSUI_H
#define LOGSUI_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QTextEdit>

#include "logsmanager.h"


class LogsUI : public QWidget
{
  Q_OBJECT

public:
  explicit LogsUI(QWidget *parent = nullptr);

  bool BuildUI();


public slots:
  void SL_AddToDisplay(QVector<QSharedPointer<LogObject*>> logs);


private:
  QGridLayout *_mlay    = nullptr;
  QLabel      *_lb      = nullptr;
  QTextEdit   *_display = nullptr;
};

#endif // LOGSUI_H
