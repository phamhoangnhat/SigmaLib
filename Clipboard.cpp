#include "Clipboard.h"
#include "Variable.h"
#include "Listener.h"
#include <TaskAI.h>

#include <thread>
#include <chrono>
#include <iostream>
#include <cstring>
#include <QDebug>
#include <ChangeCase.h>


using namespace std;

namespace {
	const wchar_t* CLIPBOARD_WND_CLASS = L"HiddenClipboardListenerClass";

	ATOM registerHiddenWindowClass(HINSTANCE hInstance) {
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = Clipboard::WndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = CLIPBOARD_WND_CLASS;
		return RegisterClassEx(&wc);
	}
}

Clipboard& Clipboard::getInstance() {
	static Clipboard instance;
	return instance;
}

Clipboard::Clipboard() {}

Clipboard::~Clipboard() {
	stop();
}

wstring Clipboard::getClipboardText() {
	if (!OpenClipboard(NULL)) {
		return L"";
	}

	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (!hData) {
		CloseClipboard();
		return L"";
	}

	wchar_t* clipboardText = static_cast<wchar_t*>(GlobalLock(hData));
	if (!clipboardText) {
		CloseClipboard();
		return L"";
	}

	wstring text(clipboardText);
	GlobalUnlock(hData);
	CloseClipboard();
	return text;
}

void Clipboard::setClipboardText(const wstring& text) {
	//qDebug() << "setClipboardText start: " << text;

	if (!OpenClipboard(NULL)) return;

	EmptyClipboard();

	size_t size = (text.length() + 1) * sizeof(wchar_t);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
	if (!hMem) {
		CloseClipboard();
		return;
	}

	void* pLockedMem = GlobalLock(hMem);
	if (!pLockedMem) {
		GlobalFree(hMem);
		CloseClipboard();
		return;
	}
	flagBaseClipboard = false;
	bool flagUpdateBaseClipboardOld = flagUpdateBaseClipboard;
	flagUpdateBaseClipboard = false;

	memcpy(pLockedMem, text.c_str(), size);
	GlobalUnlock(hMem);

	if (!SetClipboardData(CF_UNICODETEXT, hMem)) {
		GlobalFree(hMem);
		flagUpdateBaseClipboard = flagUpdateBaseClipboardOld;
	}

	CloseClipboard();
	//qDebug() << "setClipboardText finish: " << text;
}

void Clipboard::getBaseClipboard()
{
	if (!flagBaseClipboard) {
		baseClipboard = getClipboardText();
		flagBaseClipboard = true;
		//qDebug() << "Get base clipboard: " << QString::fromStdWString(baseClipboard);
	}
}

void Clipboard::setBaseClipboard()
{
	if (!flagBaseClipboard && flagClipboardText) {
		//qDebug() << "Set base clipboard start: " << QString::fromStdWString(baseClipboard);

		flagUpdateBaseClipboard = false;
		setClipboardText(baseClipboard);
		flagBaseClipboard = true;

		//qDebug() << "Set base clipboard finish: " << QString::fromStdWString(baseClipboard);
	}
}

void Clipboard::sendUnicodeText(const wstring& text, int timeDelay) {
	Listener& listener = Listener::getInstance();

	//qDebug() << "sendUnicodeText Start";
	if (text.size() != 0) {
		setClipboardText(text);
		if (timeDelay != 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(timeDelay));
		}
		bool flagShiftLeftPress = ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0);
		bool flagShiftRightPress = ((GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0);
		if (flagShiftLeftPress) {
			listener.releaseKey(VK_LSHIFT);
		}
		if (flagShiftRightPress) {
			listener.releaseKey(VK_RSHIFT);
		}
		INPUT inputs[4] = { 0 };
		inputs[0].type = INPUT_KEYBOARD;
		inputs[0].ki.wVk = VK_CONTROL;
		inputs[0].ki.dwFlags = 0;
		inputs[1].type = INPUT_KEYBOARD;
		inputs[1].ki.wVk = 'V';
		inputs[1].ki.dwFlags = 0;
		inputs[2].type = INPUT_KEYBOARD;
		inputs[2].ki.wVk = 'V';
		inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
		inputs[3].type = INPUT_KEYBOARD;
		inputs[3].ki.wVk = VK_CONTROL;
		inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(4, inputs, sizeof(INPUT));

		if (flagShiftLeftPress) {
			listener.pressKey(VK_LSHIFT);
		}
		if (flagShiftRightPress) {
			listener.pressKey(VK_RSHIFT);
		}
	}
	//qDebug() << "sendUnicodeText Finish";
}

void Clipboard::simulateCopy() {
	flagUpdateBaseClipboard = false;
	flagBaseClipboard = false;
	INPUT inputs[4] = {};

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_CONTROL;
	inputs[0].ki.dwFlags = 0;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = 'C';
	inputs[1].ki.dwFlags = 0;

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = 'C';
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_CONTROL;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(4, inputs, sizeof(INPUT));
}

void Clipboard::start() {
	if (hWndListener) return;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	if (!registerHiddenWindowClass(hInstance)) {
		return;
	}

	hWndListener = CreateWindowEx(
		0,
		CLIPBOARD_WND_CLASS,
		L"HiddenClipboardListenerWindow",
		0,
		0, 0, 0, 0,
		HWND_MESSAGE,
		NULL,
		hInstance,
		NULL
	);

	if (!hWndListener) return;

	AddClipboardFormatListener(hWndListener);
	getBaseClipboard();
	flagClipboardText = (baseClipboard != L"") ? true : false;
}

void Clipboard::stop() {
	if (hWndListener) {
		RemoveClipboardFormatListener(hWndListener);
		DestroyWindow(hWndListener);
		hWndListener = NULL;
		setBaseClipboard();
	}
}

LRESULT CALLBACK Clipboard::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_CLIPBOARDUPDATE) {
		Clipboard& clipboard = Clipboard::getInstance();
		TaskAI& taskAI = TaskAI::getInstance();
		ChangeCase& changeCase = ChangeCase::getInstance();

		if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
			clipboard.flagClipboardText = true;
			if (clipboard.flagUpdateBaseClipboard && !taskAI.flagIsSending && !changeCase.flagIsProcessing) {
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				clipboard.flagBaseClipboard = false;
				clipboard.getBaseClipboard();
			}
		}
		else {
			clipboard.flagClipboardText = false;
		}
		clipboard.flagUpdateBaseClipboard = true;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}