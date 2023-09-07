#ifndef MLSPROCESSOR_H
#define MLSPROCESSOR_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "emailsmanager.h"


class MLSProcessor : public QObject
{
  Q_OBJECT

public:
  explicit MLSProcessor(QSharedPointer<EmailsManager*> _p_mls_mg = QSharedPointer<EmailsManager*>(),
                        QObject *parent = nullptr);
  ~MLSProcessor();


  qint8 EmailObjectExists(QSharedPointer<EmailObject*> pml);


signals:
  void SI_ReqNT_SendEmail(QSharedPointer<EmailObject*> pml);


public slots:
  void SL_Initialize();
  void SL_AddToWatchList(QSharedPointer<EmailObject*> pml);


private slots:
  void OnTM_Timeout();
  void OnTM_Check();


private:
  typedef struct S_ML {
    QTimer *tm = nullptr;
    QSharedPointer<EmailObject*> pml;
  } S_ML;

  QSharedPointer<EmailsManager*> _p_mls_mg;
  QVector<S_ML> _watch_list;
  QTimer *_tm_check = nullptr;
  bool _init = false;
};

#endif // MLSPROCESSOR_H
