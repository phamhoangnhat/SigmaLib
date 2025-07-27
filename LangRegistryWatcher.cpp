#include "LangRegistryWatcher.h"
#include "Variable.h"
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
    resetRegistryValue();
    start();
}

void LangRegistryWatcher::stopWatching() {
    exitThread = true;
}

bool LangRegistryWatcher::isWatching() const {
    return isRunning();
}

void LangRegistryWatcher::resetRegistryValue() {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Sigma\\AutoLang", QSettings::NativeFormat);
    QString current = settings.value("Value", "").toString().trimmed();
    if (!current.isEmpty()) {
        flagInternalChange = true;
        settings.setValue("Value", "");
    }
    else {
        flagInternalChange = false;
    }
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
    bool flagLangVietGlobal;

    QSettings settings("HKEY_CURRENT_USER\\Software\\Sigma\\AutoLang", QSettings::NativeFormat);
    QString value = settings.value("Value", "").toString().trimmed().toLower();

    if (value == "true" || value == "false") {
        flagLangVietGlobal = (value == "true");
    }
    else {
        QSettings settings(variable.appName, "ConfigUi");
        settings.beginGroup(variable.nameCurrentWindow);
        flagLangVietGlobal = settings.value("flagLangVietGlobal", variable.FLAGLANGVIETGLOBAL).toBool();
    }
    variable.flagLangVietGlobal = flagLangVietGlobal;
    resetRegistryValue();
}
