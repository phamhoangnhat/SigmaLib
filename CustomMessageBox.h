#ifndef CUSTOMMESSAGEBOX_H
#define CUSTOMMESSAGEBOX_H

#include <QWidget>

class QLabel;
class QPushButton;

class CustomMessageBox : public QWidget
{
    Q_OBJECT

public:
    explicit CustomMessageBox(const QString& title, QWidget* reference = nullptr);
    void setMessage(const QString& message);
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
    void onCloseButton();

    QLabel* label;
    QPushButton* closeButton;
    QWidget* referenceWidget = nullptr;

    bool m_forceClose = false;
};

#endif // CUSTOMMESSAGEBOX_H
