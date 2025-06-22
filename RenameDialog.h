#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPointer>
#include <QPropertyAnimation>
#include "CustomMessageBox.h"

class RenameDialog : public QDialog {
    Q_OBJECT

public:
    QString oldName;

    explicit RenameDialog(QWidget* parent = nullptr);
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

#endif // RENAMEDIALOG_H
