#ifndef TASKAIRESULT_H
#define TASKAIRESULT_H

#include <QDialog>
#include <QPointer>
#include <QTextEdit>
#include <QPushButton>
#include <QCloseEvent>

class TaskAIResult : public QDialog {
    Q_OBJECT

public:
    static TaskAIResult* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();
    void setResultText(const QString& text);
    QTextEdit* taskAIResultEditor;

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private:
    explicit TaskAIResult(QWidget* parent = nullptr);
    ~TaskAIResult();

    void fadeIn();
    void fadeOutAndClose();
    void fadeOutAndHide();

    static QPointer<TaskAIResult> m_instance;
};

#endif // FEEDBACK_H
