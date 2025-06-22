#pragma once

#include "CustomConfirmBox.h"
#include <QObject>
#include <QSystemTrayIcon>
#include <QPointer>

class QMenu;

class TrayIcon : public QObject {
    Q_OBJECT

public:
    static TrayIcon& getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();

public slots:
    void onGeneralConfig();
    void onConfigInput();
    void onConfigTaskAI();
    void onConfigSnippetEditor();
    void onResetConfigUi();
    void onFeedback();
    void onHelp();
    void onQuit();

private:
    explicit TrayIcon(QObject* parent = nullptr);
    ~TrayIcon() = default;

    TrayIcon(const TrayIcon&) = delete;
    TrayIcon& operator=(const TrayIcon&) = delete;

    QAction* addActionToMenu(const QString& text, const QString& shortcut, const QString& iconPath, const QObject* receiver, const char* slot);
    void doShow();
    void doHide();
    void doClose();

    QSystemTrayIcon* tray = nullptr;
    QMenu* menu = nullptr;

    QPointer<CustomConfirmBox> confirmResetConfigUi;
};
