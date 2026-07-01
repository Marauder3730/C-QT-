#include "employeemanager.h"

#include "engineer.h"
#include "leader.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

EmployeeManager::EmployeeManager()
{
}

EmployeeManager::~EmployeeManager()
{
    freeAll();
}

void EmployeeManager::freeAll()
{
    qDeleteAll(m_employeeList);
    m_employeeList.clear();
}

bool EmployeeManager::addEmployee(Employee *emp)
{
    if (!emp || emp->id().isEmpty()) {
        return false;
    }

    if (findEmployeeById(emp->id())) {
        return false;
    }

    m_employeeList.append(emp);
    syncRelationships();
    return true;
}

bool EmployeeManager::deleteEmployee(const QString &id)
{
    for (int i = 0; i < m_employeeList.size(); ++i) {
        if (m_employeeList.at(i)->id() == id) {
            delete m_employeeList.takeAt(i);
            syncRelationships();
            return true;
        }
    }
    return false;
}

bool EmployeeManager::updateEmployee(Employee *emp)
{
    if (!emp || emp->id().isEmpty()) {
        return false;
    }

    for (int i = 0; i < m_employeeList.size(); ++i) {
        if (m_employeeList.at(i)->id() == emp->id()) {
            delete m_employeeList[i];
            m_employeeList[i] = emp;
            syncRelationships();
            return true;
        }
    }
    return false;
}

Employee *EmployeeManager::findEmployeeById(const QString &id) const
{
    for (Employee *emp : m_employeeList) {
        if (emp->id() == id) {
            return emp;
        }
    }
    return nullptr;
}

QVector<Employee *> EmployeeManager::searchEmployee(const QString &keyword) const
{
    QVector<Employee *> result;
    const QString key = keyword.trimmed();

    if (key.isEmpty()) {
        return m_employeeList;
    }

    for (Employee *emp : m_employeeList) {
        if (emp->id().contains(key, Qt::CaseInsensitive)
            || emp->name().contains(key, Qt::CaseInsensitive)) {
            result.append(emp);
        }
    }
    return result;
}

QVector<Employee *> EmployeeManager::allEmployees() const
{
    return m_employeeList;
}

bool EmployeeManager::saveToFile(const QString &filePath) const
{
    QJsonArray array;
    for (Employee *emp : m_employeeList) {
        array.append(emp->toJson());
    }

    QJsonDocument doc(array);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool EmployeeManager::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) {
        return false;
    }

    clear();

    const QJsonArray array = doc.array();
    for (const QJsonValue &value : array) {
        if (!value.isObject()) {
            continue;
        }

        Employee *emp = Employee::fromJson(value.toObject());
        if (emp) {
            m_employeeList.append(emp);
        }
    }

    syncRelationships();
    return true;
}

void EmployeeManager::syncRelationships()
{
    cleanupInvalidLeaderLinks();

    for (Employee *emp : m_employeeList) {
        Leader *leader = dynamic_cast<Leader *>(emp);
        if (!leader) {
            continue;
        }

        const QVector<Engineer *> subordinates = getSubordinates(leader->id());
        leader->setSubordinateCount(subordinates.size());
    }
}

void EmployeeManager::cleanupInvalidLeaderLinks()
{
    for (Employee *emp : m_employeeList) {
        Engineer *engineer = dynamic_cast<Engineer *>(emp);
        if (!engineer || engineer->leaderId().isEmpty()) {
            continue;
        }

        Employee *leader = findEmployeeById(engineer->leaderId());
        if (!leader || !dynamic_cast<Leader *>(leader)) {
            engineer->setLeaderId(QString());
        }
    }
}

QVector<Leader *> EmployeeManager::getAllLeaders() const
{
    QVector<Leader *> leaders;
    for (Employee *emp : m_employeeList) {
        if (Leader *leader = dynamic_cast<Leader *>(emp)) {
            leaders.append(leader);
        }
    }
    return leaders;
}

