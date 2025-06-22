#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <string>
#include <windows.h>

class Clipboard {
public:
    static Clipboard& getInstance();

    bool flagUpdateBaseClipboard = true;
    bool flagBaseClipboard = false;
    bool flagClipboardText;
    std::wstring baseClipboard = L"";

    std::wstring getClipboardText();
    void setClipboardText(const std::wstring& text);
    void getBaseClipboard();
    void setBaseClipboard();
    void sendUnicodeText(const std::wstring& text);
    void simulateCopy();
    void start();
    void stop();
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    Clipboard(const Clipboard&) = delete;
    Clipboard& operator=(const Clipboard&) = delete;

private:
    HWND hWndListener{ nullptr };

    Clipboard();
    ~Clipboard();
};

#endif // CLIPBOARD_H
