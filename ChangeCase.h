#ifndef CHANGECASE_H
#define CHANGECASE_H

#include <QMap>
#include <QString>

class ChangeCase
{
public:
    QString textOrigin;
    QString textLowerCase;
    QString textUpperCase;
    QString textTitleCase;
    QString textSentenceCase;
    int numChangeCase = 0;
	bool flagIsProcessing;

    static ChangeCase& getInstance();
    void run();

    ChangeCase(const ChangeCase&) = delete;
    ChangeCase& operator=(const ChangeCase&) = delete;
    ChangeCase(ChangeCase&&) = delete;
    ChangeCase& operator=(ChangeCase&&) = delete;

private:

    ChangeCase();
    ~ChangeCase();
    void initMapCase();
    void convertCase();

    QMap<QString, QString> mapLowerCase;
    QMap<QString, QString> mapUpperCase;
};

#endif // CHANGECASE_H
