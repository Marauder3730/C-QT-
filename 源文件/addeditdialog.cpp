#include "addeditdialog.h"
#include "ui_addeditdialog.h"
#include "employeemanager.h"

#include "chiefengineer.h"
#include "engineer.h"
#include "leader.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

AddEditDialog::AddEditDialog(Mode mode, const EmployeeManager *manager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddEditDialog)
    , m_mode(mode)
    , m_result(nullptr)
    , m_manager(manager)
{
    ui->setupUi(this);
    setFixedSize(440, 520);
    applyFormStyle();

    ui->labelPhotoPreview->setObjectName(QStringLiteral("labelPhotoPreview"));
    ui->labelPhotoPreview->setScaledContents(true);
    ui->scrollArea->setWidgetResizable(true);
    ui->listSubordinates->setAlternatingRowColors(true);

    QButtonGroup *genderGroup = new QButtonGroup(this);
    genderGroup->addButton(ui->radioMale);
    genderGroup->addButton(ui->radioFemale);

    ui->comboType->addItems({QStringLiteral("工程师"),
                             QStringLiteral("领导"),
                             QStringLiteral("主任工程师")});
    ui->comboLevel->addItems({QStringLiteral("主管"),
                              QStringLiteral("经理"),
                              QStringLiteral("总监")});

    ui->spinAge->setRange(16, 70);
    ui->spinLeadProjectCount->setRange(0, 100);
    ui->spinTechAllowance->setRange(0, 100000);
    ui->spinTechAllowance->setDecimals(2);
    ui->dateHire->setCalendarPopup(true);
    ui->dateHire->setDate(QDate::currentDate());
    ui->radioMale->setChecked(true);

    if (m_mode == AddMode) {
        setWindowTitle(QStringLiteral("新增员工"));
        ui->lineId->setEnabled(true);
    } else {
        setWindowTitle(QStringLiteral("修改员工"));
        ui->lineId->setEnabled(false);
    }

    connect(ui->comboType,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &AddEditDialog::onTypeChanged);
    connect(ui->lineProjects, &QLineEdit::textChanged, this, &AddEditDialog::onProjectsEdited);
    connect(ui->lineId, &QLineEdit::textChanged, this, &AddEditDialog::refreshLeaderSubordinates);
    connect(ui->btnCalcSalary, &QPushButton::clicked, this, &AddEditDialog::onCalcSalary);
    connect(ui->btnSelectPhoto, &QPushButton::clicked, this, &AddEditDialog::onSelectPhoto);
    connect(ui->btnClearPhoto, &QPushButton::clicked, this, &AddEditDialog::onClearPhoto);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddEditDialog::onAccept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [this]() { reject(); });

    refreshLeaderOptions();
    updatePhotoPreview();
    onTypeChanged(ui->comboType->currentIndex());
}

AddEditDialog::~AddEditDialog()
{
    delete ui;
}

void AddEditDialog::setEmployee(const Employee *employee)
{
    if (!employee) {
        return;
    }

    m_originalId = employee->id();
    m_photoData = employee->photoData();
    fillCommonFields(employee);
    fillTypeSpecificFields(employee);
    updatePhotoPreview();
    onTypeChanged(ui->comboType->currentIndex());
    onCalcSalary();
}

Employee *AddEditDialog::takeEmployee()
{
    Employee *emp = m_result;
    m_result = nullptr;
    return emp;
}

void AddEditDialog::onTypeChanged(int index)
{
    ui->stackedTypeFields->setCurrentIndex(index);
    ui->stackedRelationship->setCurrentIndex(index);

    if (index == 0) {
        refreshLeaderOptions();
        onProjectsEdited();
    } else if (index == 1) {
        refreshLeaderSubordinates();
    }
}

void AddEditDialog::onProjectsEdited()
{
    const int count = parseProjectsInput().size();
    ui->labelProjectCount->setText(QStringLiteral("%1 个（自动统计）").arg(count));
    onCalcSalary();
}

void AddEditDialog::refreshLeaderOptions(const QString &selectedLeaderId)
{
    ui->comboLeader->clear();
    ui->comboLeader->addItem(QStringLiteral("（无直属领导）"), QString());

    if (!m_manager) {
        return;
    }

    const QVector<Leader *> leaders = m_manager->getAllLeaders();
    int selectedIndex = 0;
    for (int i = 0; i < leaders.size(); ++i) {
        Leader *leader = leaders.at(i);
        if (m_mode == EditMode && leader->id() == ui->lineId->text().trimmed()) {
            continue;
        }
        ui->comboLeader->addItem(
            QStringLiteral("%1 (%2)").arg(leader->name(), leader->id()),
            leader->id());
        if (leader->id() == selectedLeaderId) {
            selectedIndex = ui->comboLeader->count() - 1;
        }
    }
    ui->comboLeader->setCurrentIndex(selectedIndex);
}

