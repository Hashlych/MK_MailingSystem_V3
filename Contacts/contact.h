#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>

class Contact : public QObject
{
  Q_OBJECT

public:
  // Constructors:
  explicit Contact(QObject *parent = nullptr);
  Contact(const QByteArray &firstName, const QByteArray &lastName,
          const QByteArray &email, const quint32 &id = 0, QObject *parent = nullptr);
  Contact(const Contact &other);

  // Destructor:
  ~Contact();


  // Operators:
  bool operator == (const Contact &other);
  bool operator != (const Contact &other);
  Contact * operator = (const Contact &other);


  // Mutators:
  void SetID(const quint32 &id);
  void SetFirstName(const QByteArray &name);
  void SetLastName(const QByteArray &name);
  void SetEmail(const QByteArray &email);


  // Accessors:
  const quint32 & GetID() const;
  const QByteArray & GetFirstName() const;
  const QByteArray & GetLastName() const;
  const QByteArray & GetEmail() const;
  const QByteArray GetFullName() const;


signals:
  void DatasChanged();


private:
  quint32 _id = 0;
  QByteArray  _firstName, _lastName, _email;
};


Q_DECLARE_METATYPE(Contact*)


#endif // CONTACT_H
