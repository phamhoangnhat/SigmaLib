#include "TypeWord.h"
#include "ConfigUi.h"
#include "GeneralConfig.h"
#include "Listener.h"
#include "Clipboard.h"
#include "NoticeUi.h"
#include "TaskAI.h"
#include "SnippetEditor.h"
#include "Util.h"
#include <iostream>
#include <windows.h>
#include <cwctype>
#include <QDebug>
#include <QSettings>

TypeWord& TypeWord::getInstance() {
	static TypeWord instance;
	return instance;
}

void TypeWord::switchLangGlobal()
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	ConfigUi& configUi = ConfigUi::getInstance();
	listener.flagRejectHook = true;

	if (listWord.empty()) {
		listWord.push_back(Word());
		posWord = 0;
		flagTypeWord = false;
	}

	variable.flagLangVietGlobal = !variable.flagLangVietGlobal;
	QString strFlagLangVietGlobal = variable.flagLangVietGlobal ? "true" : "false";
	configUi.saveSettings(variable.nameCurrentWindow, "flagLangVietGlobal", strFlagLangVietGlobal);

	Word& word = listWord[posWord];
	word.switchLang(&variable.flagLangVietGlobal);

	std::wstring stringAdd = word.stringAdd;
	int numBackspaceStart = word.numBackspace;

	if (variable.modeClipboard) {
		fixStringDisplayCliboard(numBackspaceStart, stringAdd);
	}
	else {
		fixStringDisplay(numBackspaceStart, stringAdd);
	}
}

void TypeWord::switchLang()
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	listener.flagRejectHook = true;

	if (listWord.empty()) {
		listWord.push_back(Word());
		posWord = 0;
		flagTypeWord = false;
	}

	Word& word = listWord[posWord];
	word.switchLang();

	std::wstring stringAdd = word.stringAdd;
	int numBackspaceStart = word.numBackspace;

	if (variable.modeClipboard) {
		fixStringDisplayCliboard(numBackspaceStart, stringAdd);
	}
	else {
		fixStringDisplay(numBackspaceStart, stringAdd);
	}
}

void TypeWord::addChar(wchar_t character)
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();

	if (listWord.empty()) {
		listWord.push_back(Word());
		posWord = 0;
		flagTypeWord = false;
	}

	std::unordered_set<wchar_t> dataAddCharSpace = Variable::getInstance().dataAddCharSpace;
	bool flagCharKeyValid = (dataAddCharSpace.find(character) != dataAddCharSpace.end());
	if (flagTypeWord) {
		if (!flagCharKeyValid) {
			flagTypeWord = false;
			addWord(posWord + 1);
		}
	}
	else {
		if (flagCharKeyValid) {
			flagTypeWord = true;
		}
	}

	Word& wordAddChar = listWord[posWord];
	wordAddChar.addChar(character);

	Word& wordDisplay = listWord[posWord];
	std::wstring stringAdd = wordDisplay.stringAdd;
	int numBackspaceStart = wordDisplay.numBackspace;
	if ((numBackspaceStart == 0) && (stringAdd.size() == 1) && (character == stringAdd.back())) {
		listener.flagRejectHook = false;
	}
	else {
		listener.flagRejectHook = true;
		if (variable.modeClipboard) {
			fixStringDisplayCliboard(numBackspaceStart, stringAdd);
		}
		else {
			fixStringDisplay(numBackspaceStart, stringAdd);
		}
	}
}

void TypeWord::addWordBack()
{
	if (listWord.empty()) {
		listWord.push_back(Word());
		posWord = 0;
		flagTypeWord = false;
	}

	Word& word = listWord[posWord];
	if (!word.listCharSpaceNew.empty() || !word.listCharDisplayNew.empty()) {
		addWord(posWord + 1);
	}

	Listener& listener = Listener::getInstance();
	listener.flagRejectHook = false;
}