void AddEditDialog::refreshLeaderSubordinates()
{
    ui->listSubordinates->clear();
    if (!m_manager) {
        ui->labelSubordinateCount->setText(QStringLiteral("当前下属：0 人"));
        return;
    }

    const QString leaderId = ui->lineId->text().trimmed();
    const QVector<Engineer *> subordinates = m_manager->getSubordinates(leaderId);
    ui->labelSubordinateCount->setText(
        QStringLiteral("当前下属：%1 人").arg(subordinates.size()));

    if (subordinates.isEmpty()) {
        ui->listSubordinates->addItem(QStringLiteral("暂无下属。请让工程师在「直属领导」中选择本领导。"));
    }

    for (const Engineer *engineer : subordinates) {
        ui->listSubordinates->addItem(
            QStringLiteral("%1 (%2) - %3")
                .arg(engineer->name(), engineer->id(), engineer->projects().join(QStringLiteral("、"))));
    }
}

QStringList AddEditDialog::parseProjectsInput() const
{
    QStringList projects;
    const QStringList parts = ui->lineProjects->text().split(QRegExp("[,，;；]"), QString::SkipEmptyParts);
    for (QString part : parts) {
        part = part.trimmed();
        if (!part.isEmpty() && !projects.contains(part)) {
            projects.append(part);
        }
    }
    return projects;
}

void AddEditDialog::onCalcSalary()
{
    Employee *temp = buildEmployeeFromForm();
    if (!temp) {
        ui->lineSalary->clear();
        return;
    }

    ui->lineSalary->setText(QString::number(temp->calcSalary(), 'f', 2));
    delete temp;
}

void AddEditDialog::onAccept()
{
    if (!validateForm()) {
        return;
    }

    delete m_result;
    m_result = buildEmployeeFromForm();
    if (!m_result) {
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("无法创建员工对象。"));
        return;
    }

    accept();
}

void AddEditDialog::fillCommonFields(const Employee *employee)
{
    ui->lineId->setText(employee->id());
    ui->lineName->setText(employee->name());

    if (employee->gender() == QStringLiteral("女")) {
        ui->radioFemale->setChecked(true);
    } else {
        ui->radioMale->setChecked(true);
    }

    ui->spinAge->setValue(employee->age());
    ui->dateHire->setDate(employee->hireDate());

    if (employee->getType() == QStringLiteral("工程师")) {
        ui->comboType->setCurrentIndex(0);
    } else if (employee->getType() == QStringLiteral("领导")) {
        ui->comboType->setCurrentIndex(1);
    } else {
        ui->comboType->setCurrentIndex(2);
    }
}

void AddEditDialog::fillTypeSpecificFields(const Employee *employee)
{
    if (const Engineer *engineer = dynamic_cast<const Engineer *>(employee)) {
        ui->lineTitle->setText(engineer->title());
        ui->lineProjects->setText(engineer->projects().join(QStringLiteral("，")));
        refreshLeaderOptions(engineer->leaderId());
        onProjectsEdited();
    } else if (const Leader *leader = dynamic_cast<const Leader *>(employee)) {
        ui->lineDepartment->setText(leader->department());
        const int levelIndex = ui->comboLevel->findText(leader->level());
        if (levelIndex >= 0) {
            ui->comboLevel->setCurrentIndex(levelIndex);
        }
        refreshLeaderSubordinates();
    } else if (const ChiefEngineer *chief = dynamic_cast<const ChiefEngineer *>(employee)) {
        ui->lineTechDirection->setText(chief->techDirection());
        ui->spinLeadProjectCount->setValue(chief->leadProjectCount());
        ui->spinTechAllowance->setValue(chief->techAllowance());
    }
}

Employee *AddEditDialog::buildEmployeeFromForm() const
{
    const QString id = ui->lineId->text().trimmed();
    const QString name = ui->lineName->text().trimmed();
    const QString gender = ui->radioMale->isChecked() ? QStringLiteral("男") : QStringLiteral("女");
    const int age = ui->spinAge->value();
    const QDate hireDate = ui->dateHire->date();

    switch (ui->comboType->currentIndex()) {
    case 0: {
        Engineer *engineer = new Engineer(id, name, gender, age, hireDate,
                                          ui->lineTitle->text().trimmed(),
                                          parseProjectsInput(),
                                          ui->comboLeader->currentData().toString());
        engineer->setPhotoData(m_photoData);
        return engineer;
    }
    case 1: {
        const int subordinateCount = m_manager
                                         ? m_manager->getSubordinates(id).size()
                                         : 0;
        Leader *leader = new Leader(id, name, gender, age, hireDate,
                                    ui->lineDepartment->text().trimmed(),
                                    ui->comboLevel->currentText(),
                                    subordinateCount);
        leader->setPhotoData(m_photoData);
        return leader;
    }
    case 2: {
        ChiefEngineer *chief = new ChiefEngineer(id, name, gender, age, hireDate,
                                                 ui->lineTechDirection->text().trimmed(),
                                                 ui->spinLeadProjectCount->value(),
                                                 ui->spinTechAllowance->value());
        chief->setPhotoData(m_photoData);
        return chief;
    }
    default:
        return nullptr;
    }
}

