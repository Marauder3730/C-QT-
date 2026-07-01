#include "statdialog.h"

#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>

#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>

StatDialog::StatDialog(const EmployeeManager &manager, QWidget *parent)
    : QDialog(parent)
    , m_manager(manager)
{
    setWindowTitle(QStringLiteral("统计分析"));
    resize(720, 520);

    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(createPersonnelTab(), QStringLiteral("人员统计"));
    tabs->addTab(createSalaryTab(), QStringLiteral("薪资统计"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tabs);
}

QWidget *StatDialog::createPersonnelTab()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);

    layout->addWidget(createPieChart(), 3);

    const QMap<QString, int> typeCount = m_manager.getTypeCount();
    QString text = QStringLiteral("总人数：%1\n平均年龄：%2 岁\n\n各岗位人数：\n")
                         .arg(m_manager.getTotalCount())
                         .arg(QString::number(m_manager.getAverageAge(), 'f', 1));

    for (auto it = typeCount.constBegin(); it != typeCount.constEnd(); ++it) {
        text += QStringLiteral("  %1：%2 人\n").arg(it.key()).arg(it.value());
    }

    QLabel *label = new QLabel(text, page);
    label->setWordWrap(true);
    layout->addWidget(label, 1);

    return page;
}

QWidget *StatDialog::createSalaryTab()
{
    QWidget *page = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(page);

    layout->addWidget(createBarChart(), 3);

    const QString text = QStringLiteral("全公司平均薪资：%1 元\n最高薪资：%2 元\n最低薪资：%3 元")
                             .arg(QString::number(m_manager.getAverageSalary(), 'f', 2))
                             .arg(QString::number(m_manager.getMaxSalary(), 'f', 2))
                             .arg(QString::number(m_manager.getMinSalary(), 'f', 2));

    QLabel *label = new QLabel(text, page);
    label->setWordWrap(true);
    layout->addWidget(label, 1);

    return page;
}

QWidget *StatDialog::createPieChart() const
{
    QtCharts::QPieSeries *series = new QtCharts::QPieSeries();
    const QMap<QString, int> typeCount = m_manager.getTypeCount();

    if (typeCount.isEmpty()) {
        series->append(QStringLiteral("暂无数据"), 1);
    } else {
        for (auto it = typeCount.constBegin(); it != typeCount.constEnd(); ++it) {
            series->append(it.key(), it.value());
        }
    }

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle(QStringLiteral("岗位人数分布"));
    chart->legend()->setAlignment(Qt::AlignBottom);

    QtCharts::QChartView *view = new QtCharts::QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    return view;
}

QWidget *StatDialog::createBarChart() const
{
    QtCharts::QBarSet *set = new QtCharts::QBarSet(QStringLiteral("平均月薪"));
    QStringList categories;
    const QMap<QString, double> averages = m_manager.getAverageSalaryByType();

    if (averages.isEmpty()) {
        categories << QStringLiteral("暂无数据");
        *set << 0;
    } else {
        for (auto it = averages.constBegin(); it != averages.constEnd(); ++it) {
            categories << it.key();
            *set << it.value();
        }
    }

    QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
    series->append(set);

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle(QStringLiteral("各岗位平均薪资对比"));
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    QtCharts::QBarCategoryAxis *axisX = new QtCharts::QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    axisY->setTitleText(QStringLiteral("薪资(元)"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QtCharts::QChartView *view = new QtCharts::QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    return view;
}
