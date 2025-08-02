#ifndef WORD_H
#define WORD_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "Variable.h"
#include "Util.h"

class Word {
public:
    bool flagLangViet;
    bool flagAddCharSpace;
    bool flagAddCharInvalid;
    int numSwitchLang;
    int stateAdd;
    int numCharD;
    wchar_t stateD;
    wchar_t stateTone;
    wchar_t stateDiac;
    wchar_t charQuickVowel;
    std::vector<std::wstring> listCharSpaceNew;
    std::vector<std::wstring> listCharSpaceCurrent;
    std::vector<std::wstring> listCharVowel;
    std::vector<std::wstring> listCharDisplayCurrent;
    std::vector<std::wstring> listCharDisplayNew;
    std::vector<std::wstring> listCharOrigin;
    std::vector<std::wstring> listCharViet;
    std::vector<std::wstring> listCharVietInvalid;
    std::vector<std::wstring> listCharVietInvalidTemp;
    std::vector<std::wstring> listCharVietStart;
    std::vector<std::wstring> listCharVietMiddle;
    std::vector<std::wstring> listCharVietEnd;
    std::unordered_set<wchar_t> keyPassDiac;
    std::unordered_set<wchar_t> keyPassEnd;
    int numBackspace;
    std::wstring stringAdd;
    int modeChangeCase;
    int numChangeCase;
    bool flagChangeLangEng;

    Word();
    bool addChar(wchar_t character, bool flagMustAdd = false);
    bool addCharSpace(wchar_t character);
    bool addCharStart(wchar_t character, bool flagAddBack);
    bool addCharTemplate(wchar_t character, bool flagAddBack, std::vector<std::wstring>& listChar, bool (Word::* callBackCheckChar)(wchar_t, std::vector<std::wstring>&, std::vector<wchar_t>*), std::vector<wchar_t>* listParam = nullptr);
    bool addCharMiddle(wchar_t character, bool flagAddBack);
    bool addCharEnd(wchar_t character, bool flagAddBack);
    bool addCharInvalid(wchar_t character);
    bool addKeyTemplate(wchar_t character, wchar_t state, std::map<wchar_t, std::vector<wchar_t>>& dataAddChar, bool (Word::* callBackCheckKey)(wchar_t, wchar_t));
    bool addKeyRemoveDiacToneD(wchar_t character);
    bool addKeyD(wchar_t character);
    bool addKeyDiac(wchar_t character);
    bool addKeyTone(wchar_t character);
    bool checkKeyD(wchar_t character, wchar_t stateDTemp);
    bool checkKeyDiac(wchar_t character, wchar_t stateDiacTemp);
    bool checkKeyTone(wchar_t character, wchar_t stateToneTemp);
    bool checkCharGI(wchar_t character = L'\0');
    bool checkCharOnsetStart(wchar_t character, std::vector<std::wstring>& listCharVietStartTemp, std::vector<wchar_t>* listParam);
    bool checkCharD();
    bool checkCharVowel(wchar_t character, std::vector<std::wstring>& listCharVowelTemp, std::vector<wchar_t>* listParam);
    bool checkMiddle(wchar_t stateToneTemp, wchar_t stateDiacTemp, std::vector<std::wstring>& listCharVowelTemp, bool checkResetDiac);
    bool checkCharEnd(wchar_t character, std::vector<std::wstring>& listCharVietEndTemp, std::vector<wchar_t>* listParam);
    bool checkEnd(std::vector<std::wstring>& listCharVietEndTemp);
    bool checkEndInvalid();
    bool checkPosTone();
    void removeChar();
    bool removeCharSpace();
    bool removeCharStart();
    bool removeCharMiddle();
    bool removeCharEnd();
    bool removeCharInvalid();
    bool changeCase();
    bool changeLowerCase();
    bool changeUpperCase();
    bool changeTitleCase();
    bool changeCharSet();
    bool checkKeyNumpad();
    void checkAutoChangeLang();
    void updateListCharDisplay();
    void findListCharDisplay();
    void calStepChangeDisplay();
    void switchLang(bool* flagLangVietTemp = nullptr);
    void addDataAutoChangeLang();
	void removeDataAutoChangeLang();
    void clearListCharVietInvalid();
};

#endif