void TypeWord::removeChar()
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();

	if (listWord.empty()) {
		reset();
		listener.flagRejectHook = false;
		return;
	}

	Word& word = listWord[posWord];
	word.removeChar();

	int numBackspaceStart = word.numBackspace;
	std::wstring stringAdd = word.stringAdd;

	if (variable.modeClipboard) {
		listener.flagRejectHook = true;
		fixStringDisplayCliboard(numBackspaceStart, stringAdd);
	}
	else {
		listener.flagRejectHook = true;
		fixStringDisplay(numBackspaceStart, stringAdd);
	}

	if (word.listCharDisplayNew.empty()) {
		if (word.listCharSpaceNew.empty()) {
			removeWord();
		}
		else {
			flagTypeWord = false;
		}
	}
	else {
		flagTypeWord = true;
	}
}

void TypeWord::removeWordBack()
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	listener.flagRejectHook = false;

	if (posWord >= 0) {
		Word& word = listWord[posWord];
		int numMove = static_cast<int>(word.listCharSpaceNew.size() + word.listCharDisplayNew.size());

		if (numMove == 0) {
			reset();
		}
		else {
			removeWord();
			std::vector<INPUT> inputs;

			//numMove -= 1;
			if (variable.modeFixAutoSuggest) {
				addInput(L'·', inputs);
				for (int i = 0; i <= numMove; i++) {
					addInput(VK_BACK, inputs);
				}
			}
			else {
				for (int i = 0; i < numMove; i++) {
					addInput(VK_BACK, inputs);
				}
			}
			SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
		}
	}
}

void TypeWord::moveLeft() {
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	listener.flagRejectHook = false;

	if (posWord >= 0) {
		Word& word = listWord[posWord];
		int numMove = static_cast<int>(word.listCharSpaceNew.size() + word.listCharDisplayNew.size());

		if (posWord == 0) {
			reset();
		}
		else {
			posWord -= 1;

			std::vector<INPUT> inputs;
			if (variable.modeFixAutoSuggest) {
				addInput(L'·', inputs);
				addInput(VK_BACK, inputs);
			}

			numMove -= 1;
			for (int i = 0; i < numMove; i++) {
				addInput(VK_LEFT, inputs);
			}
			SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
		}
	}
}

void TypeWord::moveRight()
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	listener.flagRejectHook = false;

	if (posWord < (listWord.size() - 1)) {
		posWord += 1;

		Word& word = listWord[posWord];
		int numMove = static_cast<int>(word.listCharSpaceNew.size() + word.listCharDisplayNew.size());


		std::vector<INPUT> inputs;
		if (variable.modeFixAutoSuggest) {
			addInput(L'·', inputs);
			addInput(VK_BACK, inputs);
		}

		numMove -= 1;
		for (int i = 0; i < numMove; i++) {
			addInput(VK_RIGHT, inputs);
		}
		SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
	}
	else {
		reset();
	}
}

void TypeWord::addWord(int posWordTemp)
{
	Variable& variable = Variable::getInstance();

	Word& wordPrevious = listWord[posWordTemp - 1];
	updateDataChangeLang(wordPrevious);

	//if (posWordTemp > 1) {
	//	Word& wordPrev = listWord[posWordTemp - 2];
	//	Variable& variable = Variable::getInstance();
	//	if (variable.modeRestore && wordPrev.flagLangViet && !(wordPrev.listCharVietInvalid.empty())) {
	//		wordPrev.flagLangViet = false;
	//	}
	//}

	wordPrevious.keyPassDiac.clear();
	wordPrevious.keyPassEnd.clear();
	listWord.insert(listWord.begin() + posWordTemp, Word());
	posWord = posWordTemp;
	flagTypeWord = false;
}

void TypeWord::removeWord()
{
	if (posWord >= 0) {
		listWord.erase(listWord.begin() + posWord);
		if (posWord == 0) {
			reset();
			return;
		}
		posWord -= 1;
		Word& word = listWord[posWord];
		if (word.listCharDisplayNew.empty())
			flagTypeWord = false;
		else
			flagTypeWord = true;
	}
}

