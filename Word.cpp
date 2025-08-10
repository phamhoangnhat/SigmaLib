﻿// Word.cpp

#include "Word.h"
#include "SnippetEditor.h"
#include "TypeWord.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <QDebug>

Word::Word() {
	Variable& variable = Variable::getInstance();
	flagLangViet = variable.flagLangVietGlobal;
	numSwitchLang = 0;
	flagAddCharSpace = true;
	flagAddCharInvalid = false;
	stateAdd = 0;
	numCharD = 0;
	stateD = variable.listStateBase[13];
	stateTone = variable.listStateBase[0];
	stateDiac = variable.listStateBase[6];
	charQuickVowel = L'\0';
	numBackspace = 0;
	stringAdd = L"";
	modeChangeCase = 0;
	numChangeCaseDisplay = 0;
	flagChangeLangEng = false;
}

bool Word::addChar(wchar_t character, bool flagMustAdd) {
	Variable& variable = Variable::getInstance();
	TypeWord& typeWord = TypeWord::getInstance();

	bool result = false;

	if (charQuickVowel != std::tolower(character)) {
		charQuickVowel = L'\0';
	}

	std::unordered_set<wchar_t>& dataAddCharSpace = variable.dataAddCharSpace;
	bool flagCharSpace = (dataAddCharSpace.find(character) == dataAddCharSpace.end());

	if (flagAddCharSpace && flagCharSpace) {
		result = addCharSpace(character);
	}
	else {
		flagAddCharSpace = false;
	}

	if (!(numSwitchLang && !flagLangViet)) {
		if (!flagAddCharSpace && !flagCharSpace) {
			if (!flagAddCharInvalid) {
				if (!result && (stateAdd == 0)) {
					result = (
						addCharStart(character, false) ||
						addKeyD(character)
						);
					if (!result) {
						stateAdd += 1;
					}
				}

				if (!result && (stateAdd == 1)) {
					result = (
						addCharMiddle(character, false) ||
						addKeyRemoveDiacToneD(character) ||
						addKeyTone(character) ||
						addKeyDiac(character)
						);
					if (!result) {
						stateAdd += 1;
					}
				}

				if (!result && (stateAdd == 2)) {
					result = addCharEnd(character, false);
					if (!result) {
						stateAdd += 1;
					}
				}

				if (!result && (stateAdd == 3)) {
					if (variable.modeInsertChar) {
						result = (
							addKeyRemoveDiacToneD(character) ||
							addKeyTone(character) ||
							addKeyDiac(character) ||
							addCharEnd(character, true) ||
							addCharMiddle(character, true) ||
							addCharStart(character, true) ||
							addKeyD(character)
							);
					}
					else {
						result = (
							addKeyRemoveDiacToneD(character) ||
							addKeyTone(character) ||
							addKeyDiac(character) ||
							addCharEnd(character, true) ||
							addKeyD(character)
							);
					}
				}

				if (result && !flagAddCharInvalid) {
					listCharViet.push_back(std::wstring(1, character));
				}

				if (flagChangeLangEng) {
					flagChangeLangEng = false;
					result = true;
				}
			}
		}

		if (!result) {

			std::unordered_set<wchar_t>& dataCheckModeRestore = variable.dataCheckModeRestore;
			bool flagRestore = (dataCheckModeRestore.find(character) != dataCheckModeRestore.end());
			if (variable.modeRestore && flagRestore) {
				result = addCharInvalid(character);
			}
			else {
				if (flagMustAdd) {
					result = addCharInvalid(character);
				}
				else {
					typeWord.addWord(typeWord.posWord + 1);
					Word& word = typeWord.listWord[typeWord.posWord];
					result = word.addChar(character, true);
					return result;
				}
			}
		}
	}

	if (!(numSwitchLang && flagLangViet)) {
		if (!flagAddCharSpace) {
			listCharOrigin.push_back(std::wstring(1, character));
		}
	}

	bool flagChangeStateAdd = checkCharGI();
	if ((stateAdd == 0) && flagChangeStateAdd) {
		stateAdd++;
	}

	checkAutoChangeLang();
	updateListCharDisplay();
	calStepChangeDisplay();
	return result;
}

bool Word::addCharTemplate(wchar_t character, bool flagAddBack, std::vector<std::wstring>& listChar, bool (Word::* callBackCheckChar)(wchar_t, std::vector<std::wstring>&, std::vector<wchar_t>*), std::vector<wchar_t>* listParam) {
	bool result = false;

	if (flagAddBack) {
		size_t numMax = listChar.size();
		for (size_t num = 0; num <= numMax; num++) {
			std::vector<std::wstring> listCharTemp = listChar;
			listCharTemp.insert(listCharTemp.begin() + num, std::wstring(1, character));
			result = (this->*callBackCheckChar)(character, listCharTemp, listParam);
			if (result) {
				return true;
			}
		}
	}
	else {
		std::vector<std::wstring> listCharTemp = listChar;
		listCharTemp.push_back(std::wstring(1, character));
		result = (this->*callBackCheckChar)(character, listCharTemp, listParam);
		if (result) {
			return true;
		}
	}
	return false;
}

