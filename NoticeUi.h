#ifndef NOTICEUI_H
#define NOTICEUI_H

#include "Variable.h"
#include <QWidget>
#include <QTimer>
#include <QString>
#include <QColor>
#include <QCloseEvent>

enum class FadeState {
    FadingIn,
    FadingOut,
    Idle
};

class NoticeUi : public QWidget {
    Q_OBJECT

public:

    QColor backgroundColor;
    QString displayText;
    int currentFontSize;
    int currentOpacity;
    int targetOpacity;
    int iconWidth;

    static NoticeUi& getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();
    static void updateWindow(bool flagReset = false);

protected:
    void paintEvent(QPaintEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    explicit NoticeUi(QWidget* parent = nullptr);
    ~NoticeUi();

    void doShow();
    void doHide();
    void doClose();
    void moveToActiveWindow();
    void moveToCenterScreen();

    void startFadeEffect();

    NoticeUi(const NoticeUi&) = delete;
    NoticeUi& operator=(const NoticeUi&) = delete;

    QTimer fadeTimer;
    bool isOpen = false;
    FadeState fadeState = FadeState::Idle;
};

#endif // NOTICEUI_H
