#ifndef TASKAIEDITOR_H
#define TASKAIEDITOR_H

#include "CustomMessageBox.h"
#include "RenameDialog.h"
#include <QDialog>
#include <QPointer>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QMap>
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QCloseEvent>
#include <QLabel>

class TaskAIEditor : public QDialog {
    Q_OBJECT

public:

    QMap<QString, QPair<QString, QString>> dataTaskAITemp;
    QMap<QString, QString> dataModelAITemp;
    QStringList listNameTaskAITemp;

    static TaskAIEditor* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private:
    explicit TaskAIEditor(QWidget* parent = nullptr);
    ~TaskAIEditor();

    void loadWindow(const QString& _nameTaskAI = QString());
    void fadeIn();
    void fadeOut();

    void onAddTaskClicked();
    void onRenameTaskClicked();
    void onRemoveTaskClicked();
    void onTaskComboChanged();
    void onModelComboChanged();
    void onEditorChanged();
    void onSaveButtonClicked();
    void onDefaultButtonClicked();
    void onCancelButtonClicked();
    void onKeyAPIButtonClicked();
 
    QComboBox* taskCombo;
    QComboBox* modelCombo;
    QPushButton* addTaskBtn;
    QPushButton* removeTaskBtn;
    QPushButton* renameTaskBtn;
    QLabel* labelShortcut;
    QTextEdit* taskEditor;
    QPushButton* saveBtn;
    QPushButton* defaultBtn;
    QPushButton* cancelBtn;
    QPushButton* keyAPIBtn;

    QPointer<CustomMessageBox> popup;
    QPointer<RenameDialog> renameDialog;
    bool isInternalChange = false;

    static QPointer<TaskAIEditor> m_instance;
};

#endif // TASKAIEDITOR_H