bool Word::addCharStart(wchar_t character, bool flagAddBack) {
	std::wstring stringVietStartUpper = toUpperCase(listCharToString(listCharVietStart));
	std::wstring stringVowelUpper = toUpperCase(listCharToString(listCharVowel));
	if ((stringVietStartUpper == L"G") && (towupper(character) == L'I') && (stringVowelUpper.size() > 0) && (stringVowelUpper[0] == L'I')) {
		return false;
	}

	return addCharTemplate(character, flagAddBack, listCharVietStart, &Word::checkCharOnsetStart);
}

bool Word::addCharMiddle(wchar_t character, bool flagAddBack) {
	Variable& variable = Variable::getInstance();

	bool result = false;

	std::wstring stringVietStartUpper = toUpperCase(listCharToString(listCharVietStart));
	if ((stringVietStartUpper == L"QU") && (towupper(character) == L'U')) {
		return false;
	}

	if ((stringVietStartUpper == L"GI") && (towupper(character) == L'I')) {
		return false;
	}

	checkCharGI(character);
	result = addCharTemplate(character, flagAddBack, listCharVowel, &Word::checkCharVowel);
	if (result) {
		if (listCharVietMiddle.back() != listCharVowel.back()) {
			keyPassDiac.clear();
			for (auto character : variable.dataAddCharMiddle1[stateDiac]) {
				keyPassDiac.insert(character);
			}
		}
		checkEndInvalid();
		return true;
	}

	if (variable.dataAddCharMiddle2.find(character) != variable.dataAddCharMiddle2.end()) {
		wchar_t stateDiacTemp = variable.dataAddCharMiddle2[character].first;
		wchar_t charTemp = variable.dataAddCharMiddle2[character].second;

		std::vector<wchar_t> listParam = { stateDiacTemp };
		result = addCharTemplate(charTemp, flagAddBack, listCharVowel, &Word::checkCharVowel, &listParam);
		if (result) {
			charQuickVowel = std::tolower(character);
			return true;
		}
	}

	return false;
}

bool Word::addCharEnd(wchar_t character, bool flagAddBack) {
	bool result = false;

	std::wstring stringVietStartUpper = toUpperCase(listCharToString(listCharVietStart));
	if ((stringVietStartUpper == L"QU") && (towupper(character) == L'U') && listCharVietMiddle.empty()) {
		return false;
	}

	if ((stringVietStartUpper == L"GI") && (towupper(character) == L'I') && listCharVietMiddle.empty()) {
		return false;
	}

	result = addCharTemplate(character, flagAddBack, listCharVietEnd, &Word::checkCharEnd);
	if (result) {
		checkPosTone();
		return true;
	}

	return false;
}

bool Word::addCharSpace(wchar_t character) {
	listCharSpaceNew.push_back(std::wstring(1, character));
	return true;
}

bool Word::addCharInvalid(wchar_t character) {
	flagAddCharInvalid = true;
	listCharVietInvalid.push_back(std::wstring(1, character));
	listCharVietInvalidTemp = listCharVietInvalid;
	return true;
}

bool Word::addKeyTemplate(wchar_t character, wchar_t state, std::map<wchar_t, std::vector<wchar_t>>& dataAddChar, bool (Word::* callBackCheckKey)(wchar_t, wchar_t)) {
	Variable& variable = Variable::getInstance();

	int numCharDTemp = numCharD;
	wchar_t stateDTemp = stateD;
	wchar_t stateToneTemp = stateTone;
	wchar_t stateDiacTemp = stateDiac;
	std::vector<std::wstring> listCharVietStartTemp = listCharVietStart;
	std::vector<std::wstring> listCharVietMiddleTemp = listCharVietMiddle;

	bool result = false;
	wchar_t key = towupper(character);
	if (dataAddChar.find(key) != dataAddChar.end()) {
		std::vector<wchar_t> listState = dataAddChar[key];

		int length = static_cast<int>(listState.size());
		wchar_t stateOld = state;

		if (find(listState.begin(), listState.end(), stateOld) == listState.end()) {
			stateOld = listState[0];
		}

		int pos = static_cast<int>(find(listState.begin(), listState.end(), stateOld) - listState.begin());
		pos++;
		int count = 0;
		while (count < length) {
			state = listState[pos % length];
			if (stateOld != state) {
				result = (this->*callBackCheckKey)(character, state);
			}
			if (result) {
				if (flagLangViet && (!variable.modeLoopDiacTone))
				{
					if (pos >= length) {
						if (variable.modeRemoveDiacTone) {
							checkKeyTone(L'\0', variable.listStateBase[0]);
							checkKeyDiac(L'\0', variable.listStateBase[6]);
							checkKeyD(L'\0', variable.listStateBase[13]);
						}
						else {
							if ((state >= variable.listStateBase[0]) && (state <= variable.listStateBase[5])) {
								checkKeyTone(L'\0', variable.listStateBase[0]);
							}
							if ((state >= variable.listStateBase[6]) && (state <= variable.listStateBase[12])) {
								checkKeyDiac(L'\0', variable.listStateBase[6]);
							}
							if ((state >= variable.listStateBase[13]) && (state <= variable.listStateBase[14])) {
								checkKeyD(L'\0', variable.listStateBase[13]);
							}

						}
						listCharViet.clear();
						listCharViet.insert(listCharViet.end(), listCharVietStart.begin(), listCharVietStart.end());
						if ((charQuickVowel != std::tolower(character)) || (!listCharVietEnd.empty())) {
							listCharViet.insert(listCharViet.end(), listCharVietMiddle.begin(), listCharVietMiddle.end());
						}
						listCharViet.insert(listCharViet.end(), listCharVietEnd.begin(), listCharVietEnd.end());
						listCharVietInvalid.clear();
						addCharInvalid(character);

						//numCharD = numCharDTemp;
						//stateD = stateDTemp;
						//stateTone = stateToneTemp;
						//stateDiac = stateDiacTemp;
						//listCharVietStart = listCharVietStartTemp;
						//listCharVietMiddle = listCharVietMiddleTemp;
						//flagChangeLangEng = true;
					}
				}
				return result;
			}
			pos++;
			count++;
		}
	}
	return result;
}

