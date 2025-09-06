#ifndef KEYAPIMANAGE_H
#define KEYAPIMANAGE_H

#include <QDialog>
#include <QPointer>
#include <QTableWidget>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include <QPushButton>

class KeyAPIManage : public QDialog {
    Q_OBJECT

public:
	QStringList listKeyAPIDefault = {
	};
    QStringList listKeyAPI;
    QStringList listKeyAPIUser;

    static KeyAPIManage* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private:
    explicit KeyAPIManage(QWidget* parent = nullptr);
    ~KeyAPIManage();
    void loadTable();
    void onTableChanged(QTableWidgetItem* item);
    void onsaveButtonClicked();
    void onCancelButtonClicked();
	void onCreateKeyButtonClicked();
    void fadeIn();
    void fadeOut();

    bool isLoading = false;

    QTableWidget* table;
	QPushButton* createKeyBtn;
    QPushButton* saveBtn;
    QPushButton* cancelBtn;
    static QPointer<KeyAPIManage> m_instance;

};

#endif // KEYAPIMANAGE_H
