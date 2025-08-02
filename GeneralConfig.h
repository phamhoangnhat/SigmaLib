#ifndef GENERALCONFIG_H
#define GENERALCONFIG_H

#include "CustomMessageBox.h"

#include <QDialog>
#include <QPointer>
#include <QCloseEvent>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

class GeneralConfig : public QDialog {
    Q_OBJECT

public:
    static GeneralConfig* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();

    QComboBox* comboInputMethod = nullptr;
    QCheckBox* checkBoxAutoStart;
    QCheckBox* checkBoxAdmin;
    //QCheckBox* checkBoxAutoUpdate;
    QCheckBox* checkBoxRestore;
    QCheckBox* checkBoxRemoveDiacTone;
    QCheckBox* checkBoxLoopDiacTone;
    QCheckBox* checkBoxInsertChar;
    QCheckBox* checkBoxAutoChangeLang;
    QLabel* noteLabel;

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private:
    explicit GeneralConfig(QWidget* parent = nullptr);
    ~GeneralConfig();

    void loadWindow();
    void fadeIn();
    void fadeOut();

    void onSaveButtonClicked();
    void onDefaultButtonClicked();
    void onCancelButtonClicked();
    void updateShortcutLabels();
    void updateCheckBoxStyle(QString modeName, bool modeValue);
    void updateNoteLabelVisibility();

    QPushButton* saveBtn;
    QPushButton* defaultBtn;
    QPushButton* cancelBtn;
    QLabel* labelShortcutInputMethod = nullptr;
    QLabel* labelShortcutAutoStart = nullptr;
    QLabel* labelShortcutAdmin = nullptr;
    QLabel* labelShortcutRestore = nullptr;
    QLabel* labelShortcutRemoveDiacTone = nullptr;
    QLabel* labelShortcutLoopDiacTone = nullptr;
    QLabel* labelShortcutInsertChar = nullptr;
    QLabel* labelShortcutAutoChangeLang = nullptr;

    QPointer<CustomMessageBox> popup;
    static QPointer<GeneralConfig> m_instance;
};

#endif // GENERALCONFIG_H
