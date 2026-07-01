#include "mainwindow.h"
#include "logindialog.h"
#include "uistyle.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UiStyle::applyApplicationStyle(&a);

    LoginDialog login;
    if (!login.authenticate()) {
        return 0;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
