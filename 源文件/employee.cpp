#include "employee.h"
#include "engineer.h"
#include "leader.h"
#include "chiefengineer.h"

#include <QJsonArray>
#include <QPixmap>

Employee::Employee()
    : m_age(0)
{
}

Employee::Employee(const QString &id, const QString &name, const QString &gender,
                   int age, const QDate &hireDate)
    : m_id(id)
    , m_name(name)
    , m_gender(gender)
    , m_age(age)
    , m_hireDate(hireDate)
{
}

Employee::~Employee()
{
}

QString Employee::id() const
{
    return m_id;
}

void Employee::setId(const QString &id)
{
    m_id = id;
}

QString Employee::name() const
{
    return m_name;
}

void Employee::setName(const QString &name)
{
    m_name = name;
}

QString Employee::gender() const
{
    return m_gender;
}

void Employee::setGender(const QString &gender)
{
    m_gender = gender;
}

int Employee::age() const
{
    return m_age;
}

void Employee::setAge(int age)
{
    m_age = age;
}

QDate Employee::hireDate() const
{
    return m_hireDate;
}

void Employee::setHireDate(const QDate &hireDate)
{
    m_hireDate = hireDate;
}

QByteArray Employee::photoData() const
{
    return m_photoData;
}

void Employee::setPhotoData(const QByteArray &photoData)
{
    m_photoData = photoData;
}

bool Employee::hasPhoto() const
{
    return !m_photoData.isEmpty();
}

QPixmap Employee::photoPixmap(const QSize &size) const
{
    if (m_photoData.isEmpty()) {
        QPixmap placeholder(size.isValid() ? size : QSize(120, 150));
        placeholder.fill(Qt::transparent);
        return placeholder;
    }

    QPixmap pixmap;
    pixmap.loadFromData(m_photoData);
    if (pixmap.isNull()) {
        return QPixmap();
    }

    if (size.isValid()) {
        return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    return pixmap;
}

void Employee::copyBaseFieldsTo(Employee *target) const
{
    if (!target) {
        return;
    }

    target->setId(m_id);
    target->setName(m_name);
    target->setGender(m_gender);
    target->setAge(m_age);
    target->setHireDate(m_hireDate);
    target->setPhotoData(m_photoData);
}

QJsonObject Employee::baseToJson() const
{
    QJsonObject obj;
    obj["id"] = m_id;
    obj["name"] = m_name;
    obj["gender"] = m_gender;
    obj["age"] = m_age;
    obj["hireDate"] = m_hireDate.toString(Qt::ISODate);
    if (!m_photoData.isEmpty()) {
        obj["photo"] = QString::fromLatin1(m_photoData.toBase64());
    }
    return obj;
}

QJsonObject Employee::toJson() const
{
    QJsonObject obj = baseToJson();
    obj["type"] = getType();
    return obj;
}

Employee *Employee::fromJson(const QJsonObject &obj)
{
    const QString type = obj.value("type").toString();
    Employee *emp = nullptr;

    if (type == QStringLiteral("工程师")) {
        emp = new Engineer();
    } else if (type == QStringLiteral("领导")) {
        emp = new Leader();
    } else if (type == QStringLiteral("主任工程师")) {
        emp = new ChiefEngineer();
    } else {
        return nullptr;
    }

    emp->setId(obj.value("id").toString());
    emp->setName(obj.value("name").toString());
    emp->setGender(obj.value("gender").toString());
    emp->setAge(obj.value("age").toInt());
    emp->setHireDate(QDate::fromString(obj.value("hireDate").toString(), Qt::ISODate));
    emp->setPhotoData(QByteArray::fromBase64(obj.value("photo").toString().toLatin1()));

    if (Engineer *engineer = dynamic_cast<Engineer *>(emp)) {
        engineer->setTitle(obj.value("title").toString());
        engineer->setLeaderId(obj.value("leaderId").toString());

        QStringList projects;
        const QJsonArray projectArray = obj.value("projects").toArray();
        if (!projectArray.isEmpty()) {
            for (const QJsonValue &value : projectArray) {
                const QString project = value.toString().trimmed();
                if (!project.isEmpty()) {
                    projects.append(project);
                }
            }
        } else {
            const int legacyCount = obj.value("projectCount").toInt(0);
            for (int i = 1; i <= legacyCount; ++i) {
                projects.append(QStringLiteral("项目%1").arg(i));
            }
        }
        engineer->setProjects(projects);
    } else if (Leader *leader = dynamic_cast<Leader *>(emp)) {
        leader->setDepartment(obj.value("department").toString());
        leader->setLevel(obj.value("level").toString());
        leader->setSubordinateCount(obj.value("subordinateCount").toInt());
    } else if (ChiefEngineer *chief = dynamic_cast<ChiefEngineer *>(emp)) {
        chief->setTechDirection(obj.value("techDirection").toString());
        chief->setLeadProjectCount(obj.value("leadProjectCount").toInt());
        chief->setTechAllowance(obj.value("techAllowance").toDouble());
    }

    return emp;
}