void TypeWord::changeCase()
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	listener.flagRejectHook = true;

	if (posWord >= 0) {
		Word& word = listWord[posWord];
		word.changeCase();

		int numBackspaceStart = word.numBackspace;
		std::wstring stringAdd = word.stringAdd;

		if (variable.modeClipboard) {
			fixStringDisplayCliboard(numBackspaceStart, stringAdd);
		}
		else {
			fixStringDisplay(numBackspaceStart, stringAdd);
		}
	}
}

void TypeWord::changeCharSet()
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	ConfigUi& configUi = ConfigUi::getInstance();
	if (configUi.isVisible() || (variable.nameCurrentWindow == "Sigma")) {
		return;
	}

	listener.flagRejectHook = true;

	auto it = variable.mapCharacterSetBase.find(variable.characterSet);
	if (it != variable.mapCharacterSetBase.end()) {
		++it;
		if (it == variable.mapCharacterSetBase.end()) {
			it = variable.mapCharacterSetBase.begin();
		}
		variable.characterSet = it->first;
	}

	variable.update();
	configUi.saveSettings(variable.nameCurrentWindow, "characterSet", QString::fromStdWString(variable.characterSet));

	if (posWord >= 0) {
		Word& word = listWord[posWord];
		word.changeCharSet();

		int numBackspaceStart = word.numBackspace;
		std::wstring stringAdd = word.stringAdd;

		if (variable.modeClipboard) {
			fixStringDisplayCliboard(numBackspaceStart, stringAdd);
		}
		else {
			fixStringDisplay(numBackspaceStart, stringAdd);
		}
	}
	showCharSet();
}

void TypeWord::changeInputMethod()
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	GeneralConfig* generalConfig = GeneralConfig::getInstance();
	if (generalConfig->isVisible()) {
		return;
	}

	listener.flagRejectHook = true;

	auto it = variable.mapInputMethodBase.find(variable.inputMethod);
	if (it != variable.mapInputMethodBase.end()) {
		++it;
		if (it == variable.mapInputMethodBase.end()) {
			it = variable.mapInputMethodBase.begin();
		}
		variable.inputMethod = it->first;
	}
	else {
		variable.inputMethod = variable.INPUTMETHOD;
	}

	variable.update();
	QSettings settings(variable.appName, "Config");
	settings.setValue("inputMethod", QString::fromStdWString(variable.inputMethod));
	showInputMethod();
}

void TypeWord::changeConfigUi(QString nameMode)
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	ConfigUi& configUi = ConfigUi::getInstance();
	if (configUi.isVisible() || (variable.nameCurrentWindow == "Sigma")) {
		return;
	}

	listener.flagRejectHook = false;
	bool valueMode;

	if (nameMode == "modeUseDynamic") {
		variable.modeUseDynamic = !variable.modeUseDynamic;
		valueMode = variable.modeUseDynamic;
	}

	if (nameMode == "modeClipboard") {
		variable.modeClipboard = !variable.modeClipboard;
		valueMode = variable.modeClipboard;
	}

	if (nameMode == "modeFixAutoSuggest") {
		variable.modeFixAutoSuggest = !variable.modeFixAutoSuggest;
		valueMode = variable.modeFixAutoSuggest;
	}

	if (nameMode == "modeCheckCase") {
		variable.modeCheckCase = !variable.modeCheckCase;
		valueMode = variable.modeCheckCase;
	}

	if (nameMode == "modeTeenCode") {
		variable.modeTeenCode = !variable.modeTeenCode;
		valueMode = variable.modeTeenCode;
	}

	if (nameMode == "modeInsertChar") {
		variable.modeInsertChar = !variable.modeInsertChar;
		valueMode = variable.modeInsertChar;
	}

	if (nameMode == "modeUseLeftRight") {
		variable.modeUseLeftRight = !variable.modeUseLeftRight;
		valueMode = variable.modeUseLeftRight;
	}

	QSettings settings(variable.appName, "ConfigUi");
	settings.beginGroup(variable.nameCurrentWindow);
	settings.setValue(nameMode, valueMode);
	settings.endGroup();
	variable.update();

	showChangeConfig(nameMode);
}

