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
        "AIzaSyBTVUIada5EfK0lYKLgMSN4JRRvPi7oLKE",
        "AIzaSyCg1YlirGbKMB3gp28WhmKyGSPhjmuZuNw",
        "AIzaSyCLFafK9jhSd2AnIAOGSWMN-x1guwy9WAE",
        "AIzaSyCT9kz2x_vbzgCtHlFOZWWnNabGcFUG8sY",
        "AIzaSyBfxkPGehb4NZRKE2tcz0eCLbReIUZett8",

        "AIzaSyA6r_oVWxCrxuW12f4cJcDdpzY3i-0fpuo",
        "AIzaSyBdSvGvAv8LwfzsuBnEHIOI83w8bfJ2qCI",
        "AIzaSyC-VOXDwucZ2wzbm4cIUrLtIla-HURpLZQ",
        "AIzaSyCgeBcCMkf8HRxi0BiGXCG7sIlNI-s3uxs",
        "AIzaSyBR5HZThIVgaX_lGeHUYCHgGU4z8qyYGpU",

        "AIzaSyCGdRvUsP9STjffXwEosziGkeyfRi-OLI4",
        "AIzaSyBjVKq1sL4D04crFsixhl-SR-qf9tsSYvs",
        "AIzaSyDTq25nwRJl_AdWzKoz6YLOyHnT0SMrHv4",
        "AIzaSyDWTIdDCUky1WvWxCCj-dUjFtmK20fcA10",
        "AIzaSyBAgwb-Y5lhWJYjG84uO_aW1w8bJXx-3qg",

        "AIzaSyBfHMgivHaXKFbq2CdQJ36XQAA4dI2nS_A",
        "AIzaSyAiVWU07_6HcGtj_qUeHPk-zNi2Kj5hmG8",
        "AIzaSyCV-QCic55XqIU0bOTSeh6tb5awkfInXgg",
        "AIzaSyDrb6jLSDZvyY8oOd0pkKuPK4jIvP-5pTI",
        "AIzaSyDr15OiNpTDMAQ12AvpjD2KBTw-vAwARpo",

        "AIzaSyBjfQOzbn-akMO8WC2xDiLqL91NR91E_1U",
        "AIzaSyDZSibe4jLStI1vPn-2k9hpnPh0E2yP37A",
        "AIzaSyBXfEnl6VvMPcNlDiAkwB2PtpgDPcTuLZU",
        "AIzaSyD80sR_yaaiz08sF2haW46aAR_wfG9jRs0",
        "AIzaSyAmZ2l4IHvMd7R3dIYsAUJq7K50ckoI4Bw",

        "AIzaSyDxVfqIJSUHwuuZnjreeLZhiuFdaNSoPK4",
        "AIzaSyBlXNOJYGEDoEShLwtTQUdvVf14GYlJCCk",
        "AIzaSyBhXRpRS-71x52615oqc-vQUyZcrT2mzZ4",
        "AIzaSyDlttUhAVdqw3BuoDdogvvkb0pi4P9KIwI",
        "AIzaSyBiMje7QE62FO-GhtCpUQCOUOfXIqGNEwM",

        "AIzaSyC4ZEwn496U3I88mMFex-B_K1PVL8jxyOY",
        "AIzaSyDReHlbVqmNHiikBTEqLr9jF3dgeDo0VZ4",
        "AIzaSyBXXYYR2dlg5dMRf-XI_RBTe_4WZpLggig",
        "AIzaSyBLFjBheHJQKQm1eURLl4-qn3KOtJQQCuo",
        "AIzaSyB037J61HLZDF2sAjUhBOjIROJCbCCXxOk",

        "AIzaSyBv8hNzk9uSfRtSCmY2LJ4pqiTMXqBqI30",
        "AIzaSyCP99OUKEg6h-KAChcNynRrOrt3rVF1-f4",
        "AIzaSyC-fH6o7izhgkRscqGd-pZb_Xv4NWauG0c",
        "AIzaSyAWrtDh7VHnbmh_Gq7Bacfrzyzc6Dv-WhA",
        "AIzaSyCKRdaSMDVNzD99NifhKtOKxFZRvu7EPT4",

        "AIzaSyAirM_SlfhLDs82rT6YzrOYG1M1rbxfeAA",
        "AIzaSyC19rqXxfbieevu3hh7caZwSzTf7aUuwLc",
        "AIzaSyCW18f-O3cbYGuCbSRbYiR8NqQckVNILJ0",
        "AIzaSyA78XG2asgtrXv9zrXSlABXdx7hPqTs8yQ",
        "AIzaSyC1bgcoVPhptukyHf7r7p4QA17jce4Uf30",

        "AIzaSyBMeT5GPTHmVkahlfVqtu5AuPmk8hL6eRM",
        "AIzaSyDLAHCsKrcp2by6xezmTlFMWVIFtsion90",
        "AIzaSyAPxCiWMrlnyqz0za3Wlg1DmUjRBo9mfwI",
        "AIzaSyBoFLa4AxiR6s8o24u7ypI-Pq8_DO27ecE",
        "AIzaSyBj2ti5HIwPtdG7dLMuo0h_MR1lJ91mCdA",

        "AIzaSyBFdQuH53GuNeVBgOBv7-iHq8fppXI9Nqk",
        "AIzaSyA2dOopBlumtSsPicmbRBrQgQxlabz7LMc",
        "AIzaSyBXxp2yLgTR3T4dK-Z5vsWxF3AQQWn2V84",
        "AIzaSyBcamKaAc1ornzBrowWm-FcrFUHqWf6WbI",
        "AIzaSyDqLfI2HaY1tH87kCQJqPoJx-73pscskzE",

        "AIzaSyC3L5viSAKdwxxs8dD_aMkJJPn_Gt9jlnU",
        "AIzaSyBfgNxCJRhakeCl1zUI42QztjKAta61ufw",
        "AIzaSyCy-82CS3y9gv2D0Xq4Sy5AK9U2pF7WTYA",
        "AIzaSyBDIJoXkOeDpXE-2xKSXT7-R7XwyosUnV8",
        "AIzaSyDhSNpQuwZaDOOGmuvcSLUo8QobxtuA41c",

        "AIzaSyBAGcXmD7-aGpbX3wMUdnC5kJnfHCfgP8g",
        "AIzaSyDOFpMHn36YBGaOt4r622x-JxtQ83KUbYU",
        "AIzaSyDZq3fDdKFXKa0t-sz1Oa1zXbvNadojwGM",
        "AIzaSyCYi5r0U1S3Ahtauf1GFGRv3VuK4qnbx1c",
        "AIzaSyDnN0m0GZJIIXrwI-YpoXuU4YF7rVsO61Y",

        "AIzaSyBRD9eRjcEBXc21vcVrBDD9Or6ESJL6P-s",
        "AIzaSyD43pJsr9yUUb9ZaYlZJcQMDCVP91qPaLU",
        "AIzaSyAudMG2kdiOJZ7wWv1GPiRN-okxzKwfjmI",
        "AIzaSyAd4p7bKHTMEu_ZP4wgpPZAJ7T4f-nJEoE",
        "AIzaSyAcVBHmUsMmFS_Uq0Cccev_8r-t62XP9-U",

        "AIzaSyDg18d0w5esbJxFVrLsg6buZoxm9t4zPD0",
        "AIzaSyARamaz_chWBNppa6fDrl2j0-gqVqzYFPg",
        "AIzaSyBcIZJgM-WtXrusX9E53CJcMvNqsBjYUiI",
        "AIzaSyDHH2nGPdqn1VFoVAAsWVJ5YhgozoVvT70",
        "AIzaSyCXfotmL5MTDz4Vrl6cdO1_ZNswwq4jmXc",

        "AIzaSyB0p8JhnPgQZLVZSN1Fwz1o4SoUxssXeCE",
        "AIzaSyA3ukfk-vu-8n-42ydW_8smvE8WKsguwco",
        "AIzaSyBgkVVyYQyJyoNmNSO7qNk01v_BcsYMSi8",
        "AIzaSyBmmLGfqlCFEOVHgScgk7p-oWT0EzranUI",
        "AIzaSyBL8UdRTQbbsQEKlLEFLcZ5JGvTbCroS-0",

        "AIzaSyBRq029XnzQyJ7pfrngxykei1t4LskFeEg",
        "AIzaSyDv7Se1dTSaw_Us53lT-h5ibV4vvr0dNPQ",
        "AIzaSyCPur7To_Zv6opr7BVq7FaRonxArNsx-gg",
        "AIzaSyBx6KCZMx0yY9LzN7c3DgJ3khVC-TCdeSY",
        "AIzaSyAHuMI9Dy6FSOBZ_43NXSxw_RjOW80MJ-0",

        "AIzaSyALZSHVJ1QHql5sSHF98PwBw7vRNbb1zcU",
        "AIzaSyBFAmULWUd3ZWUYFm-lz7MVnHu5Wn4hwFU",
        "AIzaSyB3k06vijWu6oZGux0DWg3uq_cz7-AvyiU",
        "AIzaSyBtpMbsfWJwGs7bGeoNN8ymEAnFxjg65c8",
        "AIzaSyDD6P6GQXi8844e3SY8CNnCSxMcky-CCyc",

        "AIzaSyATYw2UQtlX0jizWUxBD4_nXYF0myg18Po",
        "AIzaSyDLRnD5t45G_wePJ7LonTFTY0jszqd60a4",
        "AIzaSyBhHOU7A0f7fyfbdSk8SFrR3GuVwy_rajw",
        "AIzaSyAwWeHbs9D76y1QemGk32mNA6TmmVOkqgU",
        "AIzaSyAVwV3mPDVpJpXhfJPLyELFuVOKJScaYoo",

        "AIzaSyBu8vYl2gU444Hqpq0G9uaje7J0rg75_ZM",
        "AIzaSyB_6Xa0KLzRtBK7CVYJogkcZk31CAsdEGs",
        "AIzaSyCSvpmu4iHsMBDCXuIi7p7-0CuqXLMumio",
        "AIzaSyCV2bAidgm8J-KSr799ff1HLHdvHc2uKig",
        "AIzaSyAdSKRU4NRLqUsw62gl89jn6TCGVdDINfs",
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
