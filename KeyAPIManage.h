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
        "AIzaSyB6RTT_W6RYT-mfFUL8uRQ_X6ueklCHzhI",
        "AIzaSyBQHPVnQgzbw353scb6F6RSmFxrAWTTzCs",
        "AIzaSyBWfOxz12WcSUK0yKN-3bgLPNjh6RtiJHg",
        "AIzaSyCPS4ljZI46KqgRslaAn_-cbrajlQgT5wM",
        "AIzaSyCYHvgagPnS2ArTneJTJrOhn6Tbt5vHiDY",

        "AIzaSyCnlfVxs_ghQVOEwH3uo84zGiLHgYwCD2I",
        "AIzaSyAUoeG00fbZkz-JhqWQBDtxQAw_8Uc8EAU",
        "AIzaSyB_-E4PIfj-4NX56leSzztm7kurtvl0D_E",
        "AIzaSyDp1S58addgafrACmzdcUjGNz0a4aJpjSM",
        "AIzaSyArA4bbAbCckXsbI-KL84v2fqohEs_7c-U",

        "AIzaSyAFe9dBEpmkzzz_bQRMODkXb4cExLoxYPA",
        "AIzaSyARsZ8UJiVhVoEsWVznR8GCRHMQc_HYDtc",
        "AIzaSyB_lhSjmT4xzwpaLRing1za0DooDwQwKbk",
        "AIzaSyBJW4rGOK19iFT--5if-sSTCzJIyMgf33Y",
        "AIzaSyBwcL631XCiaSNSBX3mqvjea0tGhHQSFok",

        "AIzaSyCvlgtUnZmmEZj9QWwGL5pptsEjxRsHfOo",
        "AIzaSyDHe0Oz35BAU8avEOevQKqcmX7EAu021Bg",
        "AIzaSyBcjfMNlGw5hyLyyggQK6eqxziBNlBcuwE",
        "AIzaSyAVzET6dBNiYRplNaGpOy-fOu2hDmNohZs",
        "AIzaSyCym08bUu28sBC8UzKPUP8A3-OrjQsJ48E",

        "AIzaSyB3o4a323WOsixUjguRi0ervNYietEv1d0",
        "AIzaSyDFTLTqRtl7h4hxufVjgZQoe4yO3pXfDTY",
        "AIzaSyAgC48ZICpf8bDcRoRQ2nSc1rW3zFvPzys",
        "AIzaSyAviJR2YP7363kth8IhYIq2ExdYBJcP7dg",
        "AIzaSyBm0TgHR3hEW9_lXhAUDUMIlsxucxEbOK8"
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
