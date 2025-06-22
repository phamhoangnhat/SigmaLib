#ifndef FEEDBACK_H
#define FEEDBACK_H

#include "CustomMessageBox.h"
#include <QDialog>
#include <QPointer>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QSettings>

class Feedback : public QDialog {
    Q_OBJECT

public:
    static Feedback* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();
    bool flagSending = false;

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private:
    explicit Feedback(QWidget* parent = nullptr);
    ~Feedback();

    void fadeIn();
    void fadeOutAndClose();
    void fadeOutAndHide();

    void onSendClicked();
    void onCancelClicked();
    void sendFeedback(const QString& userFeedback);
    void loadFeedbackFromSettings();
    void saveFeedbackToSettings();

    QTextEdit* feedbackEditor;
    QPushButton* sendButton;
    QPushButton* cancelButton;

    QNetworkAccessManager networkManager;
    QPointer<CustomMessageBox> popup;

    static QPointer<Feedback> m_instance;
};

#endif // FEEDBACK_H
