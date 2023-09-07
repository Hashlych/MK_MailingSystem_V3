#ifndef EMAILSMANAGER_H
#define EMAILSMANAGER_H

#include <QObject>
#include <QTimer>

#include "emailobject.h"


class EmailsManager : public QObject
{
  Q_OBJECT

public:
  typedef struct S_ML_Timer {
    S_ML_Timer();
    S_ML_Timer(const S_ML_Timer &other);
    ~S_ML_Timer();

    bool operator == (const S_ML_Timer &other) const;
    bool operator != (const S_ML_Timer &other);

    QTimer *tm = nullptr;
    QSharedPointer<EmailObject*> pml = nullptr;
  } S_ML_Timer;


  explicit EmailsManager(QObject *parent = nullptr);
  ~EmailsManager();


  QVector<QSharedPointer<EmailObject*>> GetEmailsList() const;
  QVector<QSharedPointer<EmailObject*>> GetEmailsForProcessing() const;


signals:
  void SI_EmailAdded(QSharedPointer<EmailObject*> pml);
  void SI_EmailDeleted(QSharedPointer<EmailObject*> pml);

  void SI_ReqNT_SendEmail(QSharedPointer<EmailObject*> pml);


public slots:
  void SL_Initialize();

  void SL_AddEmail(const QByteArray &subject, const QByteArray &content, const quint64 &dt_exec = 0,
                   const quint32 &id = 0);
  void SL_AddEmail(EmailObject *ml);

  void SL_DelEmail(QSharedPointer<EmailObject*> pml);


private slots:
  void OnTM_Check();
  void OnTM_ML();


private:
  bool    _init       = false;
  quint16 _max_timers = 1;
  QTimer  *_tm_check  = nullptr;

  QVector<EmailObject*> _emails;
  QVector<QSharedPointer<EmailObject*>>_emails_queue;
  QVector<S_ML_Timer>   _mls_timers;
};

#endif // EMAILSMANAGER_H
