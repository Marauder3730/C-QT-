#ifndef LEADER_H
#define LEADER_H

#include "employee.h"

class Leader : public Employee
{
public:
    Leader();
    Leader(const QString &id, const QString &name, const QString &gender,
           int age, const QDate &hireDate, const QString &department,
           const QString &level, int subordinateCount);

    QString department() const;
    void setDepartment(const QString &department);

    QString level() const;
    void setLevel(const QString &level);

    int subordinateCount() const;
    void setSubordinateCount(int subordinateCount);

    double calcSalary() const override;
    QString getType() const override;
    QString getDetail() const override;
    QJsonObject toJson() const override;
    Employee *clone() const override;

private:
    QString m_department;
    QString m_level;
    int m_subordinateCount;
};

#endif // LEADER_H
