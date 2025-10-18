#pragma once
#include <QThread>
#include <QMap>
#include <QString>

class LangRegistryWatcher : public QThread {
    Q_OBJECT

public:
    static LangRegistryWatcher& getInstance();
    QMap<QString, bool> dataLangWatcher;

    void startWatching();
    void stopWatching();
    bool isWatching() const;

protected:
    void run() override;

private:
    LangRegistryWatcher();
    ~LangRegistryWatcher();
    LangRegistryWatcher(const LangRegistryWatcher&) = delete;
    LangRegistryWatcher& operator=(const LangRegistryWatcher&) = delete;

    bool exitThread = false;
    bool flagInternalChange = false;

    void handleRegistryChange();
};
