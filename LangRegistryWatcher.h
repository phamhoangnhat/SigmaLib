#pragma once
#include <QThread>

class LangRegistryWatcher : public QThread {
    Q_OBJECT

public:
    static LangRegistryWatcher& getInstance();

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
    void resetRegistryValue();
};
