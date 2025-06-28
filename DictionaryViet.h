#ifndef DICTIONARYVIET_H
#define DICTIONARYVIET_H

#include <string>
#include <unordered_set>

class DictionaryViet {
public:
    std::unordered_set<std::wstring> dataDictionaryViet;

    static DictionaryViet& getInstance() {
        static DictionaryViet instance;
        return instance;
    }

    void init();
    bool isWord(const std::wstring& word);

private:
    DictionaryViet();
    ~DictionaryViet();

    DictionaryViet(const DictionaryViet&) = delete;
    DictionaryViet& operator=(const DictionaryViet&) = delete;
};

#endif // DICTIONARYVIET_H
