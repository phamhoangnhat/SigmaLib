#include "Listener.h"
#include "Variable.h"
#include "Clipboard.h"
#include "Util.h"
#include "ConfigUi.h"
#include "TaskAI.h"
#include "TaskAIEditor.h"
#include "TaskAIDatabase.h"
#include "NoticeUi.h"
#include "TrayIcon.h"
#include <ShortcutKeyEditor.h>

#include <windows.h>
#include <chrono>
#include <thread>
#include <QTimer>
#include <qsettings>
#include <ChangeCase.h>

HHOOK Listener::keyboardHook = NULL;
HHOOK Listener::mouseHook = NULL;

Listener& Listener::getInstance() {
	static Listener instance;
	return instance;
}

Listener::Listener() {
	init();
}

Listener::~Listener() {}

void Listener::start() {
	stop();

	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookProc, NULL, 0);
	if (keyboardHook == NULL) {
		qDebug() << "Failed to install keyboard hook!";
		return;
	}

	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, NULL, 0);
	if (mouseHook == NULL) {
		qDebug() << "Failed to install mouse hook!";
		UnhookWindowsHookEx(keyboardHook);
		keyboardHook = NULL;
		return;
	}

	std::thread([]() {
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		}).detach();
}

void Listener::stop() {
	if (keyboardHook != NULL) {
		UnhookWindowsHookEx(keyboardHook);
		keyboardHook = NULL;
	}
	if (mouseHook != NULL) {
		UnhookWindowsHookEx(mouseHook);
		mouseHook = NULL;
	}
}

void Listener::init()
{
	keyModifier.clear();
	keyModifierFull.clear();
	keyNormal.clear();
	keyNormalFull.clear();
	keyMouse.clear();
	keyMouseFull.clear();

	flagHoldKeyControl = false;
	flagHoldKeyShift = false;
	flagHoldKeyMenu = false;
	flagRejectHook = false;

	POINT lastMousePoint = { -1, -1 };
}

bool Listener::throttling()
{
	auto nowCall = std::chrono::steady_clock::now();

	if ((nowCall - lastCall) >= minInterval) {
		lastCall = nowCall;
		return true;
	}
	return false;
}

bool Listener::addChar(int vkCode)
{
	Variable& variable = Variable::getInstance();
	TypeWord& typeWord = TypeWord::getInstance();

	if ((variable.setVirtualKeyCodeValid.find(vkCode) != variable.setVirtualKeyCodeValid.end())
		//&& ((keyNormalFull.size() == 1) && checkKeyNormal(vkCode))
		&& ((keyModifierFull.size() == 0) || ((keyModifierFull.size() == 1) && checkKeyModifierFull(VK_SHIFT)))
		&& (keyMouseFull.size() == 0))
	{
		BYTE keyboardState[256] = { 0 };
		if (!GetKeyboardState(keyboardState)) {
			flagRejectHook = false;
			return false;
		}

		BYTE stateCapslock = (BYTE)((GetKeyState(VK_CAPITAL) & 0x0001) ? 0x01 : 0x00);
		BYTE stateShift = (BYTE)((GetKeyState(VK_SHIFT) & 0x8000) ? 0x80 : 0x00);
		keyboardState[VK_CAPITAL] = stateCapslock;
		keyboardState[VK_SHIFT] = stateShift;
		keyboardState[VK_LSHIFT] = stateShift;
		keyboardState[VK_RSHIFT] = stateShift;

		HKL keyboardLayout = GetKeyboardLayout(0);

		UINT scanCode = MapVirtualKeyEx(vkCode, MAPVK_VK_TO_VSC, keyboardLayout);
		wchar_t buffer[5] = { 0 };
		int result = ToUnicodeEx(vkCode, scanCode, keyboardState, buffer, 4, 0, keyboardLayout);
		if (result > 0) {
			wchar_t keyChar = buffer[0];
			typeWord.addChar(keyChar);
		}
		flagCheckSpell = true;
		return true;
	}
	else
		return false;
}

