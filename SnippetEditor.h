#ifndef SNIPPETEDITOR_H
#define SNIPPETEDITOR_H

#include "CustomMessageBox.h"

#include <QDialog>
#include <QPointer>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include <QComboBox>

class SnippetEditor : public QDialog {
    Q_OBJECT

public:
    std::map<std::wstring, std::wstring> mapSnippetString;
    std::map<std::wstring, std::wstring> mapSnippetWords;
    std::vector<std::pair<std::wstring, std::wstring>> listSnippetString;
    std::vector<std::pair<std::wstring, std::wstring>> listSnippetWords;
    std::set<int> setCountString;
    std::set<int> setCountWords;
    int maxCount;

    static SnippetEditor* getInstance();
    static void showWindow();
    static void hideWindow();
    static void closeWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private:
    explicit SnippetEditor(QWidget* parent = nullptr);
    ~SnippetEditor();

    bool isLoading = false;

    void loadSnippetFromFile();
    void loadWindow(int index);
    void saveWindow(int index);
    void findSetCount();
    void onSaveButtonClicked();
    void onCancelButtonClicked();
    void fadeIn();
    void fadeOut();

    QTableWidget* table;
    QPushButton* saveBtn;
    QPushButton* cancelBtn;
    QComboBox* changeTypeSnippet;

    static QPointer<SnippetEditor> m_instance;
    QPointer<CustomMessageBox> popup;
};

#endif // SNIPPETEDITOR_H
