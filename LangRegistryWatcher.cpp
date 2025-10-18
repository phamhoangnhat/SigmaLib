#include "LangRegistryWatcher.h"
#include "Variable.h"
#include "AccountManager.h"

#include <QSettings>
#include <QDebug>
#include <windows.h>

LangRegistryWatcher::LangRegistryWatcher() {}

LangRegistryWatcher::~LangRegistryWatcher() {
    stopWatching();
    wait();
}

LangRegistryWatcher& LangRegistryWatcher::getInstance() {
    static LangRegistryWatcher instance;
    return instance;
}

void LangRegistryWatcher::startWatching() {
    if (isRunning()) return;

    exitThread = false;
    start();
}

void LangRegistryWatcher::stopWatching() {
    exitThread = true;
}

bool LangRegistryWatcher::isWatching() const {
    return isRunning();
}

void LangRegistryWatcher::run() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Sigma\\AutoLang", 0, KEY_NOTIFY, &hKey) != ERROR_SUCCESS) {
        return;
    }

    while (!exitThread) {
        if (RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, NULL, FALSE) == ERROR_SUCCESS) {
            handleRegistryChange();
        }
        QThread::msleep(10);
    }

    RegCloseKey(hKey);
}

void LangRegistryWatcher::handleRegistryChange() {
    if (flagInternalChange) {
        flagInternalChange = false;
        return;
    }

    Variable& variable = Variable::getInstance();
    AccountManager* accountManager = AccountManager::getInstance();
    bool flagLangVietGlobal;

    QMap<QString, bool> mapLang = {
        { "TRUE",   true},
        { "FALSE",  false}
    };
    QSettings settings("HKEY_CURRENT_USER\\Software\\Sigma\\AutoLang", QSettings::NativeFormat);
    QString value = settings.value("Value", "").toString().trimmed().toUpper();

    if (mapLang.contains(value)) {
        flagLangVietGlobal = mapLang[value];
		dataLangWatcher[variable.nameCurrentWindow] = flagLangVietGlobal;
    }
    else {
        bool modeLangVietGlobalDefault = variable.listAppLangVietGlobal.contains(variable.nameCurrentWindow.toLower()) ? true : false;
        QSettings settings(APP_NAME, "AccountManager");
        settings.beginGroup(accountManager->currentAccount + "/ConfigUi/" + variable.nameCurrentWindow);
        flagLangVietGlobal = settings.value("flagLangVietGlobal", modeLangVietGlobalDefault).toBool();
        settings.endGroup();
		dataLangWatcher.remove(variable.nameCurrentWindow);
    }

    variable.flagLangVietGlobal = flagLangVietGlobal;
}
