// TaskAIDatabase.h
#ifndef TASKAI_DATABASE_H
#define TASKAI_DATABASE_H

#include <QMap>
#include <QString>
#include <QPair>
#include <QStringList>
#include <QSet>

class TaskAIDatabase {
public:
    static TaskAIDatabase& getInstance();

    QMap<QString, QPair<QString, QString>> dataTaskAI;
    QMap<QString, QString> dataModelAI;
    QMap<QString, QString> dataNameModel;
    QMap<QString, QString> dataShortcutAI;
    QMap<QString, QString> dataShortcutAICheck;
    QStringList listNameTaskAI;
    QSet<QString> listNameDefaultTaskAI;
    QPair<QString, QString> dataCheckSpell;
    QStringList listNameModel;
    QStringList listShortcutAI;
    int numModelDefault;

    void loadDataTaskAI();
    void loadDataModel();
    void loadDataShortcut();
    void addDataTaskAIDefault(
        QMap<QString, QPair<QString, QString>>& dataTaskAI,
        QMap<QString, QString>& dataModelAI,
        QMap<QString, QString>& dataShortcutAI,
        QMap<QString, QString>& dataShortcutAICheck);
    void saveDataTaskAI();
    void saveDataModel();
    void saveDataShortcut();
    bool isValidTaskContent(const QString& content);
    bool isValidTaskName(const QString& name, const QMap<QString, QPair<QString, QString>>& dataTaskAI, const QString& current);
    QStringList createListNameTaskAI(QMap<QString, QString>& dataShortcutAI, QMap<QString, QString>& dataShortcutAICheck);

private:
    TaskAIDatabase();
    TaskAIDatabase(const TaskAIDatabase&) = delete;
    TaskAIDatabase& operator=(const TaskAIDatabase&) = delete;
};

#endif // TASKAI_DATABASE_H