bool Word::addKeyRemoveDiacToneD(wchar_t character)
{
	Variable& variable = Variable::getInstance();
	bool result = false;

	if (checkKeyNumpad()) {
		return false;
	}

	wchar_t key = towupper(character);
	if (variable.dataAddKeyRemoveDiacToneD.count(key)) {
		if (stateTone != variable.listStateBase[0]) {
			checkKeyTone(L'\0', variable.listStateBase[0]);
			result = true;
		}

		if (variable.modeRemoveDiacTone) {
			if (stateDiac != variable.listStateBase[6]) {
				checkKeyDiac(L'\0', variable.listStateBase[6]);
				result = true;
			}

			if (stateD != variable.listStateBase[13]) {
				checkKeyD(L'\0', variable.listStateBase[13]);
				result = true;
			}
		}
		if (!result) {
			listCharViet.clear();
			listCharViet.insert(listCharViet.end(), listCharVietStart.begin(), listCharVietStart.end());
			if ((charQuickVowel != std::tolower(character)) || (!listCharVietEnd.empty())) {
				listCharViet.insert(listCharViet.end(), listCharVietMiddle.begin(), listCharVietMiddle.end());
			}
			listCharViet.insert(listCharViet.end(), listCharVietEnd.begin(), listCharVietEnd.end());
			listCharVietInvalid.clear();
			addCharInvalid(character);
			result = true;
		}
	}
	return result;
}

bool Word::addKeyD(wchar_t character) {
	if (checkKeyNumpad()) {
		return false;
	}
	return addKeyTemplate(character, stateD, Variable::getInstance().dataAddKeyD, &Word::checkKeyD);
}

bool Word::addKeyDiac(wchar_t character) {
	bool result = false;
	if (checkKeyNumpad()) {
		return false;
	}
	if (keyPassDiac.find(towupper(character)) != keyPassDiac.end()) {
		keyPassDiac.clear();
		return true;
	}
	result = addKeyTemplate(character, stateDiac, Variable::getInstance().dataAddKeyDiac, &Word::checkKeyDiac);
	checkEndInvalid();
	return result;
}

bool Word::addKeyTone(wchar_t character) {
	if (checkKeyNumpad()) {
		return false;
	}
	return addKeyTemplate(character, stateTone, Variable::getInstance().dataAddKeyTone, &Word::checkKeyTone);
}

bool Word::checkKeyD(wchar_t character, wchar_t stateDTemp) {
	Variable& variable = Variable::getInstance();
	std::wstring stringVietStart = listCharToString(listCharVietStart);
	bool flagUpper;
	bool flagChangeStateD = false;

	if ((stateDTemp == variable.listStateBase[13]) || (stateDTemp == variable.listStateBase[14]))
	{
		if (stringVietStart == L"d" || stringVietStart == variable.characterSetBase[L'M'][0])
		{
			flagUpper = false;
			flagChangeStateD = true;
		}
		else if (stringVietStart == L"D" || stringVietStart == variable.characterSetBase[L'Z'][0])
		{
			flagUpper = true;
			flagChangeStateD = true;
		}
	}

	if (flagChangeStateD)
	{
		stateD = stateDTemp;
		if (flagUpper) {
			numCharD = 2;
		}
		else {
			numCharD = 1;
		}
		checkCharD();
		return true;
	}
	return false;
}

bool Word::checkKeyDiac(wchar_t character, wchar_t stateDiacTemp) {
	return checkMiddle(stateTone, stateDiacTemp, listCharVowel, false);
}

