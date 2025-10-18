#pragma once
#include <QThread>
#include <QMap>
#include <QString>

class CharsetRegistryWatcher : public QThread {
    Q_OBJECT

public:
    static CharsetRegistryWatcher& getInstance();
	QMap<QString, QString> dataCharsetWatcher;

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
};
