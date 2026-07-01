#ifndef ENGINEER_H
#define ENGINEER_H

#include "employee.h"

#include <QStringList>

class Engineer : public Employee
{
public:
    Engineer();
    Engineer(const QString &id, const QString &name, const QString &gender,
             int age, const QDate &hireDate, const QString &title,
             const QStringList &projects = QStringList(), const QString &leaderId = QString());

    QString title() const;
    void setTitle(const QString &title);

    QStringList projects() const;
    void setProjects(const QStringList &projects);
    int projectCount() const;

    QString leaderId() const;
    void setLeaderId(const QString &leaderId);

    double calcSalary() const override;
    QString getType() const override;
    QString getDetail() const override;
    QJsonObject toJson() const override;
    Employee *clone() const override;

private:
    QString m_title;
    QStringList m_projects;
    QString m_leaderId;
};

#endif // ENGINEER_H
