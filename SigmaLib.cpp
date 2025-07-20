#include "SigmaLib.h"
#include "Util.h"
#include "Variable.h"
#include "NoticeUi.h"
#include "TrayIcon.h"
#include "Clipboard.h"
#include "Listener.h"
#include "ConfigUi.h"
#include "GeneralConfig.h"
#include "CustomInputMethod.h"
#include "CustomMessageBox.h"
#include "TaskAIEditor.h"
#include "TaskAIResult.h"
#include "TaskAI.h"
#include "Feedback.h"
#include "SnippetEditor.h"
#include "ShortcutKeyEditor.h"
#include <KeyAPIManage.h>

#include <QApplication>
#include <QTimer>
#include <QWidget>
#include <QDebug>
#include <QAbstractNativeEventFilter>
#include <QByteArray>

#include <windows.h>
#include <wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

static bool isRunning = false;
static QWidget* sessionNotifier = nullptr;

class SessionEventFilter : public QAbstractNativeEventFilter {
public:
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_WTSSESSION_CHANGE) {
            if (msg->wParam == WTS_SESSION_UNLOCK) {
                qDebug() << "Session unlocked → Restarting SigmaLib";
                QTimer::singleShot(500, [] {
                    SigmaLib::getInstance().run();
                    });
            }
            else if (msg->wParam == WTS_SESSION_LOCK) {
                qDebug() << "Session locked";
            }
        }
        return false;
    }
};

SigmaLib::SigmaLib() {}

SigmaLib::~SigmaLib() {}

SigmaLib& SigmaLib::getInstance() {
    static SigmaLib instance;
    return instance;
}

void SigmaLib::start() {
    if (isRunning) return;
    isRunning = true;

    if (!sessionNotifier) {
        sessionNotifier = new QWidget();
        sessionNotifier->setAttribute(Qt::WA_DontShowOnScreen);
        WTSRegisterSessionNotification(reinterpret_cast<HWND>(sessionNotifier->winId()), NOTIFY_FOR_THIS_SESSION);

        static SessionEventFilter* filter = new SessionEventFilter();
        qApp->installNativeEventFilter(filter);
    }

    run();
}

void SigmaLib::run()
{
    disableStickyKeysPopup();
    disableTextSuggestionsSetting();

    Variable::getInstance().init();
    ConfigUi::getInstance();
    NoticeUi::updateWindow(true);
    TrayIcon::showWindow();
    Clipboard::getInstance().start();
    Listener::getInstance().start();
}

void SigmaLib::stop() {
    if (!isRunning) return;
    isRunning = false;

    GeneralConfig::closeWindow();
    ConfigUi::closeWindow();
    NoticeUi::closeWindow();
    TrayIcon::closeWindow();
    CustomInputMethod::closeWindow();
    TaskAIEditor::closeWindow();
    KeyAPIManage::closeWindow();
    TaskAIResult::closeWindow();
    Feedback::closeWindow();
    SnippetEditor::closeWindow();
    TaskAI::getInstance().closeWindow();
    Clipboard::getInstance().stop();
    Listener::getInstance().stop();
    ShortcutKeyEditor::closeWindow();
    QTimer::singleShot(1500, qApp, &QCoreApplication::quit);
}

extern "C" {

    SIGMALIB_EXPORT void sigmalib_start() {
        SigmaLib::getInstance().start();
    }

    SIGMALIB_EXPORT void sigmalib_stop() {
        SigmaLib::getInstance().stop();
    }

}
