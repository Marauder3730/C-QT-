#include "engineer.h"

#include <QJsonArray>

Engineer::Engineer()
{
}

Engineer::Engineer(const QString &id, const QString &name, const QString &gender,
                   int age, const QDate &hireDate, const QString &title,
                   const QStringList &projects, const QString &leaderId)
    : Employee(id, name, gender, age, hireDate)
    , m_title(title)
    , m_projects(projects)
    , m_leaderId(leaderId)
{
}

QString Engineer::title() const
{
    return m_title;
}

void Engineer::setTitle(const QString &title)
{
    m_title = title;
}

QStringList Engineer::projects() const
{
    return m_projects;
}

void Engineer::setProjects(const QStringList &projects)
{
    m_projects = projects;
}

int Engineer::projectCount() const
{
    return m_projects.size();
}

QString Engineer::leaderId() const
{
    return m_leaderId;
}

void Engineer::setLeaderId(const QString &leaderId)
{
    m_leaderId = leaderId;
}

double Engineer::calcSalary() const
{
    return 3000.0 + m_projects.size() * 500.0;
}

QString Engineer::getType() const
{
    return QStringLiteral("工程师");
}

QString Engineer::getDetail() const
{
    const QString projectText = m_projects.isEmpty()
                                    ? QStringLiteral("暂无")
                                    : m_projects.join(QStringLiteral("、"));
    return QStringLiteral("职称：%1 | 项目：%2")
        .arg(m_title, projectText);
}

QJsonObject Engineer::toJson() const
{
    QJsonObject obj = Employee::toJson();
    obj["title"] = m_title;
    obj["leaderId"] = m_leaderId;
    obj["projectCount"] = m_projects.size();

    QJsonArray projectArray;
    for (const QString &project : m_projects) {
        projectArray.append(project);
    }
    obj["projects"] = projectArray;
    return obj;
}

Employee *Engineer::clone() const
{
    Engineer *copy = new Engineer(id(), name(), gender(), age(), hireDate(),
                                  m_title, m_projects, m_leaderId);
    copyBaseFieldsTo(copy);
    return copy;
}
