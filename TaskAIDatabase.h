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
    QStringList listNameTaskAI;
    QSet<QString> listNameDefaultTaskAI;
    QPair<QString, QString> dataCheckSpell;
    QStringList listNameModel;
    int numModelDefault;

    void loadDataTaskAI();
    void loadModel();
    void addDataTaskAIDefault(QMap<QString, QPair<QString, QString>>& dataTaskAI);
    void saveDataTaskAI();
    void saveModel();
    bool isValidTaskContent(const QString& content);
    bool isValidTaskName(const QString& name, const QMap<QString, QPair<QString, QString>>& dataTaskAI, const QString& current);
    QStringList createListNameTaskAI(QMap<QString, QPair<QString, QString>>& dataTaskAI);

private:
    TaskAIDatabase();
    TaskAIDatabase(const TaskAIDatabase&) = delete;
    TaskAIDatabase& operator=(const TaskAIDatabase&) = delete;
};

#endif // TASKAI_DATABASE_H
