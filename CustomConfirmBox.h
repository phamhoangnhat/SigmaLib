#ifndef CUSTOMCONFIRMBOX_H
#define CUSTOMCONFIRMBOX_H

#include <QWidget>
#include <QEventLoop>

class QLabel;
class QPushButton;

class CustomConfirmBox : public QWidget
{
    Q_OBJECT

public:
    int confirm = -1;
    explicit CustomConfirmBox(const QString& title, const QString& message, QWidget* reference = nullptr);
    void showWindow();
    void closeWindow();

protected:
    void paintEvent(QPaintEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void fadeIn();
    void fadeOutAndClose();
    void fadeOutAndHide();
    void onYesButton();
    void onNoButton();

    QLabel* label;
    QPushButton* yesButton;
    QPushButton* noButton;
    QWidget* referenceWidget = nullptr;

    bool m_forceClose = false;
    QEventLoop eventLoop;
};

#endif // CUSTOMCONFIRMBOX_H
