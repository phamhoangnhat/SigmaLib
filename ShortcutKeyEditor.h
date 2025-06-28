#ifndef SHORTCUTKEYEDITOR_H
#define SHORTCUTKEYEDITOR_H

#include "CustomMessageBox.h"
#include <QDialog>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <vector>

class ShortcutKeyEditor : public QDialog {
    Q_OBJECT

public:
    QStringList listShortcutKey;
    QMap<QString, QString> dataShortcutKeyDefault;
    QMap<QString, QString> dataShortcutKeyCurrent;
    QMap<QString, QString> dataShortcutKey;
    QMap<int, QString> mapVKCodeToShortcut;

    static ShortcutKeyEditor* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();
    QString getShortcutKey(const QString& key);
    QString getAction(const int& vkCode);

protected:
    void closeEvent(QCloseEvent* event) override;;
    void reject() override;

    void changeShortcutKey(int index);
    void saveChanges();
    void loadFromSettings();
    void loadDefault();
    bool validateShortcut(const QString& shortcutKey, const QMap<QString, QString>& dataShortcutKeyTemp);

private:
    explicit ShortcutKeyEditor(QWidget* parent = nullptr);
    ~ShortcutKeyEditor();

    bool flagUpdating = false;
    QMap<QString, QString> dataShortcutKeyReverse;

    static QPointer<ShortcutKeyEditor> m_instance;
    QVector<QLineEdit*> shortcutEdits;
    QPushButton* saveBtn = nullptr;
    QPushButton* cancelBtn = nullptr;
    QPushButton* defaultBtn = nullptr;
    QPointer<CustomMessageBox> popup;
};

#endif // SHORTCUTKEYEDITOR_H