bool Word::checkKeyTone(wchar_t character, wchar_t stateToneTemp) {
	return checkMiddle(stateToneTemp, stateDiac, listCharVowel, false);
}

bool Word::checkCharGI(wchar_t character) {
	Variable& variable = Variable::getInstance();

	std::vector<std::wstring> listCharVietTemp;
	std::vector<std::wstring> listCharVietTempLower;
	listCharVietTemp.insert(listCharVietTemp.end(), listCharVietStart.begin(), listCharVietStart.end());
	listCharVietTemp.insert(listCharVietTemp.end(), listCharVietMiddle.begin(), listCharVietMiddle.end());
	listCharVietTempLower = listCharVietTemp;
	convertVectorToLower(listCharVietTempLower);
	std::wstring stringCharVietTemp = listCharToString(listCharVietTemp);
	std::wstring stringCharVietTempLower = listCharToString(listCharVietTempLower);

	std::vector<std::wstring> listCharTemp;
	std::vector<std::wstring> listCharTempLower;
	listCharTemp.insert(listCharTemp.end(), listCharVietStart.begin(), listCharVietStart.end());
	listCharTemp.insert(listCharTemp.end(), listCharVowel.begin(), listCharVowel.end());
	listCharTempLower = listCharTemp;
	convertVectorToLower(listCharTempLower);
	std::wstring stringCharTemp = listCharToString(listCharTemp);
	std::wstring stringCharTempLower = listCharToString(listCharTempLower);

	int pos = 0;
	if (stringCharTempLower.substr(0, 2) == L"gi") {
		if (character == L'\0') {
			pos = variable.dataCheckCharGI1.count(stringCharVietTempLower) ? 1 : 2;
		}
		else {
			if ((stringCharTempLower == L"gi") && variable.dataCheckCharGI2.count(std::tolower(character))) {
				pos = 2;
			}
		}

		if (pos > 0) {
			listCharVietStart = std::vector<std::wstring>(listCharTemp.begin(), listCharTemp.begin() + pos);
			listCharVietMiddle = std::vector<std::wstring>(listCharVietTemp.begin() + pos, listCharVietTemp.end());
			listCharVowel = std::vector<std::wstring>(listCharTemp.begin() + pos, listCharTemp.end());
			return true;
		}
	}
	return false;
}

bool Word::checkCharOnsetStart(wchar_t character, std::vector<std::wstring>& listCharVietStartTemp, std::vector<wchar_t>* listParam) {
	std::wstring stringVietStartTemp = listCharToString(listCharVietStartTemp);
	transform(stringVietStartTemp.begin(), stringVietStartTemp.end(), stringVietStartTemp.begin(), towlower);

	if (Variable::getInstance().dataCheckCharOnsetStart.find(stringVietStartTemp) != Variable::getInstance().dataCheckCharOnsetStart.end()) {
		listCharVietStart = listCharVietStartTemp;
		return true;
	}

	return false;
}

bool Word::checkCharD()
{
	if (numCharD == 0) {
		return false;
	}

	Variable& variable = Variable::getInstance();
	if (stateD == variable.listStateBase[13]) {
		if (numCharD == 1) {
			listCharVietStart = { L"d" };
		}
		else {
			listCharVietStart = { L"D" };
		}
	}
	else {
		if (numCharD == 1) {
			listCharVietStart = { variable.characterSetBase[L'M'][0] };
		}
		else {
			listCharVietStart = { variable.characterSetBase[L'Z'][0] };
		}
	}

	return true;
}

bool Word::checkCharVowel(wchar_t character, std::vector<std::wstring>& listCharVowelTemp, std::vector<wchar_t>* listParam) {
	bool result = false;

	wchar_t stateDiacTemp;
	if (listParam == nullptr) {
		stateDiacTemp = stateDiac;
		result = checkMiddle(stateTone, stateDiacTemp, listCharVowelTemp, true);
	}
	else {
		stateDiacTemp = (*listParam)[0];
		result = checkMiddle(stateTone, stateDiacTemp, listCharVowelTemp, false);
	}

	if (result) {
		return true;
	}

	return false;
}

bool Word::checkMiddle(wchar_t stateToneTemp, wchar_t stateDiacTemp, std::vector<std::wstring>& listCharVowelTemp, bool checkResetDiac) {

	std::wstring stringVowelTemp = listCharToString(listCharVowelTemp);
	std::wstring stringVowelTempLower = toLowerCase(stringVowelTemp);
	Variable& variable = Variable::getInstance();

	if (variable.dataCheckMiddle1.find(stringVowelTempLower) != variable.dataCheckMiddle1.end()) {
		std::vector<wchar_t> listStateDiac = variable.dataCheckMiddle1[stringVowelTempLower];

		if (find(listStateDiac.begin(), listStateDiac.end(), stateDiacTemp) == listStateDiac.end() && checkResetDiac) {
			stateDiacTemp = listStateDiac[0];
		}
	}

	std::wstring keyTemp = stateDiacTemp + stringVowelTempLower;
	std::wstring indexTone = (!listCharVietEnd.empty() && (keyTemp == L"Goa" || keyTemp == L"Goe" || keyTemp == L"Guy")) ? L"1" : L"0";
	keyTemp = std::wstring(1, stateToneTemp) + indexTone + std::wstring(1, stateDiacTemp) + stringVowelTemp;

	if (variable.dataCheckMiddle2.find(keyTemp) != variable.dataCheckMiddle2.end()) {
		listCharVietMiddle = variable.dataCheckMiddle2[keyTemp];
		stateTone = stateToneTemp;
		stateDiac = stateDiacTemp;
		listCharVowel = listCharVowelTemp;
		return true;
	}

	return false;
}