bool Listener::removeChar(int vkCode)
{
	if ((vkCode == VK_BACK)
		&& (keyNormalFull.size() == 1)
		&& (keyModifierFull.size() == 0)
		&& (keyMouseFull.size() == 0))
	{
		if (throttling()) {
			TypeWord& typeWord = TypeWord::getInstance();
			typeWord.removeChar();
		}
		else {
			flagRejectHook = true;
		}
		return true;
	}
	else
		return false;
}

bool Listener::moveLeft(int vkCode)
{
	Variable& variable = Variable::getInstance();
	if (!variable.modeUseLeftRight) {
		return false;
	}

	if ((vkCode == VK_LEFT)
		&& (keyNormalFull.size() == 1)
		&& (keyModifierFull.size() == 0)
		&& (keyMouseFull.size() == 0))
	{
		TypeWord& typeWord = TypeWord::getInstance();
		typeWord.moveLeft();
		return true;
	}
	else
		return false;
}

bool Listener::moveRight(int vkCode)
{
	Variable& variable = Variable::getInstance();
	if (!variable.modeUseLeftRight) {
		return false;
	}

	if ((vkCode == VK_RIGHT)
		&& (keyNormalFull.size() == 1)
		&& (keyModifierFull.size() == 0)
		&& (keyMouseFull.size() == 0))
	{
		TypeWord& typeWord = TypeWord::getInstance();
		typeWord.moveRight();
		return true;
	}
	else
		return false;
}

bool Listener::switchLangGlobal(int vkCode)
{
	if (((vkCode == VK_LCONTROL) || (vkCode == VK_LSHIFT))
		&& ((std::chrono::steady_clock::now() - lastCallControl) < minIntervalCombine)
		&& (keyNormalFull.size() == 0)
		&& (keyModifier.size() == 1)
		&& (keyModifierFull.size() == 2)
		&& (keyMouseFull.size() == 0)
		&& checkKeyModifierFull(VK_CONTROL)
		&& checkKeyModifierFull(VK_SHIFT))
	{
		releaseKey(vkCode);
		TypeWord& typeWord = TypeWord::getInstance();
		typeWord.switchLangGlobal();
		typeWord.showLanguage();
		return true;
	}
	else {
		return false;
	}
}

bool Listener::switchLang(int vkCode)
{
	if ((vkCode == VK_LCONTROL)
		&& ((std::chrono::steady_clock::now() - lastCallControl) < minIntervalSingle)
		&& (keyNormalFull.size() == 0)
		&& (keyModifierFull.size() == 1)
		&& (keyMouseFull.size() == 0))
	{
		releaseKey(vkCode);
		TypeWord& typeWord = TypeWord::getInstance();
		Word& word = typeWord.listWord[typeWord.posWord];
		typeWord.switchLang();
		return true;
	}
	else {
		return false;
	}
}

bool Listener::checkSpell(int vkCode)
{
	if ((vkCode == VK_RCONTROL)
		&& ((std::chrono::steady_clock::now() - lastCallControl) < minIntervalSingle)
		&& (keyNormalFull.size() == 0)
		&& (keyModifierFull.size() == 1)
		&& (keyMouseFull.size() == 0))
	{;
		TypeWord& typeWord = TypeWord::getInstance();
		TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
		TaskAI& taskAI = TaskAI::getInstance();

		QString stringBase = QString::fromStdWString(typeWord.createStringDisplayAll());
		if (!flagCheckSpell || stringBase.isEmpty()) {
			return false;
		}

		releaseKey(vkCode);
		taskAI.flagOpenWindow = false;
		taskAI.run(taskAIDatabase.dataCheckSpell, stringBase, true);
		return true;
	}
	else {
		return false;
	}
}

bool Listener::passSwitchWindow(int vkCode)
{
	if ((vkCode == VK_TAB)
		|| (vkCode == VK_LWIN)
		|| (vkCode == VK_RWIN))
	{
		flagRejectHook = false;
		TypeWord& typeWord = TypeWord::getInstance();
		Variable& variable = Variable::getInstance();
		typeWord.reset(true);
		variable.loadSettingsWindow();
		return true;
	}
	else {
		return false;
	}
}

