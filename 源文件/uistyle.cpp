#include "uistyle.h"

#include <QApplication>

QString UiStyle::mainStylesheet()
{
    return QStringLiteral(
        "QMainWindow {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 #0b1220, stop:0.45 #152238, stop:1 #1a2a44);"
        "}"
        "QDialog {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "    stop:0 #1b2942, stop:1 #121b2d);"
        "  color: #e8eef9;"
        "}"
        "QWidget#centralwidget { background: transparent; }"
        "QWidget#leftPanel {"
        "  background: rgba(18, 28, 48, 0.72);"
        "  border: 1px solid rgba(120, 160, 220, 0.25);"
        "  border-radius: 12px;"
        "}"
        "QGroupBox {"
        "  color: #dce8ff;"
        "  font-weight: 600;"
        "  border: 1px solid rgba(120, 160, 220, 0.35);"
        "  border-radius: 10px;"
        "  margin-top: 12px;"
        "  padding-top: 10px;"
        "  background: rgba(255, 255, 255, 0.04);"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 10px;"
        "  padding: 0 6px;"
        "  color: #9ec5ff;"
        "}"
        "QLabel { color: #dbe7ff; }"
        "QLabel#labelLoginTitle {"
        "  color: #ffffff;"
        "  font-size: 22px;"
        "  font-weight: 700;"
        "}"
        "QLabel#labelPhotoPreview {"
        "  background: rgba(255,255,255,0.06);"
        "  border: 1px dashed rgba(158, 197, 255, 0.45);"
        "  border-radius: 8px;"
        "  color: #8faee0;"
        "}"
        "QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QDateEdit, QListWidget {"
        "  background: rgba(255,255,255,0.08);"
        "  color: #eef4ff;"
        "  border: 1px solid rgba(130, 170, 230, 0.35);"
        "  border-radius: 6px;"
        "  padding: 5px 8px;"
        "  selection-background-color: #3d7bd9;"
        "}"
        "QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus, QDateEdit:focus {"
        "  border: 1px solid #6aa8ff;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background: #1a2740;"
        "  color: #eef4ff;"
        "  selection-background-color: #3d7bd9;"
        "}"
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #3b82f6, stop:1 #6366f1);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 7px 14px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #4f93ff, stop:1 #7577ff);"
        "}"
        "QPushButton:pressed {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #2f6fd6, stop:1 #4f52d6);"
        "}"
        "QPushButton#btnDelete {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #ef4444, stop:1 #dc2626);"
        "}"
        "QPushButton#btnStatistics {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #14b8a6, stop:1 #0891b2);"
        "}"
        "QRadioButton { color: #dbe7ff; }"
        "QTableWidget {"
        "  background: rgba(255,255,255,0.05);"
        "  alternate-background-color: rgba(255,255,255,0.025);"
        "  color: #edf2ff;"
        "  gridline-color: rgba(130, 170, 230, 0.18);"
        "  border: 1px solid rgba(120, 160, 220, 0.28);"
        "  border-radius: 10px;"
        "  selection-background-color: rgba(59, 130, 246, 0.45);"
        "}"
        "QHeaderView::section {"
        "  background: rgba(30, 45, 75, 0.95);"
        "  color: #bcd3ff;"
        "  padding: 7px;"
        "  border: none;"
        "  border-bottom: 2px solid #3b82f6;"
        "  font-weight: 600;"
        "}"
        "QMenuBar {"
        "  background: rgba(15, 22, 38, 0.85);"
        "  color: #dbe7ff;"
        "  border-bottom: 1px solid rgba(120, 160, 220, 0.2);"
        "}"
        "QMenuBar::item:selected { background: rgba(59, 130, 246, 0.35); }"
        "QMenu { background: #1a2740; color: #eef4ff; border: 1px solid #3d5f92; }"
        "QMenu::item:selected { background: #3d7bd9; }"
        "QToolBar { background: rgba(15, 22, 38, 0.75); border: none; }"
        "QStatusBar {"
        "  background: rgba(15, 22, 38, 0.9);"
        "  color: #9ec5ff;"
        "  border-top: 1px solid rgba(120, 160, 220, 0.2);"
        "}"
        "QScrollArea { border: none; background: transparent; }"
        "QTabWidget::pane { border: 1px solid rgba(120, 160, 220, 0.3); background: rgba(255,255,255,0.03); }"
        "QTabBar::tab {"
        "  background: rgba(255,255,255,0.06);"
        "  color: #c9dcff;"
        "  padding: 7px 14px;"
        "  border-top-left-radius: 6px;"
        "  border-top-right-radius: 6px;"
        "}"
        "QTabBar::tab:selected { background: rgba(59, 130, 246, 0.45); color: white; }"
    );
}

void UiStyle::applyApplicationStyle(QApplication *app)
{
    if (app) {
        app->setStyleSheet(mainStylesheet());
    }
}

void UiStyle::fadeIn(QWidget *widget, int durationMs)
{
    Q_UNUSED(widget)
    Q_UNUSED(durationMs)
}

void UiStyle::slideUp(QWidget *widget, int durationMs)
{
    Q_UNUSED(widget)
    Q_UNUSED(durationMs)
}

void UiStyle::applyCardShadow(QWidget *widget, int blurRadius, int yOffset)
{
    Q_UNUSED(widget)
    Q_UNUSED(blurRadius)
    Q_UNUSED(yOffset)
}

void UiStyle::installButtonFeedback(QPushButton *button)
{
    Q_UNUSED(button)
}

void UiStyle::installButtonFeedback(QWidget *root)
{
    Q_UNUSED(root)
}

void UiStyle::installHoverPressFeedback(QWidget *root)
{
    Q_UNUSED(root)
}
