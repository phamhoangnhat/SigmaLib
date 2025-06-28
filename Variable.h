#ifndef VARIABLE_H
#define VARIABLE_H

#include "version.h"
#include <string>
#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <tuple>
#include <QString>
#include <Qset>

class Variable {
public:
    const std::wstring CHARACTERSET = L"Unicode";
    const std::wstring INPUTMETHOD = L"Tích hợp";
    const bool FLAGLANGVIETGLOBAL = false;
    const bool MODEAUTOSTART = true;
    const bool MODEAUTOUPDATE = true;
    const bool MODEUSESNIPPET = true;
    const bool MODELOOPDIACTONE = false;
    const bool MODEUSEDYNAMIC = false;
    const bool MODEINSERTCHAR = false;
    const bool MODEAUTOADDVOWEL = false;
    const bool MODESHORTCUTLAST = false;
    const bool MODECHECKCASE = false;
    const bool MODETEENCODE = false;
    const bool MODEAUTOCHANGELANG = true;
    const QString NAMETASKAI = "01. Kiểm tra chính tả tiếng Việt";

    QString appName = APP_NAME;
    QString version = APP_VERSION_STRING;
    QString appNameFull = appName + " " + version;
    QString nameCurrentWindow = "";
    QString namePreviousWindow = "";
    QString nameTaskAI;
    std::wstring characterSet;
    std::wstring inputMethod;
    bool flagLangVietGlobal;
    bool modeAutoStart;
    bool modeAutoUpdate;
    bool modeUseSnippet;
    bool modeRestore;
    bool modeLoopDiacTone;
    bool modeUseDynamic;
    bool modeClipboard;
    bool modeFixAutoSuggest;
    bool modeInsertChar;
    bool modeAutoAddVowel;
    bool modeShortcutLast;
    bool modeUseLeftRight;
    bool modeCheckCase;
    bool modeTeenCode;
    bool modeAutoChangeLang;
    bool flagSendingKey = false;
    int vkCodePrevious;

    std::map<std::wstring, std::map<wchar_t, std::vector<std::wstring>>> mapCharacterSetBase;
    std::map<std::wstring, std::vector<std::wstring>> mapInputMethodBase;
    std::vector<wchar_t> listStateBase;
    std::vector<int> listOrderStatePush;
    std::vector<int> listOrderStateQuick;
    std::unordered_set<std::wstring> dataCheckCharOnsetStartBase;
    std::vector<std::tuple<wchar_t, std::wstring, int, std::wstring, std::wstring>> listDataMiddleBase;
    std::vector<std::tuple<wchar_t, std::wstring, int, std::wstring, std::wstring>> listDataMiddleBaseOrigin;
    std::map<std::wstring, wchar_t> dataRemoveCharMiddle2;
    std::map<std::wstring, wchar_t> dataRemoveCharMiddle2Origin;
    std::map<std::wstring, std::map<std::wstring, std::wstring>> mapDataEndBase;
    std::map<std::wstring, std::map<std::wstring, std::wstring>> mapDataEndBaseOrigin;
    std::unordered_set<int> setVirtualKeyCodeValid;
    std::unordered_set<wchar_t> setCharSpaceSnippet;
    std::map<std::wstring, std::wstring> mapUniToVni;
    std::map<std::wstring, std::wstring> mapUniToAbc;
    std::unordered_set<std::wstring> setPunctuation;
    std::vector<std::wstring> dataOnsetStartTeenCode;
    std::vector<std::wstring> dataOnsetEndTeenCodeTotal;
    std::vector<std::wstring> dataOnsetEndTeenCode;
    std::unordered_set<std::wstring> dataCheckRetore;

