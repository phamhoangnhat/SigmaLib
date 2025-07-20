#ifndef SNIPPETEDITOR_H
#define SNIPPETEDITOR_H

#include "CustomMessageBox.h"
#include "RenameSnippetDialog.h"

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
    QString nameSnippet;
    QSet<QString> listNameSnippetUpper;
    QSet<QString> listNameSnippet;
    QMap<QString, std::set<int>> mapCountSnippet;
    QMap<QString, QList<QPair<QString, QString>>> dataSnippetTable;
    std::map<QString, std::map<std::wstring, std::wstring>> dataSnippet;
    std::map<QString, std::map<std::wstring, std::wstring>> dataSnippetTotal;

    std::map<std::wstring, std::wstring> mapSnippetString;
    std::map<std::wstring, std::wstring> mapSnippetWords;
    std::set<int> setCountString;
    std::set<int> setCountWords;
    int maxCount;

    static SnippetEditor* getInstance();
    void loadSnippetForApp(QString nameSnippetString, QString nameSnippetWords);
    static void showWindow();
    static void hideWindow();
    static void closeWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void reject() override;

private:
    explicit SnippetEditor(QWidget* parent = nullptr);
    ~SnippetEditor();
    void loadSnippetFromFile();
    void updateTotal();
    void loadNameSnippetCombo();
    void onNameSnippetComboChanged();
	void onRenameSnippetButtonClicked();
	void onRemoveSnippetButtonClicked();
	void onAddSnippetButtonClicked();
    void onTableChanged(QTableWidgetItem* item);
    void loadTable();
    void onsaveButtonClicked();
    void onCancelButtonClicked();
    void fadeIn();
    void fadeOut();

    bool isLoading = false;

    QComboBox* nameSnippetCombo;
    QPushButton* addSnippetBtn;
    QPushButton* removeSnippetBtn;
    QPushButton* renameSnippetBtn;
    QTableWidget* table;
    QPushButton* saveBtn;
    QPushButton* cancelBtn;

    static QPointer<SnippetEditor> m_instance;
    QPointer<RenameSnippetDialog> renameDialog;
};

#endif // SNIPPETEDITOR_H