bool Listener::passResetBuffer(int vkCode)
{
	if ((keyNormalFull.size() == 0))
	{
		if ((vkCode == VK_CAPITAL)
			|| (vkCode == VK_NUMLOCK)
			|| (vkCode == VK_LCONTROL)
			|| (vkCode == VK_RCONTROL)
			|| (vkCode == VK_LSHIFT)
			|| (vkCode == VK_RSHIFT)
			|| (vkCode == VK_LMENU)
			|| (vkCode == VK_RMENU))
		{
			flagRejectHook = false;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

bool Listener::resetBuffer(int vkCode)
{
	TypeWord& typeWord = TypeWord::getInstance();
	typeWord.reset();
	flagRejectHook = false;
	return true;
}

bool Listener::checkHotkey(int vkCode)
{
	TaskAI& taskAI = TaskAI::getInstance();
	if (((vkCode == VK_LSHIFT) || (vkCode == VK_RSHIFT))
		&& ((std::chrono::steady_clock::now() - lastCallShift) < minIntervalSingle)
		&& (keyNormalFull.size() == 0)
		&& (keyModifierFull.size() == 1)
		&& (keyMouseFull.size() == 0))
	{
		taskAI.flagOpenWindow = (vkCode == VK_RSHIFT);
		numHotkey = 0;
	}
	return false;
}

bool Listener::checkFunction(int vkCode)
{
	Variable& variable = Variable::getInstance();
	TypeWord& typeWord = TypeWord::getInstance();
	ChangeCase& changeCase = ChangeCase::getInstance();
	ShortcutKeyEditor* shortcutKeyEditor = ShortcutKeyEditor::getInstance();
	TaskAI& taskAI = TaskAI::getInstance();

	if ((vkCode >= 0x70) && (vkCode <= 0x7B) && (numHotkey == 0))
	{
		TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();

		int index = vkCode - 0x70;
		QString keyTaskAI;
		QString stringBase = QString::fromStdWString(typeWord.createStringDisplayAll());

		if (index >= 0 && index < taskAIDatabase.dataTaskAI.size()) {
			auto it = taskAIDatabase.dataTaskAI.constBegin();
			std::advance(it, index);
			keyTaskAI = it.key();
		}
		
		if (keyTaskAI.isEmpty()) {
			numHotkey = -1;
			return false;
		}

		taskAI.run(taskAIDatabase.dataTaskAI[keyTaskAI], stringBase, true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((vkCode == VK_DELETE) && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		TypeWord& typeWord = TypeWord::getInstance();
		typeWord.removeWordBack();
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	QString nameAction = shortcutKeyEditor->getAction(vkCode);

	if ((nameAction == "Chuyển đổi bộ mã") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeCharSet();
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Chuyển đổi kiểu gõ") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeInputMethod();
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Chuyển đổi dạng chữ viết hoa thường") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeCase();
		if (numHotkey != -1) {
			numHotkey = vkCode;
		}
		flagRejectHook = true;
		return true;
	}
	if (!variable.modeClipboard) {
		changeCase.textOrigin.clear();
	}

	if ((nameAction == "Gọi bảng cấu hình ứng dụng") && (numHotkey == 0))
	{
		QTimer::singleShot(0, []() {
			if (ConfigUi::getInstance().isVisible()) {
				ConfigUi::hideWindow();
			}
			else {
				ConfigUi::showWindow();
			}
			});
		typeWord.reset(true);

		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Gọi bảng cấu hình chung") && (numHotkey == 0))
	{
		TrayIcon& trayIcon = TrayIcon::getInstance();
		trayIcon.onGeneralConfig();
		typeWord.reset(true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Gọi bảng tùy chỉnh kiểu gõ tích hợp") && (numHotkey == 0))
	{
		TrayIcon& trayIcon = TrayIcon::getInstance();
		trayIcon.onConfigInput();
		typeWord.reset(true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Gọi bảng trình chỉnh sửa tác vụ AI") && (numHotkey == 0))
	{
		TrayIcon& trayIcon = TrayIcon::getInstance();
		trayIcon.onConfigTaskAI();
		typeWord.reset(true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Gọi bảng trình quản lý phím tắt") && (numHotkey == 0))
	{
		TrayIcon& trayIcon = TrayIcon::getInstance();
		trayIcon.onShortcutKeyEditor();
		typeWord.reset(true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Gọi bảng trình quản lý gõ tắt") && (numHotkey == 0))
	{
		TrayIcon& trayIcon = TrayIcon::getInstance();
		trayIcon.onConfigSnippetEditor();
		typeWord.reset(true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Gọi bảng xóa các tiết lập đã lưu cho từng ứng dụng") && (numHotkey == 0))
	{
		TrayIcon& trayIcon = TrayIcon::getInstance();
		trayIcon.onResetConfigUi();
		typeWord.reset(true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Gọi bảng đóng góp ý kiến") && (numHotkey == 0))
	{
		TrayIcon& trayIcon = TrayIcon::getInstance();
		trayIcon.onFeedback();
		typeWord.reset(true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Gọi bảng hướng dẫn sử dụng") && (numHotkey == 0))
	{
		TrayIcon& trayIcon = TrayIcon::getInstance();
		trayIcon.onHelp();
		typeWord.reset(true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Đóng ứng dụng Sigma") && (numHotkey == 0))
	{
		TrayIcon& trayIcon = TrayIcon::getInstance();
		trayIcon.onQuit();
		typeWord.reset(true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Thực hiện tác vụ AI mặc định") && (numHotkey == 0))
	{
		TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
		QString keyTaskAI = variable.nameTaskAI.toUpper();
		QString stringBase = QString::fromStdWString(typeWord.createStringDisplayAll());
		if (keyTaskAI.isEmpty()) {
			numHotkey = -1;
			return false;
		}
		taskAI.run(taskAIDatabase.dataTaskAI[keyTaskAI], stringBase, true);
		numHotkey = -1;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt sử dụng chế độ tiếng Việt chủ động") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeConfigUi("modeUseDynamic");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt sử dụng clipboard khi gửi phím") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeConfigUi("modeClipboard");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt tương thích với ứng dụng có gợi ý từ") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeConfigUi("modeFixAutoSuggest");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt tự động viết hoa thông minh") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeConfigUi("modeCheckCase");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt cho phép dùng phụ âm đầu \"f\" \"j\" \"w\" \"z\"") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeConfigUi("modeTeenCode");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt dùng phím ← → để điều hướng từng từ") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeConfigUi("modeUseLeftRight");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt khởi động cùng Windows") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeGeneralConfig("modeAutoStart");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt khôi phục từ gốc khi gõ sai chính tả") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeGeneralConfig("modeRestore");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt tự động chuyển từ tiếng Anh đã ghi nhớ") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeGeneralConfig("modeAutoChangeLang");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt xóa toàn bộ dấu khi nhấn phím bỏ dấu") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeGeneralConfig("modeRemoveDiacTone");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt cho phép bỏ dấu xoay vòng") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeGeneralConfig("modeLoopDiacTone");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	if ((nameAction == "Bật | tắt cho phép chèn ký tự bị thiếu") && ((numHotkey == 0) || (numHotkey == vkCode)))
	{
		typeWord.changeGeneralConfig("modeInsertChar");
		numHotkey = vkCode;
		flagRejectHook = true;
		return true;
	}

	numHotkey = -1;
	return false;
}

void Listener::updateKeyPress(int vkCode)
{
	if ((vkCode == VK_LCONTROL) || (vkCode == VK_RCONTROL)) {
		if (!flagHoldKeyControl) {
			flagHoldKeyControl = true;
			lastCallControl = std::chrono::steady_clock::now();
		}
		keyModifierFull.insert(VK_CONTROL);
		keyModifier.insert(VK_CONTROL);
		return;
	}
	if ((vkCode == VK_LSHIFT) || (vkCode == VK_RSHIFT)) {
		if (!flagHoldKeyShift) {
			flagHoldKeyShift = true;
			lastCallShift = std::chrono::steady_clock::now();
		}
		keyModifierFull.insert(VK_SHIFT);
		keyModifier.insert(VK_SHIFT);
		return;
	}
	if ((vkCode == VK_LMENU) || (vkCode == VK_RMENU)) {
		if (!flagHoldKeyMenu) {
			flagHoldKeyMenu = true;
			lastCallMenu = std::chrono::steady_clock::now();
		}
		keyModifierFull.insert(VK_MENU);
		keyModifier.insert(VK_MENU);
		return;
	}
	if ((vkCode == VK_CAPITAL) || (vkCode == VK_NUMLOCK)) {
		keyModifierFull.insert(vkCode);
		keyModifier.insert(vkCode);
		return;
	}
	if ((vkCode == VK_LCONTROL) || (vkCode == VK_RCONTROL)) {
		if (!flagHoldKeyControl) {
			flagHoldKeyControl = true;
			lastCallControl = std::chrono::steady_clock::now();
		}
		keyModifierFull.insert(VK_CONTROL);
		keyModifier.insert(VK_CONTROL);
	}
	keyNormalFull.insert(vkCode);
	keyNormal.insert(vkCode);
}

void Listener::updateKeyRelease(int vkCode)
{
	if ((vkCode == VK_LCONTROL) || (vkCode == VK_RCONTROL)) {
		flagHoldKeyControl = false;
		keyModifier.erase(VK_CONTROL);
	}

	if ((vkCode == VK_LSHIFT) || (vkCode == VK_RSHIFT)) {
		flagHoldKeyShift = false;
		keyModifier.erase(VK_SHIFT);
	}

	if ((vkCode == VK_LMENU) || (vkCode == VK_RMENU)) {
		flagHoldKeyMenu = false;
		keyModifier.erase(VK_MENU);
	}

	if ((vkCode == VK_CAPITAL) || (vkCode == VK_NUMLOCK)) {
		keyModifier.erase(vkCode);
	}

	keyNormal.erase(vkCode);

	if (keyModifier.empty() && keyNormal.empty() && keyMouse.empty()) {
		keyNormalFull.clear();
		keyModifierFull.clear();
		keyMouseFull.clear();
	}
}

void Listener::updateMousePress(WPARAM wParam)
{
	if ((wParam == WM_LBUTTONDOWN)
		|| (wParam == WM_RBUTTONDOWN)
		|| (wParam == WM_MBUTTONDOWN)
		|| (wParam == WM_XBUTTONDOWN))
	{
		keyMouseFull.insert(wParam);
		keyMouse.insert(wParam);
	}
}

void Listener::updateMouseRelease(WPARAM wParam)
{
	if (wParam == WM_LBUTTONUP)
	{
		keyMouse.erase(WM_LBUTTONDOWN);
	}

	if (wParam == WM_RBUTTONUP)
	{
		keyMouse.erase(WM_RBUTTONDOWN);
	}

	if (wParam == WM_MBUTTONUP)
	{
		keyMouse.erase(WM_MBUTTONDOWN);
	}

	if (wParam == WM_XBUTTONUP)
	{
		keyMouse.erase(WM_XBUTTONDOWN);
	}

	if (keyModifier.empty() && keyNormal.empty() && keyMouse.empty()) {
		keyNormalFull.clear();
		keyModifierFull.clear();
		keyMouseFull.clear();
	}
}

void Listener::checkKeyMouse(WPARAM wParam)
{
	TypeWord& typeWord = TypeWord::getInstance();
	Variable& variable = Variable::getInstance();
	ChangeCase& changeCase = ChangeCase::getInstance();
	TaskAI& taskAI = TaskAI::getInstance();

	if ((wParam == WM_MOUSEMOVE)
		|| (wParam == WM_LBUTTONDOWN)
		|| (wParam == WM_LBUTTONUP)
		|| (wParam == WM_LBUTTONDBLCLK)
		|| (wParam == WM_RBUTTONDOWN)
		|| (wParam == WM_RBUTTONUP)
		|| (wParam == WM_RBUTTONDBLCLK)
		|| (wParam == WM_MBUTTONDOWN)
		|| (wParam == WM_MBUTTONUP)
		|| (wParam == WM_MBUTTONDBLCLK)
		|| (wParam == WM_MOUSEWHEEL)
		|| (wParam == WM_XBUTTONDOWN)
		|| (wParam == WM_XBUTTONUP)
		|| (wParam == WM_XBUTTONDBLCLK))
	{
		updateMousePress(wParam);
		updateMouseRelease(wParam);

		if (wParam != WM_MOUSEMOVE) {
			flagCheckSpell = false;
			if (!variable.modeClipboard) {
				changeCase.textOrigin.clear();
			}
			numHotkey = -1;
			taskAI.interrupted = true;

			if (variable.modeUseDynamic) {
				typeWord.reset(true);
			}
			else {
				typeWord.reset();
			}
		}

		if (mouseMovedEnough(wParam))
		{
			if (variable.modeUseDynamic) {
				typeWord.reset(true);
			}
		}

		variable.loadSettingsWindow();
	}
}

bool Listener::mouseMovedEnough(WPARAM wParam)
{
	if (wParam != WM_MOUSEMOVE) {
		return false;
	}

	POINT currentMousePoint;
	if (!GetCursorPos(&currentMousePoint)) {
		return false;
	}

	if (lastMousePoint.x == -1 && lastMousePoint.y == -1) {
		lastMousePoint = currentMousePoint;
		return false;
	}

	int deltaX = currentMousePoint.x - lastMousePoint.x;
	int deltaY = currentMousePoint.y - lastMousePoint.y;
	int distanceSquared = deltaX * deltaX + deltaY * deltaY;

	if (distanceSquared >= minMouseMoveDistanceSquared) {
		lastMousePoint = currentMousePoint;
		return true;
	}

	return false;
}

void Listener::pressKey(int vkCode) {
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vkCode;
	input.ki.dwFlags = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
}

void Listener::releaseKey(int vkCode) {
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vkCode;
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
}

bool Listener::checkKeyModifierFull(int vkKey)
{
	return keyModifierFull.find(vkKey) != keyModifierFull.end();
}

bool Listener::checkKeyNormal(int vkKey)
{
	return keyNormalFull.find(vkKey) != keyNormalFull.end();
}

LRESULT CALLBACK Listener::keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	Clipboard& clipboard = Clipboard::getInstance();
	TaskAI& taskAI = TaskAI::getInstance();

	if ((nCode >= 0) && (!variable.flagSendingKey)) {
		KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
		int vkCode = kbdStruct->vkCode;
		listener.flagRejectHook = false;

		if (listener.keyNormalFull.size() > 0) {
			listener.init();
		}
		if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN)) {
			variable.flagSendingKey = true;
			variable.vkCodeCurrent = vkCode;
			taskAI.interrupted = true;
			listener.updateKeyPress(vkCode);
			(
				listener.checkFunction(vkCode)
				|| listener.addChar(vkCode)
				|| listener.removeChar(vkCode)
				|| listener.moveLeft(vkCode)
				|| listener.moveRight(vkCode)
				|| listener.passSwitchWindow(vkCode)
				|| listener.passResetBuffer(vkCode)
				|| listener.resetBuffer(vkCode));

			variable.flagSendingKey = false;
			variable.vkCodePrevious = vkCode;
		}

		if (wParam == WM_KEYUP) {
			variable.flagSendingKey = true;
			(
				listener.checkHotkey(vkCode)
				|| listener.switchLangGlobal(vkCode)
				|| listener.switchLang(vkCode)
				|| listener.checkSpell(vkCode)
			);

			listener.updateKeyRelease(vkCode);
			variable.flagSendingKey = false;
		}

		if (listener.flagRejectHook) {
			return 1;
		}
		else {
			return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
		}
	}
	return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK Listener::mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		Listener& listener = Listener::getInstance();
		listener.checkKeyMouse(wParam);
	}
	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}
