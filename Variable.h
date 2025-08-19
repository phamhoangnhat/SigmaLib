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
    const std::wstring INPUTMETHOD = L"Tích hợp";
    const bool MODEAUTOUPDATE = true;
    const bool MODEADMIN = false;
    const bool MODEAUTOSTART = true;
    const bool MODERESTORE = true;
    const bool MODEAUTOCHANGELANG = false;
    const bool MODEREMOVEDIACTONE = false;
    const bool MODELOOPDIACTONE = false;
    const bool MODEINSERTCHAR = false;

    const std::wstring CHARACTERSET = L"Unicode";
    const QString NAMETASKAI = "01. Kiểm tra chính tả tiếng Việt";
    const QString NAMESNIPPETSTRING = "Không sử dụng";
    const QString NAMESNIPPETWORDS = "Không sử dụng";
    const bool MODEUSEDYNAMIC = false;
    const bool MODECHECKCASE = false;
    const bool MODETEENCODE = false;
    const bool MODEUSELEFTRIGHT = false;

    QString appName = APP_NAME;
    QString version = APP_VERSION_STRING;
    QString appNameFull = appName + " " + version;
    QString nameCurrentWindow = "";
    QString namePreviousWindow = "";
    double verSigmaExe;

    std::wstring inputMethod;
    bool modeAutoStart;
    bool modeAdmin;
    bool modeAutoUpdate;
    bool modeRestore;
    bool modeAutoChangeLang;
    bool modeRemoveDiacTone;
    bool modeLoopDiacTone;
    bool modeInsertChar;

    bool flagLangVietGlobal;
    std::wstring characterSet;
    QString nameTaskAI;
    QString nameSnippetString;
    QString nameSnippetWords;
    bool modeUseDynamic;
    bool modeClipboard;
    bool modeFixAutoSuggest;
    bool modeCheckCase;
    bool modeTeenCode;
    bool modeUseLeftRight;

    bool flagSendingKey = false;
    int vkCodePrevious;
    int vkCodeCurrent;

    std::map<std::wstring, std::map<wchar_t, std::vector<std::wstring>>> mapCharacterSetBase;
    std::map<std::wstring, std::vector<std::wstring>> mapInputMethodBase;
    std::vector<wchar_t> listStateBase;
    std::vector<int> listOrderStatePush;
    std::vector<int> listOrderStateQuick;
    std::unordered_set<std::wstring> dataCheckCharOnsetStartBase;
    std::vector<std::tuple<wchar_t, std::wstring, int, std::wstring, std::wstring>> listDataMiddleBase;
    std::map<std::wstring, wchar_t> dataRemoveCharMiddle2;
    std::map<std::wstring, std::map<std::wstring, std::wstring>> mapDataEndBase;
    std::unordered_set<int> setVirtualKeyCodeValid;
    std::unordered_set<wchar_t> setCharSpaceSnippet;
    std::map<std::wstring, std::wstring> mapUniToVni;
    std::map<std::wstring, std::wstring> mapUniToAbc;
    std::unordered_set<std::wstring> setPunctuation;
    std::vector<std::wstring> dataOnsetStartTeenCode;
    QSet<QChar> dataValidateKeyToneDiac;

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
    std::unordered_set<wchar_t> dataAddNewWord;
    std::unordered_set<wchar_t> dataAddCharSpace;
    std::unordered_set<wchar_t> dataCheckModeRestore;
    std::map<std::wstring, std::wstring> dataChangeCaseLower;
    std::map<std::wstring, std::wstring> dataChangeCaseUpper;
    std::unordered_set<std::wstring> dataAutoChangeLang;

    QSet<QString> listAppLangVietGlobal;
    QSet<QString> listAppUseClipboard;
    QSet<QString> listAppFixAutoSuggest;

    static Variable& getInstance() {
        static Variable instance;
        return instance;
    }

    void init();
    void update();

    Variable(const Variable&) = delete;
    Variable& operator=(const Variable&) = delete;
    bool loadSettingsWindow();
    void addKeyInputMethod(const QString& stringRaw, int indexChar, std::vector<std::wstring>& inputMethodBase);
    void saveDataAutoChangeLang(QString& nameApp);
    void loadDataAutoChangeLang(QString& nameApp);

private:
    Variable();
    ~Variable();

    void initMapCharacterSetBase();
    void initMapInputMethodBase();
    void initListStateBase();
    void initListOrderState();
    void initDataCheckCharOnsetStartBase();
    void initListDataMiddleBase();
    void initDataRemoveCharMiddle2();
    void initMapDataEndBase();
    void initSetVirtualKeyCodeValid();
    void initSetCharSpaceSnippet();
    void initMapConvertCharset();
    void initSetPunctuation();
    void initDataOnsetStartTeenCode();
    void initDataValidateKeyToneDiac();
    void loadGeneralConfig();

    std::vector<std::wstring> createInputMethodBase();
    std::unordered_set<std::wstring> createDataCheckCharOnsetStart();
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
    std::unordered_set<wchar_t> createDataAddNewWord();
    std::unordered_set<wchar_t> createDataAddCharSpace();
    std::unordered_set<wchar_t> createDataCheckModeRestore();
    std::map<std::wstring, std::wstring> createDataChangeCaseLower();
    std::map<std::wstring, std::wstring> createDataChangeCaseUpper();
};

#endif // VARIABLE_H