void TypeWord::changeGeneralConfig(QString nameMode)
{
	Variable& variable = Variable::getInstance();
	Listener& listener = Listener::getInstance();
	GeneralConfig* generalConfig = GeneralConfig::getInstance();
	if (generalConfig->isVisible()) {
		return;
	}

	listener.flagRejectHook = false;
	bool valueMode;

	if (nameMode == "modeAutoStart") {
		variable.modeAutoStart = !variable.modeAutoStart;
		valueMode = variable.modeAutoStart;
		setAutoStartApp(valueMode);
	}

	if (nameMode == "modeUseSnippet") {
		variable.modeUseSnippet = !variable.modeUseSnippet;
		valueMode = variable.modeUseSnippet;
	}

	if (nameMode == "modeLoopDiacTone"){
		variable.modeLoopDiacTone = !variable.modeLoopDiacTone;
		valueMode = variable.modeLoopDiacTone;
	}

	if (nameMode == "modeAutoAddVowel") {
		variable.modeAutoAddVowel = !variable.modeAutoAddVowel;
		valueMode = variable.modeAutoAddVowel;
	}

	if (nameMode == "modeShortcutLast") {
		variable.modeShortcutLast = !variable.modeShortcutLast;
		valueMode = variable.modeShortcutLast;
	}

	if (nameMode == "modeAutoChangeLang") {
		variable.modeAutoChangeLang = !variable.modeAutoChangeLang;
		valueMode = variable.modeAutoChangeLang;
	}

	QSettings settings(variable.appName, "Config");
	settings.setValue(nameMode, valueMode);
	variable.update();

	showChangeConfig(nameMode);
}

void TypeWord::fixStringDisplayCliboard(int numBackspaceStart, std::wstring stringAdd)
{
	Clipboard& clipboard = Clipboard::getInstance();
	Variable& variable = Variable::getInstance();

	if ((numBackspaceStart == 0) && (stringAdd.size() == 0)) {
		return;
	}

	std::vector<INPUT> inputs;
	if (variable.modeFixAutoSuggest) {
		addInput(L'·', inputs);
		for (size_t i = 0; i <= numBackspaceStart; ++i) {
			addInput(VK_BACK, inputs);
		}
	}
	else {
		for (size_t i = 0; i < numBackspaceStart; ++i) {
			addInput(VK_BACK, inputs);
		}
	}

	SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
	inputs.clear();
	clipboard.sendUnicodeText(stringAdd);

	if (stringSnippet != L"") {
		reset();
	}
}

void TypeWord::fixStringDisplay(int numBackspaceStart, std::wstring stringAdd)
{
	Variable& variable = Variable::getInstance();

	if ((numBackspaceStart == 0) && (stringAdd.size() == 0)) {
		return;
	}

	std::vector<INPUT> inputs;
	if (variable.modeFixAutoSuggest) {
		addInput(L'·', inputs);
		for (size_t i = 0; i <= numBackspaceStart; ++i) {
			addInput(VK_BACK, inputs);
		}
	}
	else {
		for (size_t i = 0; i < numBackspaceStart; ++i) {
			addInput(VK_BACK, inputs);
		}
	}

	for (wchar_t character : stringAdd) {
		addInput(character, inputs);
	}

	SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));

	if (stringSnippet != L"") {
		reset();
	}
}

