#ifndef TASKAI_H
#define TASKAI_H

#include "CustomMessageBox.h"
#include "MessageApiKeyBox.h"
#include <QString>
#include <QStringList>
#include <QObject>
#include <QNetworkAccessManager>
#include <QEvent>
#include <QPointer>

class TaskAI : public QObject {
    Q_OBJECT

public:
    static TaskAI& getInstance();
    void run(QPair<QString, QString> dataAI, QString inputBase, bool flagShowNotice = true);
    QSet<QString> setKeyAPIInterrupted;
    void closeWindow();
    bool flagOpenWindow = false;
    bool flagInWindow = false;
    bool flagIsSending = false;
    bool interrupted = false;

private:
    explicit TaskAI(QObject* parent = nullptr);
    ~TaskAI();

    void sendRequest(const QString& prompt, QString codeModel, QString inputBase, int numSpace);
    void showNotice(QString& name);
    QString removeMarkdownFormatting(const QString& input);

    QNetworkAccessManager networkManager;
    QPointer<CustomMessageBox> popup1;
    QPointer<MessageApiKeyBox> popup2;
};

#endif // TASKAI_H
