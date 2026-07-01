#include "chiefengineer.h"

ChiefEngineer::ChiefEngineer()
    : m_leadProjectCount(0)
    , m_techAllowance(1500.0)
{
}

ChiefEngineer::ChiefEngineer(const QString &id, const QString &name, const QString &gender,
                             int age, const QDate &hireDate, const QString &techDirection,
                             int leadProjectCount, double techAllowance)
    : Employee(id, name, gender, age, hireDate)
    , m_techDirection(techDirection)
    , m_leadProjectCount(leadProjectCount)
    , m_techAllowance(techAllowance)
{
}

QString ChiefEngineer::techDirection() const
{
    return m_techDirection;
}

void ChiefEngineer::setTechDirection(const QString &techDirection)
{
    m_techDirection = techDirection;
}

int ChiefEngineer::leadProjectCount() const
{
    return m_leadProjectCount;
}

void ChiefEngineer::setLeadProjectCount(int leadProjectCount)
{
    m_leadProjectCount = leadProjectCount;
}

double ChiefEngineer::techAllowance() const
{
    return m_techAllowance;
}

void ChiefEngineer::setTechAllowance(double techAllowance)
{
    m_techAllowance = techAllowance;
}

double ChiefEngineer::calcSalary() const
{
    return 4000.0 + m_leadProjectCount * 800.0 + m_techAllowance;
}

QString ChiefEngineer::getType() const
{
    return QStringLiteral("主任工程师");
}

QString ChiefEngineer::getDetail() const
{
    return QStringLiteral("技术方向：%1，负责项目：%2个")
        .arg(m_techDirection)
        .arg(m_leadProjectCount);
}

QJsonObject ChiefEngineer::toJson() const
{
    QJsonObject obj = Employee::toJson();
    obj["techDirection"] = m_techDirection;
    obj["leadProjectCount"] = m_leadProjectCount;
    obj["techAllowance"] = m_techAllowance;
    return obj;
}

Employee *ChiefEngineer::clone() const
{
    ChiefEngineer *copy = new ChiefEngineer(id(), name(), gender(), age(), hireDate(),
                                            m_techDirection, m_leadProjectCount, m_techAllowance);
    copyBaseFieldsTo(copy);
    return copy;
}