    std::map<wchar_t, std::vector<std::wstring>> characterSetBase;
    std::unordered_set<std::wstring> dataCheckCharOnsetStart;
    std::vector<std::wstring> inputMethodBase;
    std::map<wchar_t, std::unordered_set<wchar_t>> dataAddCharMiddle1;
    std::map<wchar_t, std::pair<wchar_t, wchar_t>> dataAddCharMiddle2;
    std::unordered_set<wchar_t> dataAddKeyRemoveDiacToneD;
    std::map<wchar_t, std::vector<wchar_t>> dataAddKeyTone;
    std::map<wchar_t, std::vector<wchar_t>> dataAddKeyDiac;
    std::map<wchar_t, std::vector<wchar_t>> dataAddKeyD;
    std::map<std::wstring, std::vector<wchar_t>> dataCheckMiddle1;
    std::map<std::wstring, std::vector<std::wstring>> dataCheckMiddle2;
    std::unordered_set<std::wstring> dataCheckCharGI1;
    std::unordered_set<wchar_t> dataCheckCharGI2;
    std::map<std::wstring, std::map<std::wstring, std::wstring>> dataCheckEnd;
    std::unordered_set<std::wstring> dataRemoveCharMiddle1;
    std::map<std::wstring, std::vector<std::wstring>> dataCheckPosTone;
    std::unordered_set<wchar_t> dataAddCharSpace;
    std::unordered_set<wchar_t> dataCharKeyStartValid;
    std::map<std::wstring, std::wstring> dataChangeCaseLower;
    std::map<std::wstring, std::wstring> dataChangeCaseUpper;
    std::unordered_set<std::wstring> dataAutoChangeLang;

    QSet<QString> listAppUseClipboard;
    QSet<QString> listAppFixAutoSuggest;
    QSet<QString> listAppNotUseLeftRight;

    static Variable& getInstance() {
        static Variable instance;
        return instance;
    }

    void init();
    void update();

    Variable(const Variable&) = delete;
    Variable& operator=(const Variable&) = delete;
    QString validateKeySequence(const QString& raw, int indexChar);
    bool loadSettingsWindow();
    void saveDataAutoChangeLang(QString& nameApp);
    void loadDataAutoChangeLang(QString& nameApp);

private:
    Variable();
    ~Variable();

    void initGeneralConfig();
    void initMapCharacterSetBase();
    void initMapInputMethodBase();
    void initListStateBase();
    void initListOrderState();
    void initDataCheckCharOnsetStartBase();
    void initListDataMiddleBaseOrigin();
    void initDataRemoveCharMiddle2Origin();
    void initMapDataEndBaseOrigin();
    void initSetVirtualKeyCodeValid();
    void initSetCharSpaceSnippet();
    void initMapConvertCharset();
    void initSetPunctuation();
    void initDataOnsetStartTeenCode();
    void initDataOnsetEndTeenCodeTotal();
    void initDataCheckRetore();

    std::vector<std::wstring> createInputMethodBase();
    std::unordered_set<std::wstring> createDataCheckCharOnsetStart();
    std::vector<std::tuple<wchar_t, std::wstring, int, std::wstring, std::wstring>> createListDataMiddleBase();
    std::map<std::wstring, wchar_t> createDataRemoveCharMiddle2();
    std::map<std::wstring, std::map<std::wstring, std::wstring>> createMapDataEndBase();
    std::vector<std::wstring> createDataOnsetEndTeenCode();
    std::unordered_set<wchar_t> createDataAddKeyRemoveDiacToneD();
    std::map<wchar_t, std::vector<wchar_t>> createDataAddKey(int numStart, int numEnd, std::vector<int> listStateBase);
    std::map<wchar_t, std::unordered_set<wchar_t>> createDataAddCharMiddle1();
    std::map<wchar_t, std::pair<wchar_t, wchar_t>> createDataAddCharMiddle2();
    std::map<std::wstring, std::vector<wchar_t>> createDataCheckMiddle1();
    std::map<std::wstring, std::vector<std::wstring>> createDataCheckMiddle2();
    std::unordered_set<std::wstring> createDataCheckCharGI1();
    std::unordered_set<wchar_t> createDataCheckCharGI2();
    std::map<std::wstring, std::map<std::wstring, std::wstring>> createDataCheckEnd();
    std::unordered_set<std::wstring> createDataRemoveCharMiddle1();
    std::map<std::wstring, std::vector<std::wstring>> createDataCheckPosTone();
    std::unordered_set<wchar_t> createDataAddCharSpace();
    std::unordered_set<wchar_t> createDataCharKeyStartValid();
    std::map<std::wstring, std::wstring> createDataChangeCaseLower();
    std::map<std::wstring, std::wstring> createDataChangeCaseUpper();
};

#endif // VARIABLE_H
