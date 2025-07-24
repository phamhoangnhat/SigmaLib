#ifndef MESSAGEAPIKEYBOX_H
#define MESSAGEAPIKEYBOX_H

#include <QWidget>

class QLabel;
class QPushButton;

class MessageApiKeyBox : public QWidget
{
    Q_OBJECT

public:
    explicit MessageApiKeyBox(QWidget* reference = nullptr);
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
    void onAddKeyButton();

    QLabel* label;
    QPushButton* closeButton;
    QPushButton* addKeyButton;
    QWidget* referenceWidget = nullptr;

    bool m_forceClose = false;
};

#endif // MESSAGEAPIKEYBOX_H
