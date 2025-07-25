#ifndef RENAMESNIPPETDIALOG_H
#define RENAMESNIPPETDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPointer>
#include <QPropertyAnimation>
#include "CustomMessageBox.h"

class RenameSnippetDialog : public QDialog {
    Q_OBJECT

public:
    QString oldName;

    explicit RenameSnippetDialog(QWidget* parent = nullptr);
    QString getNewName() const;
    void setOldName(QString name);
    void showWindow();
    void hideWindow();
    void closeWindow();

protected:
    void showEvent(QShowEvent* event) override;
    void reject() override;

private:
    void fadeIn();
    void onAcceptClicked();

    QLineEdit* lineEdit;
    QPointer<CustomMessageBox> popup;
};

#endif // RENAMESNIPPETDIALOG_H
