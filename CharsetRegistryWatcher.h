#pragma once
#include <QThread>

class CharsetRegistryWatcher : public QThread {
    Q_OBJECT

public:
    static CharsetRegistryWatcher& getInstance();

    void startWatching();
    void stopWatching();
    bool isWatching() const;

protected:
    void run() override;

private:
    CharsetRegistryWatcher();
    ~CharsetRegistryWatcher();
    CharsetRegistryWatcher(const CharsetRegistryWatcher&) = delete;
    CharsetRegistryWatcher& operator=(const CharsetRegistryWatcher&) = delete;

    bool exitThread = false;
    bool flagInternalChange = false;

    void handleRegistryChange();
    void resetRegistryValue();
};
