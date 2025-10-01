#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "RenameAccountDialog.h"
#include "CustomConfirmBox.h"
#include "CustomMessageBox.h"

#include <QString>
#include <QComboBox>
#include <QPushButton>
#include <QPointer>

class AccountManager : public QDialog {
    Q_OBJECT

public:
    QMap<QString, QString> dataAccounts;
    QString currentAccount;

    static AccountManager* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private:
    explicit AccountManager(QWidget* parent = nullptr);
    ~AccountManager();

    void loadWindow();
    void fadeIn();
    void fadeOut();

    void onNewAccountClicked();
    void onCopyAccountClicked();
    void onRenameAccountClicked();
    void onRemoveAccountClicked();
    void onAccountComboChanged(const QString& account);
    void onCloseButtonClicked();

    void getDataAccounts();
    bool copyAccounts(const QString& accountSrc, const QString& accountDes, bool isMove);
    bool copyKeys(const QString& accountSrc, const QString& accountDes, const QString& groupParent, const QStringList& listKey, bool hasGroupSub, bool clearDestFirst, bool isMove);
	void setCurrentAccount(const QString& account);
    QString addAccounts();
    bool removeAccounts(const QString& account);
    bool exportCurrentAccountToReg(QWidget* parent);


    QComboBox* accountCombo;
    QPushButton* renameAccountBtn;
    QPushButton* copyAccountBtn;
    QPushButton* removeAccountBtn;
    QPushButton* newAccountBtn;
    QPushButton* exportAccountBtn;
    QPushButton* closeAccountBtn;

    QPointer<CustomMessageBox> popup;
    QPointer<RenameAccountDialog> renameAccountDialog;
	QPointer<CustomConfirmBox> confirmRemoveAccount;

    bool isInternalChange = false;

    static QPointer<AccountManager> m_instance;
};

#endif // ACCOUNTMANAGER_H
