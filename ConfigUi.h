#ifndef CONFIGUI_H
#define CONFIGUI_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QTimer>
#include <QString>
#include <QPushButton>

class ConfigUi : public QWidget {
    Q_OBJECT

public:
    static ConfigUi& getInstance();

    static void showWindow(QWidget* parent = nullptr);
    static void hideWindow();
    static void closeWindow();

    void saveSettings(QString appNameConfig, QString nameValue, QString value);
    void loadSettings();

    bool m_isLoading = false;
    QComboBox* m_comboCharacterSet = nullptr;
    QComboBox* m_comboNameTaskAI = nullptr;
    QCheckBox* m_checkBoxModeClipboard = nullptr;
    QCheckBox* m_checkBoxModeUseDynamic = nullptr;
    QCheckBox* m_checkBoxModeFixAutoSuggest = nullptr;
    QCheckBox* m_checkBoxModeCheckCase = nullptr;
    QCheckBox* m_checkBoxModeTeenCode = nullptr;
    QCheckBox* m_checkBoxModeInsertChar = nullptr;
    QString m_AppNameConfig;

protected:
    void closeEvent(QCloseEvent* event) override;
    bool event(QEvent* e) override;

private:
    explicit ConfigUi(QWidget* parent = nullptr);
    ~ConfigUi();

    void doShow(QWidget* parent = nullptr);
    void doHide();
    void doClose();
    void onResetButtonClicked();
    void onResetAllButtonClicked();

    QPushButton* resetBtn;
    QPushButton* resetAllBtn;
};

#endif // CONFIGUI_H