QVector<Engineer *> EmployeeManager::getSubordinates(const QString &leaderId) const
{
    QVector<Engineer *> result;
    if (leaderId.isEmpty()) {
        return result;
    }

    for (Employee *emp : m_employeeList) {
        Engineer *engineer = dynamic_cast<Engineer *>(emp);
        if (engineer && engineer->leaderId() == leaderId) {
            result.append(engineer);
        }
    }
    return result;
}

QString EmployeeManager::getEmployeeDisplayName(const QString &id) const
{
    if (id.isEmpty()) {
        return QStringLiteral("未指定");
    }

    const Employee *employee = findEmployeeById(id);
    if (!employee) {
        return id;
    }
    return QStringLiteral("%1 (%2)").arg(employee->name(), employee->id());
}

QString EmployeeManager::relationshipSummary(const Employee *employee) const
{
    if (!employee) {
        return QString();
    }

    if (const Engineer *engineer = dynamic_cast<const Engineer *>(employee)) {
        const QString projects = engineer->projects().isEmpty()
                                     ? QStringLiteral("暂无项目")
                                     : engineer->projects().join(QStringLiteral("、"));
        return QStringLiteral("参与项目：%1\n直属领导：%2")
            .arg(projects, getEmployeeDisplayName(engineer->leaderId()));
    }

    if (const Leader *leader = dynamic_cast<const Leader *>(employee)) {
        const QVector<Engineer *> subordinates = getSubordinates(leader->id());
        if (subordinates.isEmpty()) {
            return QStringLiteral("当前暂无已关联的下属工程师");
        }

        QStringList names;
        for (const Engineer *engineer : subordinates) {
            names.append(QStringLiteral("%1 (%2)").arg(engineer->name(), engineer->id()));
        }
        return QStringLiteral("下属工程师（%1人）：\n%2")
            .arg(subordinates.size())
            .arg(names.join(QStringLiteral("\n")));
    }

    return QString();
}

void EmployeeManager::clear()
{
    freeAll();
}

int EmployeeManager::getTotalCount() const
{
    return m_employeeList.size();
}

double EmployeeManager::getAverageSalary() const
{
    if (m_employeeList.isEmpty()) {
        return 0.0;
    }

    double total = 0.0;
    for (Employee *emp : m_employeeList) {
        total += emp->calcSalary();
    }
    return total / m_employeeList.size();
}

double EmployeeManager::getMaxSalary() const
{
    if (m_employeeList.isEmpty()) {
        return 0.0;
    }

    double maxSalary = m_employeeList.first()->calcSalary();
    for (Employee *emp : m_employeeList) {
        maxSalary = qMax(maxSalary, emp->calcSalary());
    }
    return maxSalary;
}

double EmployeeManager::getMinSalary() const
{
    if (m_employeeList.isEmpty()) {
        return 0.0;
    }

    double minSalary = m_employeeList.first()->calcSalary();
    for (Employee *emp : m_employeeList) {
        minSalary = qMin(minSalary, emp->calcSalary());
    }
    return minSalary;
}

double EmployeeManager::getAverageAge() const
{
    if (m_employeeList.isEmpty()) {
        return 0.0;
    }

    double total = 0.0;
    for (Employee *emp : m_employeeList) {
        total += emp->age();
    }
    return total / m_employeeList.size();
}

QMap<QString, int> EmployeeManager::getTypeCount() const
{
    QMap<QString, int> counts;
    for (Employee *emp : m_employeeList) {
        counts[emp->getType()]++;
    }
    return counts;
}

QMap<QString, double> EmployeeManager::getAverageSalaryByType() const
{
    QMap<QString, double> salarySum;
    QMap<QString, int> typeCount;

    for (Employee *emp : m_employeeList) {
        const QString type = emp->getType();
        salarySum[type] += emp->calcSalary();
        typeCount[type]++;
    }

    QMap<QString, double> averages;
    for (auto it = salarySum.constBegin(); it != salarySum.constEnd(); ++it) {
        const int count = typeCount.value(it.key(), 0);
        if (count > 0) {
            averages[it.key()] = it.value() / count;
        }
    }
    return averages;
}
