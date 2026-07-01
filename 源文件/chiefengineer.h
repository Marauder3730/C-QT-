#ifndef CHIEFENGINEER_H
#define CHIEFENGINEER_H

#include "employee.h"

class ChiefEngineer : public Employee
{
public:
    ChiefEngineer();
    ChiefEngineer(const QString &id, const QString &name, const QString &gender,
                  int age, const QDate &hireDate, const QString &techDirection,
                  int leadProjectCount, double techAllowance);

    QString techDirection() const;
    void setTechDirection(const QString &techDirection);

    int leadProjectCount() const;
    void setLeadProjectCount(int leadProjectCount);

    double techAllowance() const;
    void setTechAllowance(double techAllowance);

    double calcSalary() const override;
    QString getType() const override;
    QString getDetail() const override;
    QJsonObject toJson() const override;
    Employee *clone() const override;

private:
    QString m_techDirection;
    int m_leadProjectCount;
    double m_techAllowance;
};

#endif // CHIEFENGINEER_H
