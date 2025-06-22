#ifndef GENERALCONFIG_H
#define GENERALCONFIG_H

#include <QDialog>
#include <QPointer>
#include <QCloseEvent>
#include <QCheckBox>
#include <QComboBox>

class GeneralConfig : public QDialog {
    Q_OBJECT

public:
    static GeneralConfig* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();

    QComboBox* comboInputMethod = nullptr;
    QCheckBox* checkBoxAutoStart;
    //QCheckBox* checkBoxAutoUpdate;
    QCheckBox* checkBoxUseSnippet;
    QCheckBox* checkBoxLoopDiacTone;
    QCheckBox* checkBoxAutoAddVowel;
    QCheckBox* checkBoxShortcutLast;
    QCheckBox* checkBoxAutoChangeLang;
    QCheckBox* checkBoxUseLeftRight;

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

    QPushButton* saveBtn;
    QPushButton* defaultBtn;
    QPushButton* cancelBtn;

    static QPointer<GeneralConfig> m_instance;
};

#endif // GENERALCONFIG_H
