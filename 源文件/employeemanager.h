#ifndef EMPLOYEEMANAGER_H
#define EMPLOYEEMANAGER_H

#include <QMap>
#include <QVector>

#include "employee.h"

class Engineer;
class Leader;

class EmployeeManager
{
public:
    EmployeeManager();
    ~EmployeeManager();

    bool addEmployee(Employee *emp);
    bool deleteEmployee(const QString &id);
    bool updateEmployee(Employee *emp);
    Employee *findEmployeeById(const QString &id) const;
    QVector<Employee *> searchEmployee(const QString &keyword) const;
    QVector<Employee *> allEmployees() const;

    QVector<Leader *> getAllLeaders() const;
    QVector<Engineer *> getSubordinates(const QString &leaderId) const;
    QString getEmployeeDisplayName(const QString &id) const;
    QString relationshipSummary(const Employee *employee) const;

    bool saveToFile(const QString &filePath) const;
    bool loadFromFile(const QString &filePath);
    void clear();
    void syncRelationships();

    int getTotalCount() const;
    double getAverageSalary() const;
    double getMaxSalary() const;
    double getMinSalary() const;
    double getAverageAge() const;
    QMap<QString, int> getTypeCount() const;
    QMap<QString, double> getAverageSalaryByType() const;

private:
    void freeAll();
    void cleanupInvalidLeaderLinks();

    QVector<Employee *> m_employeeList;
};

#endif // EMPLOYEEMANAGER_H