void TypeWord::addInput(int vkCode, std::vector<INPUT>& inputs)
{
	INPUT input;
	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vkCode;
	input.ki.dwFlags = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	inputs.push_back(input);

	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vkCode;
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	inputs.push_back(input);
}

void TypeWord::addInput(wchar_t character, std::vector<INPUT>& inputs)
{
	INPUT input;
	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = character;
	input.ki.dwFlags = KEYEVENTF_UNICODE;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	inputs.push_back(input);

	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = character;
	input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	inputs.push_back(input);
}

void TypeWord::showLanguage()
{
	Variable& variable = Variable::getInstance();
	NoticeUi& notice = NoticeUi::getInstance();

	bool flagLangViet = false;
	if (posWord >= 0) {
		Word& word = listWord[posWord];
		flagLangViet = word.flagLangViet;
	}
	else {
		flagLangViet = variable.flagLangVietGlobal;
	}

	notice.iconWidth = 40;
	notice.currentFontSize = 15;
	if (variable.flagLangVietGlobal) {
		if (flagLangViet) {
			notice.displayText = QStringLiteral("V");
			if (variable.modeUseDynamic) {
				notice.backgroundColor = QColor(165, 87, 214);
			}
			else {
				notice.backgroundColor = QColor(149, 187, 53);
			}
		}
		else {
			notice.displayText = QStringLiteral("E");
			notice.backgroundColor = QColor(102, 153, 204);
		}
	}
	else {
		if (flagLangViet) {
			notice.displayText = QStringLiteral("V");
			notice.backgroundColor = QColor(102, 153, 204);
		}
		else {
			notice.displayText = QStringLiteral("E");
			notice.backgroundColor = QColor(241, 121, 121);
		}
	}
	NoticeUi::updateWindow();
}

void TypeWord::showCharSet()
{
	Variable& variable = Variable::getInstance();
	NoticeUi& notice = NoticeUi::getInstance();

	notice.currentFontSize = 10;
	notice.backgroundColor = QColor(34, 103, 107);
	notice.displayText = QString::fromStdWString(L"Bộ mã: " + variable.characterSet);

	QFont font;
	font.setPointSize(notice.currentFontSize);
	QFontMetrics fm(font);
	notice.iconWidth = fm.horizontalAdvance(notice.displayText) + 30;

	NoticeUi::updateWindow();
}

void TypeWord::showInputMethod()
{
	Variable& variable = Variable::getInstance();
	NoticeUi& notice = NoticeUi::getInstance();

	notice.currentFontSize = 10;
	notice.backgroundColor = QColor(34, 103, 107);
	notice.displayText = QString::fromStdWString(L"Kiểu gõ: " + variable.inputMethod);

	QFont font;
	font.setPointSize(notice.currentFontSize);
	QFontMetrics fm(font);
	notice.iconWidth = fm.horizontalAdvance(notice.displayText) + 30;

	NoticeUi::updateWindow();
}

