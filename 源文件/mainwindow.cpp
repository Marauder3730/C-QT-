#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "addeditdialog.h"
#include "statdialog.h"
#include "uistyle.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dataFilePath(QCoreApplication::applicationDirPath() + "/employee.json")
{
    ui->setupUi(this);
    setupUiExtras();
    setupConnections();

    ui->btnDelete->setObjectName(QStringLiteral("btnDelete"));
    ui->btnStatistics->setObjectName(QStringLiteral("btnStatistics"));
    ui->labelPhotoPreview->setObjectName(QStringLiteral("labelPhotoPreview"));
    ui->labelPhotoPreview->setScaledContents(true);

    m_manager.loadFromFile(m_dataFilePath);
    m_manager.syncRelationships();
    refreshTable();
    showStatusMessage(QStringLiteral("系统启动成功，数据已加载"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiExtras()
{
    setWindowTitle(QStringLiteral("公司员工管理系统"));
    resize(1100, 680);

    ui->tableEmployees->setColumnCount(9);
    ui->tableEmployees->setHorizontalHeaderLabels(
        {QStringLiteral("照片"), QStringLiteral("工号"), QStringLiteral("姓名"), QStringLiteral("性别"),
         QStringLiteral("年龄"), QStringLiteral("岗位类型"), QStringLiteral("岗位详情"),
         QStringLiteral("入职日期"), QStringLiteral("月薪(元)")});
    ui->tableEmployees->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableEmployees->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableEmployees->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableEmployees->horizontalHeader()->setStretchLastSection(true);
    ui->tableEmployees->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableEmployees->setColumnWidth(0, 72);
    for (int col = 1; col < 9; ++col) {
        ui->tableEmployees->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    }
    ui->tableEmployees->setSortingEnabled(true);
    ui->tableEmployees->setAlternatingRowColors(true);
    ui->tableEmployees->verticalHeader()->setDefaultSectionSize(58);
    ui->tableEmployees->verticalHeader()->setVisible(false);

    statusBar()->showMessage(QStringLiteral("就绪"));
}

void MainWindow::setupConnections()
{
    connect(ui->btnSearch, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(ui->btnShowAll, &QPushButton::clicked, this, &MainWindow::onShowAll);
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onAddEmployee);
    connect(ui->btnEdit, &QPushButton::clicked, this, &MainWindow::onEditEmployee);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteEmployee);
    connect(ui->btnStatistics, &QPushButton::clicked, this, &MainWindow::onStatistics);

    connect(ui->actionAdd, &QAction::triggered, this, [this]() { onAddEmployee(); });
    connect(ui->actionEdit, &QAction::triggered, this, [this]() { onEditEmployee(); });
    connect(ui->actionDelete, &QAction::triggered, this, [this]() { onDeleteEmployee(); });
    connect(ui->actionRefresh, &QAction::triggered, this, [this]() { onRefresh(); });
    connect(ui->actionImport, &QAction::triggered, this, [this]() { onImportData(); });
    connect(ui->actionExport, &QAction::triggered, this, [this]() { onExportData(); });
    connect(ui->actionExit, &QAction::triggered, this, [this]() { close(); });
    connect(ui->actionStatistics, &QAction::triggered, this, [this]() { onStatistics(); });
    connect(ui->actionAbout, &QAction::triggered, this, [this]() { onAbout(); });

    connect(ui->tableEmployees, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onTableSelectionChanged);
    connect(ui->tableEmployees, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::onTableDoubleClicked);

    connect(&m_statusTimer, &QTimer::timeout, this, &MainWindow::clearStatusMessage);
}

void MainWindow::refreshTable()
{
    refreshTable(m_manager.allEmployees());
}

void MainWindow::refreshTable(const QVector<Employee *> &employees)
{
    const QVector<Employee *> list = employees;

    ui->tableEmployees->setSortingEnabled(false);
    ui->tableEmployees->setRowCount(list.size());

    for (int row = 0; row < list.size(); ++row) {
        Employee *emp = list.at(row);
        ui->tableEmployees->setCellWidget(row, 0, createPhotoCellWidget(emp));
        ui->tableEmployees->setItem(row, 1, new QTableWidgetItem(emp->id()));
        ui->tableEmployees->setItem(row, 2, new QTableWidgetItem(emp->name()));
        ui->tableEmployees->setItem(row, 3, new QTableWidgetItem(emp->gender()));
        ui->tableEmployees->setItem(row, 4, new QTableWidgetItem(QString::number(emp->age())));
        ui->tableEmployees->setItem(row, 5, new QTableWidgetItem(emp->getType()));
        ui->tableEmployees->setItem(row, 6, new QTableWidgetItem(emp->getDetail()));
        ui->tableEmployees->setItem(row, 7, new QTableWidgetItem(emp->hireDate().toString("yyyy-MM-dd")));
        ui->tableEmployees->setItem(row, 8, new QTableWidgetItem(QString::number(emp->calcSalary(), 'f', 2)));
    }

    ui->tableEmployees->setSortingEnabled(true);
    statusBar()->showMessage(QStringLiteral("共 %1 名员工").arg(m_manager.getTotalCount()));
    onTableSelectionChanged();
}

Employee *MainWindow::selectedEmployee() const
{
    const int row = ui->tableEmployees->currentRow();
    if (row < 0) {
        return nullptr;
    }

    const QString id = ui->tableEmployees->item(row, 1)->text();
    return m_manager.findEmployeeById(id);
}

void MainWindow::showStatusMessage(const QString &message, int timeoutMs)
{
    statusBar()->showMessage(message);
    m_statusTimer.start(timeoutMs);
}

void MainWindow::clearStatusMessage()
{
    m_statusTimer.stop();
    statusBar()->showMessage(QStringLiteral("共 %1 名员工").arg(m_manager.getTotalCount()));
}

void MainWindow::autoSave()
{
    m_manager.syncRelationships();
    if (!m_manager.saveToFile(m_dataFilePath)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"),
                             QStringLiteral("无法写入数据文件：%1").arg(m_dataFilePath));
    }
}

bool MainWindow::confirmDelete(const QString &name, const QString &id)
{
    return QMessageBox::question(
               this,
               QStringLiteral("确认删除"),
               QStringLiteral("确定要删除员工【%1 / %2】吗？此操作不可恢复。").arg(name, id),
               QMessageBox::Yes | QMessageBox::No,
               QMessageBox::No) == QMessageBox::Yes;
}

void MainWindow::onAddEmployee()
{
    AddEditDialog dialog(AddEditDialog::AddMode, &m_manager, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Employee *emp = dialog.takeEmployee();
    if (!emp) {
        return;
    }

    if (!m_manager.addEmployee(emp)) {
        delete emp;
        QMessageBox::warning(this, QStringLiteral("新增失败"),
                             QStringLiteral("工号已存在或数据无效，请检查后重试。"));
        showStatusMessage(QStringLiteral("新增失败：工号重复"));
        return;
    }

    refreshTable();
    autoSave();
    showStatusMessage(QStringLiteral("新增员工成功"));
}

void MainWindow::onEditEmployee()
{
    Employee *current = selectedEmployee();
    if (!current) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择要修改的员工。"));
        return;
    }

    AddEditDialog dialog(AddEditDialog::EditMode, &m_manager, this);
    dialog.setEmployee(current);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Employee *updated = dialog.takeEmployee();
    if (!updated) {
        return;
    }

    if (!m_manager.updateEmployee(updated)) {
        delete updated;
        QMessageBox::warning(this, QStringLiteral("修改失败"), QStringLiteral("未能更新员工信息。"));
        showStatusMessage(QStringLiteral("修改失败"));
        return;
    }

    refreshTable();
    autoSave();
    showStatusMessage(QStringLiteral("修改员工成功"));
}

void MainWindow::onDeleteEmployee()
{
    Employee *current = selectedEmployee();
    if (!current) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择要删除的员工。"));
        return;
    }

    if (!confirmDelete(current->name(), current->id())) {
        return;
    }

    if (!m_manager.deleteEmployee(current->id())) {
        QMessageBox::warning(this, QStringLiteral("删除失败"), QStringLiteral("未找到该员工。"));
        showStatusMessage(QStringLiteral("删除失败：工号不存在"));
        return;
    }

    refreshTable();
    autoSave();
    showStatusMessage(QStringLiteral("删除员工成功"));
}

