#include "logindialog.h"
#include "ui_logindialog.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

namespace {
QString hashPassword(const QString &password)
{
    return QString::fromUtf8(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}
}

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , m_failedAttempts(0)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("系统登录"));
    setModal(true);
    setFixedSize(360, 260);
    ui->linePassword->setEchoMode(QLineEdit::Password);
    ui->labelTitle->setObjectName(QStringLiteral("labelLoginTitle"));

    loadCredentials();
    ui->lineUsername->setText(m_username);

    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, [this]() { reject(); });
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

bool LoginDialog::authenticate()
{
    return exec() == QDialog::Accepted;
}

void LoginDialog::loadCredentials()
{
    const QString configPath = QCoreApplication::applicationDirPath() + "/login.json";
    QFile file(configPath);
    if (!file.exists()) {
        m_username = QStringLiteral("WangYixian");
        m_passwordHash = hashPassword(QStringLiteral("123456"));
        saveCredentials(m_username, QStringLiteral("123456"));
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        m_username = QStringLiteral("WangYixian");
        m_passwordHash = hashPassword(QStringLiteral("123456"));
        return;
    }

    const QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    m_username = obj.value("username").toString(QStringLiteral("WangYixian"));
    m_passwordHash = obj.value("passwordHash").toString(hashPassword(QStringLiteral("123456")));

    if (m_username == QStringLiteral("admin")) {
        m_username = QStringLiteral("WangYixian");
        QJsonObject updated;
        updated["username"] = m_username;
        updated["passwordHash"] = m_passwordHash;
        QFile out(configPath);
        if (out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            out.write(QJsonDocument(updated).toJson(QJsonDocument::Indented));
        }
    }
}

void LoginDialog::saveCredentials(const QString &username, const QString &password) const
{
    QJsonObject obj;
    obj["username"] = username;
    obj["passwordHash"] = hashPassword(password);

    const QString configPath = QCoreApplication::applicationDirPath() + "/login.json";
    QFile file(configPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    }
}

bool LoginDialog::verifyCredentials(const QString &username, const QString &password) const
{
    return username == m_username && hashPassword(password) == m_passwordHash;
}

void LoginDialog::onLoginClicked()
{
    const QString username = ui->lineUsername->text().trimmed();
    const QString password = ui->linePassword->text();

    if (verifyCredentials(username, password)) {
        accept();
        return;
    }

    ++m_failedAttempts;
    if (m_failedAttempts >= 3) {
        QMessageBox::critical(this, QStringLiteral("登录失败"),
                              QStringLiteral("连续 3 次密码错误，程序即将退出。"));
        reject();
        return;
    }

    QMessageBox::warning(this, QStringLiteral("登录失败"),
                         QStringLiteral("用户名或密码错误，还可尝试 %1 次。")
                             .arg(3 - m_failedAttempts));
    ui->linePassword->clear();
    ui->linePassword->setFocus();
}
