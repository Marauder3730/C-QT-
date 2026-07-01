#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    bool authenticate();

private slots:
    void onLoginClicked();

private:
    bool verifyCredentials(const QString &username, const QString &password) const;
    void saveCredentials(const QString &username, const QString &password) const;
    void loadCredentials();

    Ui::LoginDialog *ui;
    int m_failedAttempts;
    QString m_username;
    QString m_passwordHash;
};

#endif // LOGINDIALOG_H
