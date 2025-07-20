#ifndef CONFIGUI_H
#define CONFIGUI_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QTimer>
#include <QString>
#include <QPushButton>
#include <QLabel>

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
    QComboBox* comboLangVietGlobal = nullptr;
    QComboBox* comboCharacterSet = nullptr;
    QComboBox* comboNameTaskAI = nullptr;
    QComboBox* comboNameSnippetString = nullptr;
    QComboBox* comboNameSnippetWords = nullptr;
    QCheckBox* checkBoxUseDynamic = nullptr;
    QCheckBox* checkBoxClipboard = nullptr;
    QCheckBox* checkBoxFixAutoSuggest = nullptr;
    QCheckBox* checkBoxCheckCase = nullptr;
    QCheckBox* checkBoxTeenCode = nullptr;
    QCheckBox* checkBoxUseLeftRight = nullptr;

    QLabel* labelShortcutLangVietGlobal;
    QLabel* labelShortcutCharacterSet;
    QLabel* labelShortcutTaskAI;
    QLabel* labelShortcutNameSnippetString;
    QLabel* labelShortcutNameSnippetWords;
    QLabel* labelShortcutUseDynamic;
    QLabel* labelShortcutClipboard;
    QLabel* labelShortcutFixAutoSuggest;
    QLabel* labelShortcutCheckCase;
    QLabel* labelShortcutTeenCode;
    QLabel* labelShortcutUseLeftRight;
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
    void updateShortcutLabels();
    void updateCheckBoxStyle(QCheckBox* checkBox, QString stringWarning);

    QPushButton* resetBtn;
    QPushButton* resetAllBtn;
};

#endif // CONFIGUI_H
