#ifndef ADDEDITDIALOG_H
#define ADDEDITDIALOG_H

#include <QByteArray>
#include <QDialog>

#include "employee.h"

class EmployeeManager;

namespace Ui {
class AddEditDialog;
}

class AddEditDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        AddMode,
        EditMode
    };

    explicit AddEditDialog(Mode mode, const EmployeeManager *manager, QWidget *parent = nullptr);
    ~AddEditDialog();

    void setEmployee(const Employee *employee);
    Employee *takeEmployee();

private slots:
    void onTypeChanged(int index);
    void onCalcSalary();
    void onAccept();
    void onProjectsEdited();

private:
    Employee *buildEmployeeFromForm() const;
    bool validateForm() const;
    void fillCommonFields(const Employee *employee);
    void fillTypeSpecificFields(const Employee *employee);
    void updatePhotoPreview();
    void onSelectPhoto();
    void onClearPhoto();
    void refreshLeaderOptions(const QString &selectedLeaderId = QString());
    void refreshLeaderSubordinates();
    QStringList parseProjectsInput() const;
    void applyFormStyle();

    Ui::AddEditDialog *ui;
    Mode m_mode;
    QString m_originalId;
    Employee *m_result;
    QByteArray m_photoData;
    const EmployeeManager *m_manager;
};

#endif // ADDEDITDIALOG_H
