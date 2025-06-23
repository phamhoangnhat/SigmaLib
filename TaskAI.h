#ifndef TASKAI_H
#define TASKAI_H

#include "CustomMessageBox.h"
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
    void run(QString keyTaskAI);
    void closeWindow();
    bool flagOpenWindow = false;
    bool flagInWindow = false;
    bool flagIsSending = false;
    bool interrupted = false;

private:
    explicit TaskAI(QObject* parent = nullptr);
    ~TaskAI();

    void sendRequest(const QString& prompt);
    void showNotice(QString& name);
    QString removeMarkdownFormatting(const QString& input);

    QStringList listKeyAPI;
    QNetworkAccessManager networkManager;
    QPointer<CustomMessageBox> popup;
};

#endif // TASKAI_H
