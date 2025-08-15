#include "ChangeCase.h"
#include <Variable.h>
#include <Clipboard.h>

#include <QDebug>

#include <chrono>
#include <thread>
#include <TypeWord.h>
#include <Listener.h>

ChangeCase& ChangeCase::getInstance()
{
    static ChangeCase instance;
    return instance;
}

void ChangeCase::run()
{
    Clipboard& clipboard = Clipboard::getInstance();
    Variable& variable = Variable::getInstance();
    TypeWord& typeWord = TypeWord::getInstance();
    Listener& listener = Listener::getInstance();

    if(flagIsProcessing) {
        return;
	}

    flagIsProcessing = true;
    variable.flagSendingKey = true;

    if (textOrigin.isEmpty()) {
        numChangeCase = 0;
        clipboard.simulateCopy();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        textOrigin = QString::fromStdWString(clipboard.getClipboardText());
        textOrigin.replace("\r", "");
        if (!textOrigin.isEmpty()) {
            convertCase();
            std::vector<INPUT> inputs;
            typeWord.addInput(VK_RIGHT, inputs);
            SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
        }
    }

    if (!textOrigin.isEmpty()) {
        int numBackspaceStart = textOrigin.size();

        if (numChangeCase == 0) {
            textOrigin = textTitleCase;
        }
        if (numChangeCase == 1) {
            textOrigin = textUpperCase;
        }
        if (numChangeCase == 2) {
            textOrigin = textLowerCase;
        }
        if (numChangeCase == 3) {
            textOrigin = textSentenceCase;
        }

        if (variable.modeClipboard) {
            typeWord.fixStringDisplayCliboard(numBackspaceStart, textOrigin.toStdWString());
        }
        else {
            typeWord.fixStringDisplay(numBackspaceStart, textOrigin.toStdWString());
        }
        numChangeCase = (numChangeCase + 1) % 4;
	}

    //typeWord.listWord.clear();
    //typeWord.listWord.push_back(Word());
    //typeWord.posWord = 0;
    //typeWord.flagTypeWord = false;

	flagIsProcessing = false;
    variable.flagSendingKey = false;
}

ChangeCase::ChangeCase()
{
    initMapCase();
}

ChangeCase::~ChangeCase() {}

void ChangeCase::initMapCase()
{
    Variable& variable = Variable::getInstance();
    mapLowerCase.clear();
    mapUpperCase.clear();

    for (const auto& dataTemp : variable.mapCharacterSetBase) {
        const auto& characterSetBase = dataTemp.second;
        for (wchar_t key = L'A'; key <= L'M'; ++key) {
            const auto& dataLowers = characterSetBase.at(key);
            const auto& dataUppers = characterSetBase.at(key + 13);

            for (size_t i = 0; i < dataLowers.size(); ++i)
            {
                QString lower = QString::fromStdWString(dataLowers[i]);
                QString upper = QString::fromStdWString(dataUppers[i]);
                mapLowerCase[upper] = lower;
                mapUpperCase[lower] = upper;
            }
        }
    }

    for (wchar_t upperChar = L'A'; upperChar <= L'Z'; ++upperChar)
    {
        wchar_t lowerChar = towlower(upperChar);
        QString upper = QString(QChar(upperChar));
        QString lower = QString(QChar(lowerChar));

        mapLowerCase[upper] = lower;
        mapUpperCase[lower] = upper;
    }
}

void ChangeCase::convertCase()
{
    auto mapChunk = [&](const QString& textSource, int pos, const QMap<QString, QString>& mapCase, int& numUse) -> QString
        {
            for (int length = 2; length > 0; --length)
            {
                if (pos + length <= textSource.size())
                {
                    const QString sub = textSource.mid(pos, length);
                    auto it = mapCase.constFind(sub);
                    if (it != mapCase.cend())
                    {
                        numUse = length;
                        return it.value();
                    }
                }
            }
            numUse = 1;
            return textSource.mid(pos, 1);
        };


    textLowerCase.clear();
    for (int pos = 0; pos < textOrigin.size(); )
    {
        int numUse = 1;
        textLowerCase += mapChunk(textOrigin, pos, mapLowerCase, numUse);
        pos += numUse;
    }

    textUpperCase.clear();
    for (int pos = 0; pos < textOrigin.size(); )
    {
        int numUse = 1;
        textUpperCase += mapChunk(textOrigin, pos, mapUpperCase, numUse);
        pos += numUse;
    }

    textTitleCase.clear();
    {
        bool flagNewWord = true;
        for (int pos = 0; pos < textLowerCase.size(); )
        {
            const QChar character = textLowerCase.at(pos);

            if (character.isSpace() || !character.isLetterOrNumber()) {
                textTitleCase += character;
                flagNewWord = true;
                ++pos;
                continue;
            }

            if (flagNewWord) {
                int numUse = 1;
                textTitleCase += mapChunk(textLowerCase, pos, mapUpperCase, numUse);
                pos += numUse;
                flagNewWord = false;
            }
            else {
                textTitleCase += character;
                ++pos;
            }
        }
    }

    textSentenceCase.clear();
    {
        auto isEndSentencePunct = [](QChar character) {
            return character == '.' || character == '!' || character == '?' || character == '\n' || character == '\r' || character == QChar(0x2026);
            };

        bool flagNewSentence = true;
        for (int pos = 0; pos < textLowerCase.size(); )
        {
            const QChar character = textLowerCase.at(pos);

            if (character.isSpace() || !character.isLetterOrNumber()) {
                textSentenceCase += character;
                if (isEndSentencePunct(character))
                    flagNewSentence = true;
                ++pos;
                continue;
            }

            if (flagNewSentence) {
                int numUse = 1;
                textSentenceCase += mapChunk(textLowerCase, pos, mapUpperCase, numUse);
                pos += numUse;
                flagNewSentence = false;
            }
            else {
                textSentenceCase += character;
                ++pos;
            }
        }
    }
}