bool Word::checkCharEnd(wchar_t character, std::vector<std::wstring>& listCharVietEndTemp, std::vector<wchar_t>* listParam) {
	bool result = false;
	Variable& variable = Variable::getInstance();
	if (!listCharVietEnd.empty() && (keyPassEnd.find(towupper(character)) != keyPassEnd.end())) {
		keyPassEnd.clear();
		listCharVietEnd.back() = std::wstring(1, character);
		return true;
	}

	result = checkEnd(listCharVietEndTemp);
	if (result) {
		if (toUpperCase(listCharVietEnd.back()) != toUpperCase(listCharVietEndTemp.back())) {
			keyPassEnd.insert(towupper(listCharVietEnd.back()[0]));
		}
		return true;
	}

	return false;
}

bool Word::checkEnd(std::vector<std::wstring>& listCharVietEndTemp) {
	std::wstring stringVowelLower = toLowerCase(listCharToString(listCharVowel));
	std::wstring keyTemp = stateDiac + stringVowelLower;

	if (Variable::getInstance().dataCheckEnd.find(keyTemp) != Variable::getInstance().dataCheckEnd.end()) {
		std::map<std::wstring, std::wstring> mapOnsetEnd = Variable::getInstance().dataCheckEnd[keyTemp];
		std::wstring stringCharVietEndTemp = listCharToString(listCharVietEndTemp);

		if (mapOnsetEnd.find(stringCharVietEndTemp) != mapOnsetEnd.end()) {
			std::wstring stringCharVietEnd = mapOnsetEnd[stringCharVietEndTemp];
			listCharVietEnd.clear();
			for (const auto& ch : stringCharVietEnd) {
				listCharVietEnd.push_back(std::wstring(1, ch));
			}
			return true;
		}
	}
	return false;
}

bool Word::checkEndInvalid() {
	bool result = false;

	result = checkEnd(listCharVietEnd);
	if (!result) {
		listCharVietEnd.clear();
		return true;
	}
	return false;
}

bool Word::checkPosTone() {
	std::wstring stringVowel = listCharToString(listCharVowel);
	std::wstring stringVowelLower = toLowerCase(stringVowel);
	std::wstring keyTemp = stateDiac + stringVowelLower;
	std::wstring indexTone = (!listCharVietEnd.empty() && (keyTemp == L"Goa" || keyTemp == L"Goe" || keyTemp == L"Guy")) ? L"1" : L"0";
	keyTemp = std::wstring(1, stateTone) + indexTone + std::wstring(1, stateDiac) + stringVowel;
	if (Variable::getInstance().dataCheckPosTone.find(keyTemp) != Variable::getInstance().dataCheckPosTone.end()) {
		listCharVietMiddle = Variable::getInstance().dataCheckPosTone[keyTemp];
		return true;
	}
	return false;
}

void Word::removeChar() {
	TypeWord& typeWord = TypeWord::getInstance();
	Variable& variable = Variable::getInstance();

	removeDataAutoChangeLang();

	if (variable.modeCheckCase) {
		if (
			(numChangeCaseDisplay == 1) ||
			(numChangeCaseDisplay == 2) ||
			(numChangeCaseDisplay == 3))
		{
			typeWord.numChangeCase = 0;
			numChangeCaseDisplay = 0;
			findListCharDisplay();
			calStepChangeDisplay();
			return;
		}
	}

	if (flagAddCharSpace) {
		removeCharSpace();
	}

	if (!numSwitchLang || !flagLangViet) {
		if (!listCharOrigin.empty()) {
			listCharOrigin.pop_back();
		}
	}

	if (!numSwitchLang || flagLangViet) {
		bool result = false;
		result = removeCharInvalid();
		if (!result) {
			checkCharD();
			if (!listCharViet.empty()) {
				listCharViet.pop_back();
			}
			result = (
				removeCharEnd() ||
				removeCharMiddle() ||
				removeCharStart()
				);
		}
		if (listCharVietInvalid.empty()) {
			stateAdd = 2;
			if (listCharVietEnd.empty()) {
				stateAdd = 1;
				if (listCharVowel.empty()) {
					stateAdd = 0;
				}
			}
		}
	}

	checkCharGI();
	updateListCharDisplay();
	calStepChangeDisplay();

	if (listCharDisplayNew.empty()) {
		numSwitchLang = 0;
		flagAddCharSpace = true;
		flagAddCharInvalid = false;
		stateAdd = 0;
		numCharD = 0;
		stateD = variable.listStateBase[13];
		stateTone = variable.listStateBase[0];
		stateDiac = variable.listStateBase[6];
		charQuickVowel = L'\n';
		listCharVowel.clear();
		listCharDisplayCurrent.clear();
		listCharDisplayNew.clear();
		listCharOrigin.clear();
		listCharViet.clear();
		listCharVietInvalid.clear();
		listCharVietInvalidTemp.clear();
		listCharVietStart.clear();
		listCharVietMiddle.clear();
		listCharVietEnd.clear();
	}
}

