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
    void onShortcutKeyEditor();
    void onConfigSnippetEditor();
    void onResetConfigUi();
    void onFeedback();
    void onSigmaGroup();
    void onHelp();
    void onQuit();

private:
    explicit TrayIcon(QObject* parent = nullptr);
    ~TrayIcon() = default;

    TrayIcon(const TrayIcon&) = delete;
    TrayIcon& operator=(const TrayIcon&) = delete;

    QAction* addActionToMenu(const QString& text, const QString& iconPath, const QObject* receiver, const char* slot);
    void updateMenuShortcutText();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void doShow();
    void doHide();
    void doClose();

    QSystemTrayIcon* tray = nullptr;
    QMenu* menu = nullptr;

    QPointer<CustomConfirmBox> confirmResetConfigUi;

    QAction* actionGeneralConfig = nullptr;
    QAction* actionConfigInput = nullptr;
    QAction* actionConfigTaskAI = nullptr;
    QAction* actionShortcutKeyEditor = nullptr;
    QAction* actionSnippetEditor = nullptr;
    QAction* actionResetConfigUi = nullptr;
    QAction* actionFeedback = nullptr;
    QAction* actionSigmaGroup = nullptr;
    QAction* actionHelp = nullptr;
    QAction* actionQuit = nullptr;
};
