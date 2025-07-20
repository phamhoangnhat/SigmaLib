#ifndef TYPEWORD_H
#define TYPEWORD_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <Windows.h>
#include "Variable.h"
#include "Word.h"
#include "Util.h"
#include <QColor>

class TypeWord {
public:
    std::vector<Word> listWord;
    std::wstring stringSnippet;
    int numKeySnippet;
    int posWord;
    bool flagTypeWord;
    QColor colorUI;

    static TypeWord& getInstance();

    void switchLangGlobal();
    void switchLang();
    void addChar(wchar_t character);
    void removeChar();
    void removeWordBack();
    void moveLeft();
    void moveRight();
    void addWord(int posWordTemp);
    void addWordBack();
    void removeWord();
    void changeCase();
    void reset(bool flagCheckChangeLangEng = false);
    void changeCharSet();
    void changeInputMethod();
    void changeConfigUi(QString nameMode);
    void changeGeneralConfig(QString nameMode);
    void fixStringDisplayCliboard(int numBackspaceStart, std::wstring stringAdd);
    void fixStringDisplay(int numBackspaceStart, std::wstring stringAdd);
    void addInput(int vkCode, std::vector<INPUT>& inputs);
    void addInput(wchar_t character, std::vector<INPUT>& inputs);
    void showLanguage();
    void showCharSet();
    void showInputMethod();
    void showChangeConfig(QString nameMode);
    void calStringSnippet();
    void updateDataChangeLang(Word& wordPrevious);
    std::wstring createStringDisplayAll();

private:
    TypeWord();
    ~TypeWord();

    TypeWord(const TypeWord&) = delete;
    TypeWord& operator=(const TypeWord&) = delete;
};

#endif // TYPEWORD_H