void AddEditDialog::updatePhotoPreview()
{
    if (m_photoData.isEmpty()) {
        ui->labelPhotoPreview->clear();
        ui->labelPhotoPreview->setText(QStringLiteral("点击下方按钮上传证件照"));
        return;
    }

    QPixmap pixmap;
    pixmap.loadFromData(m_photoData);
    ui->labelPhotoPreview->setPixmap(
        pixmap.scaled(ui->labelPhotoPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void AddEditDialog::onSelectPhoto()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("选择证件照"),
        QString(),
        QStringLiteral("图片文件 (*.png *.jpg *.jpeg *.bmp)"));

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("无法读取图片文件。"));
        return;
    }

    m_photoData = file.readAll();
    updatePhotoPreview();
}

void AddEditDialog::onClearPhoto()
{
    m_photoData.clear();
    updatePhotoPreview();
}

bool AddEditDialog::validateForm() const
{
    if (ui->lineId->text().trimmed().isEmpty()) {
        QMessageBox::warning(const_cast<AddEditDialog *>(this), QStringLiteral("校验失败"),
                             QStringLiteral("工号不能为空。"));
        return false;
    }

    if (ui->lineName->text().trimmed().isEmpty()) {
        QMessageBox::warning(const_cast<AddEditDialog *>(this), QStringLiteral("校验失败"),
                             QStringLiteral("姓名不能为空。"));
        return false;
    }

    switch (ui->comboType->currentIndex()) {
    case 0:
        if (ui->lineTitle->text().trimmed().isEmpty()) {
            QMessageBox::warning(const_cast<AddEditDialog *>(this), QStringLiteral("校验失败"),
                                 QStringLiteral("请填写工程师职称。"));
            return false;
        }
        break;
    case 1:
        if (ui->lineDepartment->text().trimmed().isEmpty()) {
            QMessageBox::warning(const_cast<AddEditDialog *>(this), QStringLiteral("校验失败"),
                                 QStringLiteral("请填写所属部门。"));
            return false;
        }
        break;
    case 2:
        if (ui->lineTechDirection->text().trimmed().isEmpty()) {
            QMessageBox::warning(const_cast<AddEditDialog *>(this), QStringLiteral("校验失败"),
                                 QStringLiteral("请填写技术方向。"));
            return false;
        }
        break;
    default:
        break;
    }

    return true;
}

void AddEditDialog::applyFormStyle()
{
    setStyleSheet(QStringLiteral(
        "QDialog#AddEditDialog {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #1b2942, stop:1 #121b2d);"
        "}"
        "QDialog#AddEditDialog QScrollArea,"
        "QDialog#AddEditDialog QWidget#scrollAreaWidgetContents {"
        "  background: #e8ebf0;"
        "  border: none;"
        "}"
        "QDialog#AddEditDialog QGroupBox {"
        "  color: #111111;"
        "  background: #ffffff;"
        "  border: 1px solid #aeb6c2;"
        "  border-radius: 8px;"
        "  margin-top: 10px;"
        "  padding-top: 8px;"
        "  font-weight: 600;"
        "}"
        "QDialog#AddEditDialog QGroupBox::title {"
        "  color: #0d47a1;"
        "  subcontrol-origin: margin;"
        "  left: 8px;"
        "  padding: 0 4px;"
        "}"
        "QDialog#AddEditDialog QGroupBox#groupRelationship {"
        "  border: 2px solid #1565c0;"
        "}"
        "QDialog#AddEditDialog QGroupBox#groupRelationship::title {"
        "  color: #0d47a1;"
        "}"
        "QDialog#AddEditDialog QLabel {"
        "  color: #212121;"
        "}"
        "QDialog#AddEditDialog QLabel#labelRelationHintEngineer,"
        "QDialog#AddEditDialog QLabel#labelRelationHintLeader,"
        "QDialog#AddEditDialog QLabel#labelRelationHintChief {"
        "  color: #424242;"
        "  font-size: 12px;"
        "}"
        "QDialog#AddEditDialog QLabel#labelSubordinateCount {"
        "  color: #0d47a1;"
        "  font-weight: bold;"
        "}"
        "QDialog#AddEditDialog QLabel#labelPhotoPreview {"
        "  color: #616161;"
        "  background: #f5f5f5;"
        "  border: 1px dashed #9e9e9e;"
        "  border-radius: 6px;"
        "}"
        "QDialog#AddEditDialog QLineEdit,"
        "QDialog#AddEditDialog QSpinBox,"
        "QDialog#AddEditDialog QDoubleSpinBox,"
        "QDialog#AddEditDialog QComboBox,"
        "QDialog#AddEditDialog QDateEdit,"
        "QDialog#AddEditDialog QListWidget {"
        "  color: #111111;"
        "  background: #ffffff;"
        "  border: 1px solid #8a939f;"
        "  border-radius: 6px;"
        "  padding: 4px 8px;"
        "  selection-background-color: #bbdefb;"
        "  selection-color: #111111;"
        "}"
        "QDialog#AddEditDialog QComboBox QAbstractItemView {"
        "  color: #111111;"
        "  background: #ffffff;"
        "  selection-background-color: #1976d2;"
        "  selection-color: #ffffff;"
        "}"
        "QDialog#AddEditDialog QRadioButton {"
        "  color: #212121;"
        "}"
    ));
}
