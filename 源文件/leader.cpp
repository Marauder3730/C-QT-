#include "leader.h"

Leader::Leader()
    : m_subordinateCount(0)
{
}

Leader::Leader(const QString &id, const QString &name, const QString &gender,
               int age, const QDate &hireDate, const QString &department,
               const QString &level, int subordinateCount)
    : Employee(id, name, gender, age, hireDate)
    , m_department(department)
    , m_level(level)
    , m_subordinateCount(subordinateCount)
{
}

QString Leader::department() const
{
    return m_department;
}

void Leader::setDepartment(const QString &department)
{
    m_department = department;
}

QString Leader::level() const
{
    return m_level;
}

void Leader::setLevel(const QString &level)
{
    m_level = level;
}

int Leader::subordinateCount() const
{
    return m_subordinateCount;
}

void Leader::setSubordinateCount(int subordinateCount)
{
    m_subordinateCount = subordinateCount;
}

double Leader::calcSalary() const
{
    return 5000.0 + 2000.0 + m_subordinateCount * 100.0;
}

QString Leader::getType() const
{
    return QStringLiteral("领导");
}

QString Leader::getDetail() const
{
    return QStringLiteral("部门：%1，级别：%2，下属：%3人")
        .arg(m_department)
        .arg(m_level)
        .arg(m_subordinateCount);
}

QJsonObject Leader::toJson() const
{
    QJsonObject obj = Employee::toJson();
    obj["department"] = m_department;
    obj["level"] = m_level;
    obj["subordinateCount"] = m_subordinateCount;
    return obj;
}

Employee *Leader::clone() const
{
    Leader *copy = new Leader(id(), name(), gender(), age(), hireDate(),
                              m_department, m_level, m_subordinateCount);
    copyBaseFieldsTo(copy);
    return copy;
}
