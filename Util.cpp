#include "Util.h"
#include "Variable.h"

#include <cwctype>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <Psapi.h>
#include <Lm.h>
#include <Lmcons.h>
#include <QStringList.h>
#include <QRegularExpression.h>
#include <QFileInfo.h>
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#pragma comment(lib, "Netapi32.lib")


std::wstring toUpperCase(const std::wstring& s)
{
    std::map<std::wstring, std::wstring> dataChangeCaseUpper = Variable::getInstance().dataChangeCaseUpper;
    std::wstring r(s);
    if (dataChangeCaseUpper.find(r) != dataChangeCaseUpper.end()) {
        r = dataChangeCaseUpper[r];
    }
    else {
        for (wchar_t& c : r) {
            c = towupper(c);
        }
    }
    return r;
}

std::wstring toLowerCase(const std::wstring& s)
{
    std::map<std::wstring, std::wstring> dataChangeCaseLower = Variable::getInstance().dataChangeCaseLower;
    std::wstring r(s);
    if (dataChangeCaseLower.find(r) != dataChangeCaseLower.end()) {
        r = dataChangeCaseLower[r];
    }
    else {
        for (wchar_t& c : r) {
            c = towlower(c);
        }
    }
    return r;
}

bool convertVectorToLower(std::vector<std::wstring>& list) {
    if (list.empty()) {
        return false;
    }
    for (std::wstring& str : list) {
        str = toLowerCase(str);
    }
    return true;
}

bool convertVectorToUpper(std::vector<std::wstring>& list) {
    if (list.empty()) {
        return false;
    }
    for (std::wstring& str : list) {
        str = toUpperCase(str);
    }
    return true;
}

bool convertVectorToTitle(std::vector<std::wstring>& list) {
    if (list.empty()) {
        return false;
    }

    bool flagFirstChar = true;
    for (std::wstring& str : list) {
        if (flagFirstChar) {
            str = toUpperCase(str);
            flagFirstChar = false;
        }
        else {
            str = toLowerCase(str);
        }
    }
    return true;
}

std::wstring listCharToString(const std::vector<std::wstring>& list) {
    std::wstring result;
    for (const auto& stringTemp : list) {
        result += stringTemp;
    }
    return result;
}

void disableStickyKeysPopup() {
    STICKYKEYS sk = { sizeof(STICKYKEYS), 0 };
    SystemParametersInfoW(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &sk, 0);
    sk.dwFlags &= ~SKF_HOTKEYACTIVE;  // Tắt Shift 5 lần
    SystemParametersInfoW(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &sk, SPIF_SENDCHANGE);
}

void disableTextSuggestionsSetting() {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Input\\Settings",
        0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {

        DWORD value = 0;
        RegSetValueExW(hKey, L"EnableHwkbTextPrediction", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);
    }
}

QString getFileName(const QString& fileName) {
    QFileInfo fileInfo(fileName);
    QString baseName = fileInfo.completeBaseName();
    return baseName;
}

QString toTitle(const QString& words)
{
    QStringList listWord = words.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    for (QString& word : listWord) {
        if (!word.isEmpty()) {
            word = toTitleWord(word);
        }
    }
    return listWord.join(' ');
}

QString toTitleWord(const QString& word)
{
    if (word.isEmpty()) {
        return QString();
    }
    return word.left(1).toUpper() + word.mid(1).toLower();
}

QString getActiveWindowAppName() {
    HWND hwnd = GetForegroundWindow();
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return "UnknownApp";
    wchar_t buffer[MAX_PATH];
    if (GetModuleBaseNameW(hProcess, nullptr, buffer, MAX_PATH)) {
        CloseHandle(hProcess);
        return getFileName(QString::fromWCharArray(buffer));
    }
    CloseHandle(hProcess);
    return "UnknownApp";
}