void MainWindow::onSearch()
{
    const QString keyword = ui->lineSearch->text().trimmed();
    refreshTable(m_manager.searchEmployee(keyword));
    showStatusMessage(QStringLiteral("查询完成，共 %1 条结果").arg(ui->tableEmployees->rowCount()));
}

void MainWindow::onShowAll()
{
    ui->lineSearch->clear();
    refreshTable();
    showStatusMessage(QStringLiteral("已显示全部员工"));
}

void MainWindow::onRefresh()
{
    refreshTable();
    showStatusMessage(QStringLiteral("列表已刷新"));
}

void MainWindow::onStatistics()
{
    StatDialog dialog(m_manager, this);
    dialog.exec();
}

void MainWindow::onImportData()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this, QStringLiteral("导入数据"), QCoreApplication::applicationDirPath(),
        QStringLiteral("JSON 文件 (*.json)"));

    if (filePath.isEmpty()) {
        return;
    }

    if (!m_manager.loadFromFile(filePath)) {
        QMessageBox::warning(this, QStringLiteral("导入失败"), QStringLiteral("文件格式错误或无法读取。"));
        return;
    }

    m_manager.syncRelationships();
    refreshTable();
    showStatusMessage(QStringLiteral("导入成功"));
}

void MainWindow::onExportData()
{
    const QString filePath = QFileDialog::getSaveFileName(
        this, QStringLiteral("导出数据"), QCoreApplication::applicationDirPath() + "/employee_export.json",
        QStringLiteral("JSON 文件 (*.json)"));

    if (filePath.isEmpty()) {
        return;
    }

    if (!m_manager.saveToFile(filePath)) {
        QMessageBox::warning(this, QStringLiteral("导出失败"), QStringLiteral("无法写入文件。"));
        return;
    }

    showStatusMessage(QStringLiteral("导出成功"));
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, QStringLiteral("关于系统"),
                       QStringLiteral("公司员工管理系统\n\n"
                                      "面向对象程序设计课程设计\n"
                                      "支持工程师、领导、主任工程师三类员工管理\n"
                                      "Qt 5.12.9 + C++"));
}