bool Word::removeCharSpace()
{
	if (!listCharSpaceNew.empty()) {
		listCharSpaceNew.pop_back();
		return true;
	}
	return false;
}

bool Word::removeCharStart() {
	size_t length = listCharVietStart.size();
	if (length == 1) {
		stateD = Variable::getInstance().listStateBase[13];
		listCharVietStart.clear();
		return true;
	}
	if (length > 1) {
		listCharVietStart.pop_back();
		return true;
	}
	return false;
}

bool Word::removeCharMiddle() {
	Variable& variable = Variable::getInstance();
	size_t length = listCharVowel.size();

	if (length == 1) {
		stateTone = variable.listStateBase[0];
		stateDiac = variable.listStateBase[6];
		listCharVowel.clear();
		listCharVietMiddle.clear();
		return true;
	}

	if (length > 1) {
		std::wstring key = toLowerCase(listCharVietMiddle.back());
		if (variable.dataRemoveCharMiddle1.find(key) != variable.dataRemoveCharMiddle1.end()) {
			stateTone = variable.listStateBase[0];
		}
		key = stateDiac + toLowerCase(listCharToString(listCharVowel));
		stateDiac = variable.dataRemoveCharMiddle2[key];

		listCharVowel.pop_back();
		listCharVietMiddle.pop_back();
		checkCharGI();
		checkMiddle(stateTone, stateDiac, listCharVowel, false);
		return true;
	}
	return false;
}

bool Word::removeCharEnd() {
	if (!listCharVietEnd.empty()) {
		listCharVietEnd.pop_back();
		if (listCharVietEnd.empty()) {
			checkPosTone();
		}
		return true;
	}
	return false;
}

bool Word::removeCharInvalid() {
	if (!listCharVietInvalid.empty()) {
		listCharVietInvalid.pop_back();
		listCharVietInvalidTemp = listCharVietInvalid;
		if (listCharVietInvalid.empty()) {
			checkCharD();
			flagAddCharInvalid = false;
		}
		return true;
	}
	return false;
}

bool Word::changeCase()
{
	TypeWord& typeWord = TypeWord::getInstance();

	std::vector<std::wstring> listCharDisplayTemp = listCharDisplayNew;
	modeChangeCase %= 3;
	switch (modeChangeCase)
	{
	case 0:
		convertVectorToTitle(listCharDisplayTemp);
		if (listCharDisplayTemp == listCharDisplayNew) {
			changeUpperCase();
			typeWord.numChangeCase = 3;
			modeChangeCase += 1;
		}
		else {
			changeTitleCase();
			typeWord.numChangeCase = 2;
		}
		break;

	case 1:
		convertVectorToUpper(listCharDisplayTemp);
		if (listCharDisplayTemp == listCharDisplayNew) {
			changeLowerCase();
			typeWord.numChangeCase = 0;
			modeChangeCase += 1;
		}
		else {
			changeUpperCase();
			typeWord.numChangeCase = 3;
		}
		break;

	case 2:
		convertVectorToLower(listCharDisplayTemp);
		if (listCharDisplayTemp == listCharDisplayNew) {
			changeTitleCase();
			typeWord.numChangeCase = 2;
			modeChangeCase += 1;
		}
		else {
			typeWord.numChangeCase = 0;
			changeLowerCase();
		}
		break;
	}
	modeChangeCase += 1;

	updateListCharDisplay();
	calStepChangeDisplay();
	return true;
}

bool Word::changeLowerCase()
{
	convertVectorToLower(listCharOrigin);
	convertVectorToLower(listCharVietStart);
	convertVectorToLower(listCharVowel);
	convertVectorToLower(listCharVietEnd);
	checkMiddle(stateTone, stateDiac, listCharVowel, false);
	convertVectorToLower(listCharVietInvalid);
	listCharVietInvalidTemp = listCharVietInvalid;
	convertVectorToLower(listCharViet);
	return true;
}

bool Word::changeUpperCase()
{
	convertVectorToUpper(listCharOrigin);
	convertVectorToUpper(listCharVietStart);
	convertVectorToUpper(listCharVowel);
	convertVectorToUpper(listCharVietEnd);
	checkMiddle(stateTone, stateDiac, listCharVowel, false);
	convertVectorToUpper(listCharVietInvalid);
	listCharVietInvalidTemp = listCharVietInvalid;
	convertVectorToUpper(listCharViet);
	return true;
}