bool setAutoStartApp(bool enable) {
    HKEY hKey;
    const wchar_t* runKeyPath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    QString appName = Variable::getInstance().appName;
    const wchar_t* appNameW = (const wchar_t*)appName.utf16();

    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    const wchar_t* appPathW = (const wchar_t*)appPath.utf16();
    if (RegOpenKeyExW(HKEY_CURRENT_USER, runKeyPath, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        return false;
    }
    if (enable) {
        if (RegSetValueExW(hKey, appNameW, 0, REG_SZ, (const BYTE*)appPathW, (DWORD)((wcslen(appPathW) + 1) * sizeof(wchar_t))) != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return false;
        }
    }
    else {
        RegDeleteValueW(hKey, appNameW);
    }
    RegCloseKey(hKey);
    return true;
}

void createAdminTaskInScheduler(bool modeAutoStart, bool modeAdmin)
{
    QString taskName = "SigmaRunAsAdmin";

    if (!modeAutoStart) {
        setAutoStartApp(false);
        QStringList args = { "/Delete", "/F", "/TN", "\"" + taskName + "\"" };
        runTaskHidden("schtasks.exe", args);
        return;
    }

    if (!modeAdmin || !isLoggingByAdmin()) {
        setAutoStartApp(true);
        QStringList args = { "/Delete", "/F", "/TN", "\"" + taskName + "\"" };
        runTaskHidden("schtasks.exe", args);
        return;
    }

    setAutoStartApp(false);
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    QString exeStr = QString::fromWCharArray(exePath);
    QStringList args;

    args << "/Create" << "/F"
        << "/SC" << "ONLOGON"
        << "/TN" << "\"" + taskName + "\""
        << "/TR" << "\"" + exeStr + "\""
        << "/RL" << "HIGHEST";

    runTaskHidden("schtasks.exe", args);
}


bool isLoggingByAdmin()
{
    wchar_t username[UNLEN + 1];
    DWORD usernameLen = UNLEN + 1;

    if (!GetUserNameW(username, &usernameLen)) {
        return false;
    }

    LPLOCALGROUP_USERS_INFO_0 pBuf = NULL;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;

    NET_API_STATUS nStatus = NetUserGetLocalGroups(
        NULL, 
        username,
        0,
        0,
        (LPBYTE*)&pBuf,
        MAX_PREFERRED_LENGTH,
        &entriesRead,
        &totalEntries
    );

    if (nStatus != NERR_Success || pBuf == NULL) {
        return false;
    }

    bool isAdmin = false;
    for (DWORD i = 0; i < entriesRead; ++i) {
        if (_wcsicmp(pBuf[i].lgrui0_name, L"Administrators") == 0) {
            isAdmin = true;
            break;
        }
    }

    NetApiBufferFree(pBuf);
    return isAdmin;
}

bool runTaskHidden(const QString& exe, const QStringList& arguments) {
    QString fullCommand = exe + " " + arguments.join(" ");
    std::wstring cmd = fullCommand.toStdWString();
    wchar_t* cmdLine = &cmd[0];

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL success = CreateProcessW(
        nullptr,
        cmdLine,
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    return false;
}

bool isRunningAsAdmin()
{
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&authority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &adminGroup))
    {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin;
}

QString getActiveWindowTitle() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return "Unknown";

    wchar_t title[256];
    int length = GetWindowTextW(hwnd, title, sizeof(title) / sizeof(wchar_t));

    if (length > 0)
        return QString::fromWCharArray(title);
    else
        return "No Title";
}

std::wstring getExeDirectory() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);

    std::wstring fullPath(path);
    size_t pos = fullPath.find_last_of(L"\\/");
    return fullPath.substr(0, pos);
}

std::wstring convertCharset(const std::wstring& string, std::map<std::wstring, std::wstring>& mapConvert, int& numCharMax)
{
    std::wstring stringResult = L"";
    std::wstring stringSub = L"";
    bool matched;

    int pos = 0;
    while (pos < string.size()) {
        matched = false;
        for (int numChar = numCharMax; numChar >= 1; --numChar) {
            if (pos + numChar <= string.size()) {
                std::wstring key = string.substr(pos, numChar);
                if (mapConvert.count(key)) {
                    matched = true;
                    pos += numChar;
                    stringSub = mapConvert[key];
                    break;
                }
            }
        }
        if (!matched) {
            stringSub = string.substr(pos, 1);
            ++pos;
        }
        stringResult += stringSub;
    }
    return stringResult;
}

