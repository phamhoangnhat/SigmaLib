#ifndef CUSTOMINPUTMETHOD_H
#define CUSTOMINPUTMETHOD_H

#include "CustomMessageBox.h"
#include <QDialog>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <vector>

class CustomInputMethod : public QDialog {
    Q_OBJECT

public:
    static CustomInputMethod* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();

protected:
    void closeEvent(QCloseEvent* event) override;;
    void reject() override;

private:
    explicit CustomInputMethod(QWidget* parent = nullptr);
    ~CustomInputMethod();

    void changeInput(int index);
    void saveChanges();
    void loadFromSettings();
    void loadDefault();

    static QPointer<CustomInputMethod> m_instance;
    std::vector<QLineEdit*> inputFields;
    std::vector<int> indexListChar;
    QPushButton* saveBtn = nullptr;
    QPushButton* cancelBtn = nullptr;
    QPushButton* defaultBtn = nullptr;
    QPointer<CustomMessageBox> popup;
    bool flagUpdatingInput = false;
};

#endif // CUSTOMINPUTMETHOD_H
