#include "CharsetRegistryWatcher.h"
#include "Variable.h"
#include "AccountManager.h"

#include <QSettings>
#include <QDebug>
#include <windows.h>

CharsetRegistryWatcher::CharsetRegistryWatcher() {}
CharsetRegistryWatcher::~CharsetRegistryWatcher() {
    stopWatching();
    wait();
}

CharsetRegistryWatcher& CharsetRegistryWatcher::getInstance() {
    static CharsetRegistryWatcher instance;
    return instance;
}

void CharsetRegistryWatcher::startWatching() {
    if (isRunning()) return;

    exitThread = false;
    start();
}

void CharsetRegistryWatcher::stopWatching() {
    exitThread = true;
}

bool CharsetRegistryWatcher::isWatching() const {
    return isRunning();
}

void CharsetRegistryWatcher::run() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Sigma\\AutoCharset", 0, KEY_NOTIFY, &hKey) != ERROR_SUCCESS) {
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

void CharsetRegistryWatcher::handleRegistryChange() {
    if (flagInternalChange) {
        flagInternalChange = false;
        return;
    }

    Variable& variable = Variable::getInstance();
    AccountManager* accountManager = AccountManager::getInstance();
    QString characterSet;

    QMap<QString, QString> mapCharset = {
        { "UNICODE",        "Unicode" },
        { "VNI WINDOWS",    "VNI Windows" },
        { "TCVN3 (ABC)",    "TCVN3 (ABC)" }
    };
    QSettings settings("HKEY_CURRENT_USER\\Software\\Sigma\\AutoCharset", QSettings::NativeFormat);
    QString value = settings.value("Value", "").toString().trimmed().toUpper();
    if (mapCharset.contains(value)) {
        characterSet = mapCharset[value];
		dataCharsetWatcher[variable.nameCurrentWindow] = characterSet;
    }
    else {
        QSettings settings(APP_NAME, "AccountManager");
        settings.beginGroup(accountManager->currentAccount + "/ConfigUi/" + variable.nameCurrentWindow);
        characterSet = settings.value("characterSet", QString::fromStdWString(variable.CHARACTERSET)).toString();
        settings.endGroup();
		dataCharsetWatcher.remove(variable.nameCurrentWindow);
    }
    variable.characterSet = characterSet.toStdWString();
    variable.update();
}