void TypeWord::showChangeConfig(QString nameMode)
{
	Variable& variable = Variable::getInstance();
	NoticeUi& notice = NoticeUi::getInstance();
	bool valueMode;
	QString displayText;

	if (nameMode == "modeUseDynamic") {
		valueMode = variable.modeUseDynamic;
		displayText = "Sử dụng chế độ tiếng Việt chủ động";
	}

	if (nameMode == "modeClipboard") {
		valueMode = variable.modeClipboard;
		displayText = "Sử dụng clipboard khi gửi phím";
	}

	if (nameMode == "modeFixAutoSuggest") {
		valueMode = variable.modeFixAutoSuggest;
		displayText = "Tương thích với ứng dụng có gợi ý từ";
	}

	if (nameMode == "modeCheckCase") {
		valueMode = variable.modeCheckCase;
		displayText = "Tự động viết hoa thông minh";
	}

	if (nameMode == "modeTeenCode") {
		valueMode = variable.modeTeenCode;
		displayText = "Cho phép gõ ký tự Teencode";
	}

	if (nameMode == "modeInsertChar") {
		valueMode = variable.modeInsertChar;
		displayText = "Cho phép chèn ký tự bị thiếu";
	}

	if (nameMode == "modeUseLeftRight") {
		valueMode = variable.modeUseLeftRight;
		displayText = "Dùng phím ← → để điều hướng từng từ";
	}

	if (nameMode == "modeAutoStart") {
		valueMode = variable.modeAutoStart;
		displayText = "Khởi động cùng Windows";
	}

	if (nameMode == "modeUseSnippet") {
		valueMode = variable.modeUseSnippet;
		displayText = "Cho phép gõ tắt";
	}

	if (nameMode == "modeLoopDiacTone") {
		valueMode = variable.modeLoopDiacTone;
		displayText = "Cho phép bỏ dấu xoay vòng";
	}

	if (nameMode == "modeAutoAddVowel") {
		valueMode = variable.modeAutoAddVowel;
		displayText = "Tự động thêm dấu mũ các vần \"iê\" \"uê\" \"uô\" \"uyê\" \"yê\"";
	}

	if( nameMode == "modeShortcutLast") {
		valueMode = variable.modeShortcutLast;
		displayText = "Cho phép gõ tắt các âm cuối \"ch\" \"ng\" \"nh\"";
	}

	if (nameMode == "modeAutoChangeLang") {
		valueMode = variable.modeAutoChangeLang;
		displayText = "Tự động chuyển từ tiếng Anh đã ghi nhớ";
	}

	if (nameMode == "modeRestore") {
		valueMode = !variable.modeRestore;
		displayText = "Cho phép gõ các từ tiếng Việt dính liền";
	}

	if(valueMode){
		notice.displayText = "Bật: " + displayText;
	}
	else {
		notice.displayText = "Tắt: " + displayText;
	}

	notice.currentFontSize = 10;
	notice.backgroundColor = QColor(34, 103, 107);

	QFont font;
	font.setPointSize(notice.currentFontSize);
	QFontMetrics fm(font);
	notice.iconWidth = fm.horizontalAdvance(notice.displayText) + 100;

	NoticeUi::updateWindow();
}

