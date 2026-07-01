#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "employeemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAddEmployee();
    void onEditEmployee();
    void onDeleteEmployee();
    void onSearch();
    void onShowAll();
    void onRefresh();
    void onStatistics();
    void onImportData();
    void onExportData();
    void onAbout();
    void onTableSelectionChanged();
    void onTableDoubleClicked(int row, int column);
    void clearStatusMessage();

private:
    void setupUiExtras();
    void setupConnections();
    void refreshTable();
    void refreshTable(const QVector<Employee *> &employees);
    Employee *selectedEmployee() const;
    void showStatusMessage(const QString &message, int timeoutMs = 3000);
    void autoSave();
    bool confirmDelete(const QString &name, const QString &id);
    void updatePhotoPreviewPanel(Employee *employee);
    QWidget *createPhotoCellWidget(Employee *employee) const;

    Ui::MainWindow *ui;
    EmployeeManager m_manager;
    QString m_dataFilePath;
    QTimer m_statusTimer;
};

#endif // MAINWINDOW_H
