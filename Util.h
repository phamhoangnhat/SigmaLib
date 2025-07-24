#ifndef UTIL_H 
#define UTIL_H

#include <string>
#include <vector>
#include <QString>
#include <map>

template<typename T>
inline T valueMax(const T& a, const T& b) {
    return (a > b) ? a : b;
}

template<typename T>
inline T valueMin(const T& a, const T& b) {
    return (a < b) ? a : b;
}

std::wstring toUpperCase(const std::wstring& s);
std::wstring toLowerCase(const std::wstring& s);
bool convertVectorToLower(std::vector<std::wstring>& list);
bool convertVectorToUpper(std::vector<std::wstring>& list);
bool convertVectorToTitle(std::vector<std::wstring>& list);
std::wstring listCharToString(const std::vector<std::wstring>& list);
void disableStickyKeysPopup();
void disableTextSuggestionsSetting();
QString getFileName(const QString& fileName);
QString toTitle(const QString& words);
QString toTitleWord(const QString& word);
QString getActiveWindowAppName();
bool setAutoStartApp(bool enable);
void createAdminTaskInScheduler(bool modeAutoStart, bool modeAdmin);
bool isLoggingByAdmin();
bool runTaskHidden(const QString& exe, const QStringList& arguments);
bool isRunningAsAdmin();
QString getActiveWindowTitle();
std::wstring getExeDirectory();
std::wstring convertCharset(const std::wstring& string, std::map<std::wstring, std::wstring>& mapConvert, int& numCharMax);

#endif // UTIL_H