void TypeWord::calStringSnippet()
{
	Variable& variable = Variable::getInstance();
	SnippetEditor* snippetEditor = SnippetEditor::getInstance();

	int count;
	int length;
	int lengthMax;
	std::wstring stringTotal = L"";
	std::wstring stringSub;
	std::wstring stringKey;
	std::wstring stringKeyTemp;
	Word word;
	int posWordTemp = posWord;

	lengthMax = snippetEditor->maxCount + 2;
	while ((posWordTemp >= 0) && (stringTotal.size() < lengthMax))
	{
		word = listWord[posWordTemp];
		stringSub = listCharToString(word.listCharSpaceNew) + listCharToString(word.listCharDisplayNew);
		stringTotal = stringSub + stringTotal;
		posWordTemp--;
	}
	length = static_cast<int>(stringTotal.size());

	stringSnippet = L"";
	for (auto it = snippetEditor->setCountString.rbegin(); it != snippetEditor->setCountString.rend(); ++it) {
		count = *it;
		stringKeyTemp = L"";
		if (length >= count) {
			stringKeyTemp = stringTotal.substr(length - count);

			stringKey = toLowerCase(stringKeyTemp);
			if ((count > 0) && (std::iswupper(stringKeyTemp[0]))) {
				stringKey[0] = std::toupper(stringKey[0]);
			}
			if ((count > 1) && (std::iswupper(stringKeyTemp[1]))) {
				stringKey[0] = std::toupper(stringKey[0]);
				stringKey[1] = std::toupper(stringKey[1]);
			}

			if (snippetEditor->mapSnippetString.count(stringKey)) {
				stringSnippet = snippetEditor->mapSnippetString[stringKey];
				numKeySnippet = static_cast<int>(stringKey.size());
				continue;
			}
		}
		else {
			continue;
		}
	}

	if ((stringSnippet == L"") && !stringTotal.empty()) {
		wchar_t charSpace1 = stringTotal.back();
		if (variable.setCharSpaceSnippet.count(charSpace1)) {
			stringTotal = stringTotal.substr(0, length - 1);
			length = static_cast<int>(stringTotal.size());
			for (auto it = snippetEditor->setCountWords.rbegin(); it != snippetEditor->setCountWords.rend(); ++it) {
				count = *it;
				stringKeyTemp = L"";
				if (length == count) {
					stringKeyTemp = stringTotal.substr(length - count);
				}
				else if (length > count) {
					wchar_t charSpace2 = stringTotal[length - count - 1];
					if (variable.setCharSpaceSnippet.count(charSpace2)) {
						stringKeyTemp = stringTotal.substr(length - count);
					}
				}
				else {
					continue;
				}

				stringKey = toLowerCase(stringKeyTemp);
				if ((count > 0) && (std::iswupper(stringKeyTemp[0]))) {
					stringKey[0] = std::toupper(stringKey[0]);
				}
				if ((count > 1) && (std::iswupper(stringKeyTemp[1]))) {
					stringKey[0] = std::toupper(stringKey[0]);
					stringKey[1] = std::toupper(stringKey[1]);
				}

				if (snippetEditor->mapSnippetWords.count(stringKey)) {
					stringSnippet = snippetEditor->mapSnippetWords[stringKey];
					stringSnippet += charSpace1;
					numKeySnippet = static_cast<int>(stringKey.size()) + 1;
					continue;
				}
			}
		}
	}

	if (stringSnippet != L"") {
		int numCharMax = 1;
		if (variable.characterSet == L"VNI Windows") {
			stringSnippet = convertCharset(stringSnippet, variable.mapUniToVni, numCharMax);
		}
		if (variable.characterSet == L"TCVN3 (ABC)") {
			stringSnippet = convertCharset(stringSnippet, variable.mapUniToAbc, numCharMax);
		}
	}
}

void TypeWord::updateDataChangeLang(Word& word)
{
	Variable& variable = Variable::getInstance();
	std::wstring stringWordEng = listCharToString(word.listCharOrigin);
	if (word.numSwitchLang) {
		QString qWord = QString::fromStdWString(stringWordEng).toLower();
		std::wstring lowerWord(reinterpret_cast<const wchar_t*>(qWord.utf16()), qWord.length());
		if (word.flagLangViet) {
			std::wstring stringTemp = L"";
			for (wchar_t character : lowerWord) {
				stringTemp += character;
				variable.dataAutoChangeLang.erase(stringTemp);
			}
		}
		else {
			if (lowerWord.size() > 1) {
				variable.dataAutoChangeLang.insert(lowerWord);
			}
		}
	}
}

void TypeWord::reset(bool flagCheckChangeLangEng)
{
	Variable& variable = Variable::getInstance();
	Clipboard& clipboard = Clipboard::getInstance();
	if (flagCheckChangeLangEng && variable.modeUseDynamic) {
		variable.flagLangVietGlobal = false;
		QString strFlagLangVietGlobal = variable.flagLangVietGlobal ? "true" : "false";
		ConfigUi::getInstance().saveSettings(variable.nameCurrentWindow, "flagLangVietGlobal", strFlagLangVietGlobal);
	}
	if (!listWord.empty()) {
		updateDataChangeLang(listWord.back());
		clipboard.setBaseClipboard();
	}

	listWord.clear();
	flagTypeWord = false;
	posWord = -1;
}

TypeWord::TypeWord() {
	flagTypeWord = false;
	posWord = -1;
	colorUI = RGB(150, 150, 150);
	stringSnippet = L"";
	numKeySnippet = 0;
	colorUI = QColor(150, 150, 150);
}

TypeWord::~TypeWord() {}


