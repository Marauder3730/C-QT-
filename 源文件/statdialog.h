#ifndef STATDIALOG_H
#define STATDIALOG_H

#include <QDialog>

#include "employeemanager.h"

class StatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatDialog(const EmployeeManager &manager, QWidget *parent = nullptr);

private:
    QWidget *createPersonnelTab();
    QWidget *createSalaryTab();
    QWidget *createPieChart() const;
    QWidget *createBarChart() const;

    const EmployeeManager &m_manager;
};

#endif // STATDIALOG_H