bool Word::changeTitleCase()
{
	convertVectorToTitle(listCharOrigin);

	bool result = false;
	if (result) {
		convertVectorToLower(listCharVietStart);
	}
	else {
		result = convertVectorToTitle(listCharVietStart);
	}

	if (result) {
		convertVectorToLower(listCharVowel);
	}
	else {
		result = convertVectorToTitle(listCharVowel);
	}

	if (result) {
		convertVectorToLower(listCharVietEnd);
	}
	else {
		result = convertVectorToTitle(listCharVietEnd);
	}
	checkMiddle(stateTone, stateDiac, listCharVowel, false);

	if (result) {
		convertVectorToLower(listCharVietInvalid);
	}
	else {
		result = convertVectorToTitle(listCharVietInvalid);
	}
	listCharVietInvalidTemp = listCharVietInvalid;

	convertVectorToTitle(listCharViet);
	return true;
}

bool Word::changeCharSet()
{
	checkCharD();
	checkMiddle(stateTone, stateDiac, listCharVowel, false);
	updateListCharDisplay();
	calStepChangeDisplay();
	return true;
}

bool Word::checkKeyNumpad()
{
	Variable& variable = Variable::getInstance();
	int vkCodeCurrent = variable.vkCodeCurrent;
	return ((vkCodeCurrent >= 0x60) && (vkCodeCurrent <= 0x6F));
}

void Word::checkAutoChangeLang()
{
	Variable& variable = Variable::getInstance();
	TypeWord& typeWord = TypeWord::getInstance();

	if(variable.modeAutoChangeLang &&
		!numSwitchLang &&
		flagLangViet)
	{
		std::wstring stringWordEng = listCharToString(listCharOrigin);
		QString qWord = QString::fromStdWString(stringWordEng).toLower();
		std::wstring lowerWord(reinterpret_cast<const wchar_t*>(qWord.utf16()), qWord.length());
		if (variable.dataAutoChangeLang.find(lowerWord) != variable.dataAutoChangeLang.end()) {
			listCharViet = std::vector<std::wstring>(listCharOrigin.begin(), listCharOrigin.end() - 1);
			listCharVietInvalid.clear();
			addCharInvalid(listCharOrigin.back()[0]);
		}
	}
}

void Word::updateListCharDisplay() {
	TypeWord& typeWord = TypeWord::getInstance();
	Variable& variable = Variable::getInstance();

	findListCharDisplay();
	typeWord.calStringSnippet();
	if (variable.modeCheckCase) {
		if ((typeWord.numChangeCase <= 2) && (listCharDisplayNew.size() == 2)) {
			std::wstring lastChar = listCharOrigin[listCharOrigin.size() - 1];
			std::wstring secondLastChar = listCharOrigin[listCharOrigin.size() - 2];
			if (islower(secondLastChar[0]) && iswupper(lastChar[0])) {
				typeWord.numChangeCase = 2;
			}
		}

		if ((typeWord.numChangeCase <= 3) && (listCharDisplayNew.size() == 2)) {
			std::wstring lastChar = listCharOrigin[listCharOrigin.size() - 1];
			std::wstring secondLastChar = listCharOrigin[listCharOrigin.size() - 2];
			if (iswupper(secondLastChar[0]) && iswupper(lastChar[0])) {
				typeWord.numChangeCase = 3;
			}
		}
		
		if ((typeWord.numChangeCase <= 1) && (listCharDisplayNew.size() == 1)) {
			if (typeWord.listWord.size() == 1) {
				typeWord.numChangeCase = 1;
				numChangeCaseDisplay = 2;
			}

			std::wstring stringTemp;
			stringTemp = listCharToString(listCharSpaceNew);
			if (variable.setPunctuation.count(stringTemp)) {
				typeWord.numChangeCase = 1;
				numChangeCaseDisplay = 2;
			}
			else {
				if (typeWord.listWord.size() > 1) {
					Word& wordPrevious = typeWord.listWord[typeWord.posWord - 1];
					stringTemp = listCharToString(wordPrevious.listCharSpaceNew);
					if (variable.setPunctuation.count(stringTemp)) {
						typeWord.numChangeCase = 1;
						numChangeCaseDisplay = 2;
					}
				}
			}
		}

		if (typeWord.numChangeCase == 2) {
			if ((typeWord.listWord.size() == 1) || (listCharSpaceNew.size() > 0)) {
				numChangeCaseDisplay = 2;
			}
			else {
				numChangeCaseDisplay = 1;
			}
		}

		if (typeWord.numChangeCase == 3) {
			numChangeCaseDisplay = 3;
		}

		if (variable.modeCheckCase) {
			if (numChangeCaseDisplay == 1) {
				convertVectorToLower(listCharDisplayNew);
			}
			if (numChangeCaseDisplay == 2) {
				convertVectorToTitle(listCharDisplayNew);
			}
			if (numChangeCaseDisplay == 3) {
				convertVectorToUpper(listCharDisplayNew);
			}
		}
	}
}