void MainWindow::onTableSelectionChanged()
{
    Employee *current = selectedEmployee();
    updatePhotoPreviewPanel(current);
    if (current) {
        statusBar()->showMessage(
            QStringLiteral("共 %1 名员工 | 当前选中：%2 (%3)")
                .arg(m_manager.getTotalCount())
                .arg(current->name())
                .arg(current->id()));
    } else {
        statusBar()->showMessage(QStringLiteral("共 %1 名员工").arg(m_manager.getTotalCount()));
    }
}

QWidget *MainWindow::createPhotoCellWidget(Employee *employee) const
{
    QLabel *label = new QLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setFixedSize(56, 48);

    if (employee && employee->hasPhoto()) {
        label->setPixmap(employee->photoPixmap(QSize(48, 48)));
        label->setScaledContents(true);
    } else {
        label->setText(QStringLiteral("无"));
        label->setStyleSheet(QStringLiteral("color:#8faee0;font-size:11px;"));
    }

    return label;
}

void MainWindow::updatePhotoPreviewPanel(Employee *employee)
{
    if (!employee) {
        ui->labelPhotoPreview->clear();
        ui->labelPhotoPreview->setText(QStringLiteral("选中员工后显示证件照"));
        ui->labelEmployeeInfo->setText(QStringLiteral("暂无选中"));
        ui->labelRelationship->setText(QStringLiteral("组织关系将在此显示"));
        return;
    }

    if (employee->hasPhoto()) {
        ui->labelPhotoPreview->setPixmap(
            employee->photoPixmap(QSize(ui->labelPhotoPreview->width(), ui->labelPhotoPreview->height())));
    } else {
        ui->labelPhotoPreview->clear();
        ui->labelPhotoPreview->setText(QStringLiteral("该员工暂无证件照"));
    }

    ui->labelEmployeeInfo->setText(
        QStringLiteral("%1\n%2 | %3\n月薪 %4 元")
            .arg(employee->name())
            .arg(employee->id())
            .arg(employee->getType())
            .arg(QString::number(employee->calcSalary(), 'f', 2)));

    const QString relationText = m_manager.relationshipSummary(employee);
    ui->labelRelationship->setText(relationText.isEmpty()
                                       ? QStringLiteral("暂无组织关系信息")
                                       : relationText);
}

void MainWindow::onTableDoubleClicked(int row, int column)
{
    Q_UNUSED(row)
    Q_UNUSED(column)
    onEditEmployee();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    autoSave();
    event->accept();
}
