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
		"AIzaSyCq5ssec28VIY5z3QK7kni1wXYMk2lv-FE",
		"AIzaSyBbkqpDxe9sw-Hy5UWTmscPGlMxneysTpA",
		"AIzaSyBHyPTLPTBlaDKh8jWl7STb7MKPUluygSQ",
		"AIzaSyCFtSeSRCZzhDaimLjGEzuXpYmvsVHEwow",
		"AIzaSyDbRsIrTDuImdfdZXDNITHvz_HXAa6_p2g",

		"AIzaSyBfHgo2K86d7OoAWdNI6ZFLf81nQyx_KeM",
		"AIzaSyDeAwp9HlXwGNi9aTRuWpuWjxmqWlYz2VM",
		"AIzaSyAb27LPzK_N8KY8_to_IZw5OSFD1QU5uvA",
		"AIzaSyBC7AhFmHL6CC1CYblrCYGvRhg2U6XfQ5Y",
		"AIzaSyC27eG-7b_Pw-dnbQ8Rc1AAcKj4oFR2VEk",

		"AIzaSyAZGPpXlOl0WET1Uy6ll5LN6EWG7lR6dCM",
		"AIzaSyAX4IwvyYVJcpnm5JYFKSF42mK5OI0Xh5o",
		"AIzaSyDvhrPH1q1NLIG3EzlpNVBP-VwzYg41eGw",
		"AIzaSyAmsx826jKHrbt8Smstuag8PVlCr6M-a2I",
		"AIzaSyApkU-_bXa5kuwCxZnm3HSchphtl8bmUpE",

		"AIzaSyApvOwr8Edg0TilCiWDk67FcAAWjwiG3SE",
		"AIzaSyD7lM_aF1Gih-WmSEt_p9-937PT43khqZQ",
		"AIzaSyBSdlYe3XqScBVR6WU72xribQ7vlPzFf8s",
		"AIzaSyCU87QTNzlAdlDaWV_Z4pD8MgMkcHfIi2Y",
		"AIzaSyBIHNv4YnPT6T9ByAotstpCe5H0cEvZMTQ",

		"AIzaSyA_-SIyRsw5bNOtH498hb3fosRGZrirG_o",
		"AIzaSyBewkmO37FKXGHH2AsDegb8DhhRBrKHdk0",
		"AIzaSyC-g8EAdugSESxh34FT0BHq2uLiz5Pqzr4",
		"AIzaSyBnQ9Dr62JIUEACtSpcXcATxIh9kcrGfWU",
		"AIzaSyC93MzVP-ugt-juwaCQ2pGHwLEQtcvNDIU",

		"AIzaSyAnWT9rSMTpc1VCxP3-qt1Qw34WMcWy4bs",
		"AIzaSyAabE1Qg4VAPaxmpB0MkIlFUESjLnBMPWU",
		"AIzaSyDtjwQ6yERTB2i332Iw1DwM3l3edaEEX0s",
		"AIzaSyCcnebYP6hWWjKiu5pAFJ35F1Tspe4xNks",
		"AIzaSyBikvivPDz7dtSgj7jeoPG7TjWPSfn5Pi8",

		"AIzaSyDbrXIVTPy1P0WZwRjq7Ti2L7yyj85H_7c",
		"AIzaSyCQCRBc8VlqhtW-DbrM0IzgB-6E-0gQY70",
		"AIzaSyDT4drywyTP2TSraQAOz11EvQtyIJbLB9U",
		"AIzaSyCgepoim_MdnXwDG4YR4hG0VDz6noVT904",
		"AIzaSyB8Y5c98NhFd0wZmjv5LJG3_5cXPHg2stY",

		"AIzaSyAbS9H4THyCeTtC4ajehgdp5iOjE7Yehow",
		"AIzaSyDXRWVI_Ew9ShJ1QvhibZevMwpRWSIOF-M",
		"AIzaSyAQVrgsno-Bcu_lKimoIKG-7KJFK_gxTaA",
		"AIzaSyCQJ31eCeHcv18gK_TLI5fny8vFtK1Q0Ag",
		"AIzaSyA4J0CD-lgTFh9zRXAT-w_AcBueOmF__l0",

		"AIzaSyAx5X3nY5OmHw1nhdC7d3FKBahMGGMnqZA",
		"AIzaSyBAcQb2z9LT4zYlZvtrtu9FOhCc6eHTPes",
		"AIzaSyDNJbgfReRsOQm9C-pl-8Im8StyQgq166A",
		"AIzaSyBaswLAJpvMh1ApTgF9TgNr9z9nuy2VEhs",
		"AIzaSyC3zMKYG_QhA7GgbtlVvtFtvKkzFG5fZnM",

		"AIzaSyCz3QaPl0y420DNHSnbHpJL1ZyntbYEM-g",
		"AIzaSyBhG9xANovoD2unhzZfUy_o6z_DBESAVC0",
		"AIzaSyAdOEaxiM__h6MvlDDHHKMh7HTZI8b70E0",
		"AIzaSyD3es1Y3jdbYFNjRXXPioNOg2yrYxQS8us",
		"AIzaSyD8bw4fzU6PBcd8JhaoD5EuXgdZ2ZWL7q0",
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
    void fadeIn();
    void fadeOut();

    bool isLoading = false;

    QTableWidget* table;
    QPushButton* saveBtn;
    QPushButton* cancelBtn;
    static QPointer<KeyAPIManage> m_instance;

};

#endif // KEYAPIMANAGE_H