void Word::findListCharDisplay()
{
	Variable& variable = Variable::getInstance();
	if (flagLangViet) {
		if (flagAddCharInvalid) {
			listCharDisplayNew = listCharViet;
			listCharDisplayNew.insert(listCharDisplayNew.end(), listCharVietInvalid.begin(), listCharVietInvalid.end());
		}
		else {
			listCharDisplayNew.clear();
			listCharDisplayNew.insert(listCharDisplayNew.end(), listCharVietStart.begin(), listCharVietStart.end());
			listCharDisplayNew.insert(listCharDisplayNew.end(), listCharVietMiddle.begin(), listCharVietMiddle.end());
			listCharDisplayNew.insert(listCharDisplayNew.end(), listCharVietEnd.begin(), listCharVietEnd.end());
		}
	}
	else {
		listCharDisplayNew = listCharOrigin;
	}
}

void Word::calStepChangeDisplay() {
	TypeWord& typeWord = TypeWord::getInstance();
	Variable& variable = Variable::getInstance();

	std::wstring stringCurrent = listCharToString(listCharSpaceCurrent) + listCharToString(listCharDisplayCurrent);
	std::wstring stringNew = listCharToString(listCharSpaceNew) + listCharToString(listCharDisplayNew);

	if (typeWord.stringSnippet != L"") {
		numBackspace = typeWord.numKeySnippet - static_cast<int>(stringNew.size()) + static_cast<int>(stringCurrent.size());
		stringAdd = typeWord.stringSnippet;
		return;
	}

	listCharSpaceCurrent = listCharSpaceNew;
	listCharDisplayCurrent = listCharDisplayNew;

	bool flagRun = true;
	int posChar = 0;
	int lengthNew = static_cast<int>(stringNew.size());
	int lengthCurrent = static_cast<int>(stringCurrent.size());
	int posMin = valueMin(lengthNew, lengthCurrent);
	while (posChar < posMin) {
		if (stringNew[posChar] != stringCurrent[posChar]) {
			break;
		}
		posChar++;
	}

	numBackspace = lengthCurrent - posChar;

	stringAdd = L"";
	for (int i = posChar; i < lengthNew; i++) {
		stringAdd += std::wstring(1, stringNew[i]);
	}
}

void Word::switchLang(bool* flagLangVietTemp)
{
	TypeWord& typeWord = TypeWord::getInstance();
	Variable& variable = Variable::getInstance();

	bool flagLangVietOld = flagLangViet;
	if (flagLangVietTemp == nullptr) {
		if (flagAddCharInvalid) {
			flagAddCharInvalid = false;
			clearListCharVietInvalid();
			flagLangViet = true;
		}
		else {
			flagLangViet = !flagLangViet;
		}
	}
	else {
		flagLangViet = *flagLangVietTemp;
	}

	if (flagLangViet) {
		removeDataAutoChangeLang();
	}

	updateListCharDisplay();
	calStepChangeDisplay();

	if (listCharDisplayNew.empty()) {
		numSwitchLang = 0;
	}
	else {
		numSwitchLang++;
		if (numSwitchLang >= 2) {
			typeWord.showLanguage();
		}
	}
}

void Word::addDataAutoChangeLang()
{
	Variable& variable = Variable::getInstance();

	if ((flagLangViet && flagAddCharInvalid) ||
		(!flagLangViet && numSwitchLang))
	{
		std::wstring stringDisplay = listCharToString(listCharDisplayCurrent);
		QString qstringDisplayLower = QString::fromStdWString(stringDisplay).toLower();
		std::wstring stringDisplayLower(reinterpret_cast<const wchar_t*>(qstringDisplayLower.utf16()), qstringDisplayLower.length());
		if (stringDisplayLower.size() > 1) {
			variable.dataAutoChangeLang.insert(stringDisplayLower);
		}
	}
}

void Word::removeDataAutoChangeLang()
{
	Variable& variable = Variable::getInstance();

	std::wstring stringDisplay = listCharToString(listCharDisplayCurrent);
	QString qstringDisplayLower = QString::fromStdWString(stringDisplay).toLower();
	std::wstring stringDisplayLower(reinterpret_cast<const wchar_t*>(qstringDisplayLower.utf16()), qstringDisplayLower.length());
	std::wstring stringTemp = L"";
	for (wchar_t character : stringDisplayLower) {
		stringTemp += character;
		variable.dataAutoChangeLang.erase(stringTemp);
	}
}

void Word::clearListCharVietInvalid()
{
	for (auto it = listCharVietInvalid.rbegin(); it != listCharVietInvalid.rend(); ++it) {
		if (!listCharOrigin.empty() && listCharOrigin.back() == *it) {
			listCharOrigin.pop_back();
		}
		else {
			break;
		}
	}
	listCharVietInvalid.clear();
}
