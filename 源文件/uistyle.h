#ifndef UISTYLE_H
#define UISTYLE_H

#include <QWidget>

class QApplication;
class QPushButton;

class UiStyle
{
public:
    static void applyApplicationStyle(QApplication *app);
    static void fadeIn(QWidget *widget, int durationMs = 450);
    static void slideUp(QWidget *widget, int durationMs = 420);
    static void applyCardShadow(QWidget *widget, int blurRadius = 28, int yOffset = 8);
    static void installButtonFeedback(QPushButton *button);
    static void installButtonFeedback(QWidget *root);
    static void installHoverPressFeedback(QWidget *root);
    static QString mainStylesheet();
};

#endif // UISTYLE_H
