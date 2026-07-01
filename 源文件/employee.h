#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QByteArray>
#include <QDate>
#include <QJsonObject>
#include <QPixmap>
#include <QString>

class Employee
{
public:
    Employee();
    Employee(const QString &id, const QString &name, const QString &gender,
             int age, const QDate &hireDate);
    virtual ~Employee();

    QString id() const;
    void setId(const QString &id);

    QString name() const;
    void setName(const QString &name);

    QString gender() const;
    void setGender(const QString &gender);

    int age() const;
    void setAge(int age);

    QDate hireDate() const;
    void setHireDate(const QDate &hireDate);

    QByteArray photoData() const;
    void setPhotoData(const QByteArray &photoData);
    bool hasPhoto() const;
    QPixmap photoPixmap(const QSize &size = QSize(120, 150)) const;

    virtual double calcSalary() const = 0;
    virtual QString getType() const = 0;
    virtual QString getDetail() const = 0;
    virtual QJsonObject toJson() const;
    virtual Employee *clone() const = 0;

    static Employee *fromJson(const QJsonObject &obj);

protected:
    QJsonObject baseToJson() const;
    void copyBaseFieldsTo(Employee *target) const;

private:
    QString m_id;
    QString m_name;
    QString m_gender;
    int m_age;
    QDate m_hireDate;
    QByteArray m_photoData;
};

#endif // EMPLOYEE_H
