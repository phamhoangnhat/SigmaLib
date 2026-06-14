// Variable.cpp

#include "Variable.h"
#include "Util.h"
#include "Clipboard.h"
#include "Listener.h"
#include "NoticeUi.h"
#include "ConfigUi.h"
#include "TaskAI.h"
#include "TaskAIDatabase.h"
#include "SnippetEditor.h"
#include "Typeword.h"
#include "AccountManager.h"
#include <LangRegistryWatcher.h>
#include <CharsetRegistryWatcher.h>

#include <algorithm>
#include <iostream>
#include <cwctype>
#include <windows.h>

#include <QString>
#include <QSettings>


Variable::Variable() {
}

Variable::~Variable() {
}

void Variable::init()
{
	initMapCharacterSetBase();
	initMapInputMethodBase();
	initListStateBase();
	initListOrderState();
	initDataCheckCharOnsetStartBase();
	initListDataMiddleBase();
	initDataRemoveCharMiddle2();
	initMapDataEndBase();
	initSetVirtualKeyCodeValid();
	initSetCharSpaceSnippet();
	initMapConvertCharset();
	initSetPunctuation();
	initDataOnsetStartTeenCode();
	initDataValidateKeyToneDiac();
	initMapDataQuickVowel();
	loadGeneralConfig();
	loadSettingsWindow();
}

void Variable::update()
{
	characterSetBase = mapCharacterSetBase[characterSet];
	inputMethodBase = createInputMethodBase();
	dataCheckCharOnsetStart = createDataCheckCharOnsetStart();
	dataAddCharMiddle1 = createDataAddCharMiddle1();
	dataAddCharMiddle2 = createDataAddCharMiddle2();
	dataAddKeyRemoveDiacToneD = createDataAddKeyRemoveDiacToneD();
	dataAddKeyTone = createDataAddKey(1, 6, listOrderStatePush);
	dataAddKeyDiac = createDataAddKey(7, 13, listOrderStatePush);
	dataAddKeyD = createDataAddKey(14, 15, listOrderStatePush);
	dataCheckMiddle1 = createDataCheckMiddle1();
	dataCheckMiddle2 = createDataCheckMiddle2();
	dataCheckCharGI1 = createDataCheckCharGI1();
	dataCheckCharGI2 = createDataCheckCharGI2();
	dataCheckEnd = createDataCheckEnd();
	dataRemoveCharMiddle1 = createDataRemoveCharMiddle1();
	dataCheckPosTone = createDataCheckPosTone();
	dataAddNewWord = createDataAddNewWord();
	dataAddCharSpace = createDataAddCharSpace();
	dataCheckModeRestore = createDataCheckModeRestore();
	dataChangeCaseLower = createDataChangeCaseLower();
	dataChangeCaseUpper = createDataChangeCaseUpper();
}

void Variable::loadGeneralConfig()
{
	AccountManager* accountManager = AccountManager::getInstance();
	namePreviousWindow = "";

	QSettings settingsConfig(APP_NAME, "Config");
	settingsConfig.setValue("appNameFull", appNameFull);
	verSigmaExe = settingsConfig.value("verSigmaExe", 0.0).toDouble();

	modeAutoStart = settingsConfig.value("modeAutoStart", MODEAUTOSTART).toBool();
	if (isLoggingByAdmin()) {
		modeAdmin = settingsConfig.value("modeAdmin", MODEADMIN).toBool();
	}
	else {
		modeAdmin = false;
	}
	createAdminTaskInScheduler(modeAutoStart, modeAdmin);
	modeAutoUpdate = settingsConfig.value("modeAutoUpdate", MODEAUTOUPDATE).toBool();

	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/Config");
	inputMethod = settings.value("inputMethod", QString::fromStdWString(INPUTMETHOD)).toString().toStdWString();
	if (mapInputMethodBase.find(inputMethod) == mapInputMethodBase.end()) {
		inputMethod = INPUTMETHOD;
	}
	modeRestore = settings.value("modeRestore", MODERESTORE).toBool();
	modeAutoChangeLang = settings.value("modeAutoChangeLang", MODEAUTOCHANGELANG).toBool();
	modeRemoveDiacTone = settings.value("modeRemoveDiacTone", MODEREMOVEDIACTONE).toBool();
	modeLoopDiacTone = settings.value("modeLoopDiacTone", MODELOOPDIACTONE).toBool();
	modeInsertChar = settings.value("modeInsertChar", MODEINSERTCHAR).toBool();
	settings.endGroup();

	listAppUseClipboard = {
		//SAP
		"saplogon", "saplgpad", "saplogonpad",
	};

	listAppFixAutoSuggest = {
		//Trình duyệt web
		"chrome", "coccoc", "brave", "msedge", "opera", "vivaldi", "firefox",

		//Ứng dụng chat nhắn tin
		//"zalo", "telegram", "discord", "whatsapp", "skype", "slack", "teams", "ms teams",

		//Ứng dụng họp / học trực tuyến
		//"zoom", "classin", "cisco", "webex", "atmgr",

		//Ứng dụng văn phòng (Microsoft & WPS)
		"applicationframehost", "winword", "excel", "powerpnt", "outlook", "msaccess", "onenote", "wps", "et", "wpp", "soffice",

		//IDE & Editor (trình soạn mã, lập trình)
		//"code", "devenv", "notepad++", "sublime_text", "atom", "eclipse", "pycharm64", "idea64", "webstorm64", "phpstorm64", "clion64", "studio64", "netbeans64", "codeblocks", "devcpp", "arduino", "thonny", "rstudio", "geany", "vscode-insiders",

		//Ghi chú / Markdown / Quản lý nội dung
		//"notion", "obsidian", "joplin", "marktext", "typora",

		//Trình nền game / launcher (có khung chat hoặc overlay)
		//"steam", "epicgameslauncher", "riotclient", "battle.net", "robloxplayerbeta",

		//Thiết kế / sáng tạo (có nhập chữ)
		"acad",
		//"photoshop", "illustrator", "coreldraw", "blender", "krita", "sketchup"
	};

	listAppLangVietGlobal = {
		//Trình duyệt web
		"chrome", "coccoc", "brave", "msedge", "opera", "vivaldi", "firefox",

		//Ứng dụng chat nhắn tin
		"zalo", "telegram", "discord", "whatsapp", "skype", "slack", "teams", "ms teams",

		//Ứng dụng họp / học trực tuyến
		"zoom", "classin", "cisco", "webex", "atmgr",

		//Ứng dụng văn phòng (Microsoft & WPS)
		"applicationframehost", "winword", "excel", "powerpnt", "outlook", "msaccess", "onenote", "wps", "et", "wpp", "soffice", "notepad",

		//Ghi chú / Markdown / Quản lý nội dung
		"notion", "obsidian", "joplin", "marktext", "typora",
	};
}

void Variable::initMapCharacterSetBase()
{
	{
		std::map<wchar_t, std::vector<std::wstring>> uni;
		uni[L'A'] = { L"a", L"á", L"à", L"ả", L"ã", L"ạ" };
		uni[L'B'] = { L"ă", L"ắ", L"ằ", L"ẳ", L"ẵ", L"ặ" };
		uni[L'C'] = { L"â", L"ấ", L"ầ", L"ẩ", L"ẫ", L"ậ" };
		uni[L'D'] = { L"e", L"é", L"è", L"ẻ", L"ẽ", L"ẹ" };
		uni[L'E'] = { L"ê", L"ế", L"ề", L"ể", L"ễ", L"ệ" };
		uni[L'F'] = { L"i", L"í", L"ì", L"ỉ", L"ĩ", L"ị" };
		uni[L'G'] = { L"o", L"ó", L"ò", L"ỏ", L"õ", L"ọ" };
		uni[L'H'] = { L"ô", L"ố", L"ồ", L"ổ", L"ỗ", L"ộ" };
		uni[L'I'] = { L"ơ", L"ớ", L"ờ", L"ở", L"ỡ", L"ợ" };
		uni[L'J'] = { L"u", L"ú", L"ù", L"ủ", L"ũ", L"ụ" };
		uni[L'K'] = { L"ư", L"ứ", L"ừ", L"ử", L"ữ", L"ự" };
		uni[L'L'] = { L"y", L"ý", L"ỳ", L"ỷ", L"ỹ", L"ỵ" };
		uni[L'M'] = { L"đ" };
		uni[L'N'] = { L"A", L"Á", L"À", L"Ả", L"Ã", L"Ạ" };
		uni[L'O'] = { L"Ă", L"Ắ", L"Ằ", L"Ẳ", L"Ẵ", L"Ặ" };
		uni[L'P'] = { L"Â", L"Ấ", L"Ầ", L"Ẩ", L"Ẫ", L"Ậ" };
		uni[L'Q'] = { L"E", L"É", L"È", L"Ẻ", L"Ẽ", L"Ẹ" };
		uni[L'R'] = { L"Ê", L"Ế", L"Ề", L"Ể", L"Ễ", L"Ệ" };
		uni[L'S'] = { L"I", L"Í", L"Ì", L"Ỉ", L"Ĩ", L"Ị" };
		uni[L'T'] = { L"O", L"Ó", L"Ò", L"Ỏ", L"Õ", L"Ọ" };
		uni[L'U'] = { L"Ô", L"Ố", L"Ồ", L"Ổ", L"Ỗ", L"Ộ" };
		uni[L'V'] = { L"Ơ", L"Ớ", L"Ờ", L"Ở", L"Ỡ", L"Ợ" };
		uni[L'W'] = { L"U", L"Ú", L"Ù", L"Ủ", L"Ũ", L"Ụ" };
		uni[L'X'] = { L"Ư", L"Ứ", L"Ừ", L"Ử", L"Ữ", L"Ự" };
		uni[L'Y'] = { L"Y", L"Ý", L"Ỳ", L"Ỷ", L"Ỹ", L"Ỵ" };
		uni[L'Z'] = { L"Đ" };

		mapCharacterSetBase[L"Unicode"] = uni;
	}

	{
		std::map<wchar_t, std::vector<std::wstring>> vni;
		vni[L'A'] = { L"a",  L"aù", L"aø", L"aû", L"aõ", L"aï" };
		vni[L'B'] = { L"aê", L"aé", L"aè", L"aú", L"aü", L"aë" };
		vni[L'C'] = { L"aâ", L"aá", L"aà", L"aå", L"aã", L"aä" };
		vni[L'D'] = { L"e",  L"eù", L"eø", L"eû", L"eõ", L"eï" };
		vni[L'E'] = { L"eâ", L"eá", L"eà", L"eå", L"eã", L"eä" };
		vni[L'F'] = { L"i",  L"í",  L"ì",  L"æ",  L"ó",  L"ò"  };
		vni[L'G'] = { L"o",  L"où", L"oø", L"oû", L"oõ", L"oï" };
		vni[L'H'] = { L"oâ", L"oá", L"oà", L"oå", L"oã", L"oä" };
		vni[L'I'] = { L"ô",  L"ôù", L"ôø", L"ôû", L"ôõ", L"ôï" };
		vni[L'J'] = { L"u",  L"uù", L"uø", L"uû", L"uõ", L"uï" };
		vni[L'K'] = { L"ö",  L"öù", L"öø", L"öû", L"öõ", L"öï" };
		vni[L'L'] = { L"y",  L"yù", L"yø", L"yû", L"yõ", L"î"  };
		vni[L'M'] = { L"ñ" };
		vni[L'N'] = { L"A",  L"AÙ", L"AØ", L"AÛ", L"AÕ", L"AÏ" };
		vni[L'O'] = { L"AÊ", L"AÉ", L"AÈ", L"AÚ", L"AÜ", L"AË" };
		vni[L'P'] = { L"AÂ", L"AÁ", L"AÀ", L"AÅ", L"AÃ", L"AÄ" };
		vni[L'Q'] = { L"E",  L"EÙ", L"EØ", L"EÛ", L"EÕ", L"EÏ" };
		vni[L'R'] = { L"EÂ", L"EÁ", L"EÀ", L"EÅ", L"EÃ", L"EÄ" };
		vni[L'S'] = { L"I",  L"Í",  L"Ì",  L"Æ",  L"Ó",  L"Ò"  };
		vni[L'T'] = { L"O",  L"OÙ", L"OØ", L"OÛ", L"OÕ", L"OÏ" };
		vni[L'U'] = { L"OÂ", L"OÁ", L"OÀ", L"OÅ", L"OÃ", L"OÄ" };
		vni[L'V'] = { L"Ô",  L"ÔÙ", L"ÔØ", L"ÔÛ", L"ÔÕ", L"ÔÏ" };
		vni[L'W'] = { L"U",  L"UÙ", L"UØ", L"UÛ", L"UÕ", L"UÏ" };
		vni[L'X'] = { L"Ö",  L"ÖÙ", L"ÖØ", L"ÖÛ", L"ÖÕ", L"ÖÏ" };
		vni[L'Y'] = { L"Y",  L"YÙ", L"YØ", L"YÛ", L"YÕ", L"Î"  };
		vni[L'Z'] = { L"Ñ" };
		mapCharacterSetBase[L"VNI Windows"] = vni;
	}

	{
		std::map<wchar_t, std::vector<std::wstring>> tcvn;
		tcvn[L'A'] = { L"a", L"¸", L"µ", L"¶", L"·", L"¹" };
		tcvn[L'B'] = { L"¨", L"¾", L"»", L"¼", L"½", L"Æ" };
		tcvn[L'C'] = { L"©", L"Ê", L"Ç", L"È", L"É", L"Ë" };
		tcvn[L'D'] = { L"e", L"Ð", L"Ì", L"Î", L"Ï", L"Ñ" };
		tcvn[L'E'] = { L"ª", L"Õ", L"Ò", L"Ó", L"Ô", L"Ö" };
		tcvn[L'F'] = { L"i", L"Ý", L"×", L"Ø", L"Ü", L"Þ" };
		tcvn[L'G'] = { L"o", L"ã", L"ß", L"á", L"â", L"ä" };
		tcvn[L'H'] = { L"«", L"è", L"å", L"æ", L"ç", L"é" };
		tcvn[L'I'] = { L"¬", L"í", L"ê", L"ë", L"ì", L"î" };
		tcvn[L'J'] = { L"u", L"ó", L"ï", L"ñ", L"ò", L"ô" };
		tcvn[L'K'] = { L"­", L"ø", L"õ", L"ö", L"÷", L"ù"  };
		tcvn[L'L'] = { L"y", L"ý", L"ú", L"û", L"ü", L"þ" };
		tcvn[L'M'] = { L"®" };
		tcvn[L'N'] = { L"A", L"¸", L"µ", L"¶", L"·", L"¹" };
		tcvn[L'O'] = { L"¡", L"¾", L"»", L"¼", L"½", L"Æ" };
		tcvn[L'P'] = { L"¢", L"Ê", L"Ç", L"È", L"É", L"Ë" };
		tcvn[L'Q'] = { L"E", L"Ð", L"Ì", L"Î", L"Ï", L"Ñ" };
		tcvn[L'R'] = { L"£", L"Õ", L"Ò", L"Ó", L"Ô", L"Ö" };
		tcvn[L'S'] = { L"I", L"Ý", L"×", L"Ø", L"Ü", L"Þ" };
		tcvn[L'T'] = { L"O", L"ã", L"ß", L"á", L"â", L"ä" };
		tcvn[L'U'] = { L"¤", L"è", L"å", L"æ", L"ç", L"é" };
		tcvn[L'V'] = { L"¥", L"í", L"ê", L"ë", L"ì", L"î" };
		tcvn[L'W'] = { L"U", L"ó", L"ï", L"ñ", L"ò", L"ô" };
		tcvn[L'X'] = { L"¦", L"ø", L"õ", L"ö", L"÷", L"ù" };
		tcvn[L'Y'] = { L"Y", L"ý", L"ú", L"û", L"ü", L"þ" };
		tcvn[L'Z'] = { L"§" };
		mapCharacterSetBase[L"TCVN3 (ABC)"] = tcvn;
	}
}

void Variable::initMapInputMethodBase()
{
	mapInputMethodBase[L"Telex"] = {
		L"Z",	L"S",	L"F",	L"R",	L"X",	L"J",	L"",	L"W",	L"A",	L"E",	L"O",	L"W",	L"W",	L"",	L"D"
	};
	mapInputMethodBase[L"Vni"] = {
		L"0",	L"1",	L"2",	L"3",	L"4",	L"5",	L"",	L"8",	L"6",	L"6",	L"6",	L"7",	L"7",	L"",	L"9"
	};
	mapInputMethodBase[L"Tích hợp"] = {
		L"Z0",	L"S1",	L"F2",	L"R3",	L"X4",	L"J5",	L"",	L"W8",	L"A6",	L"E6",	L"O6",	L"W7",	L"W7",	L"",	L"D9"
	};
	mapInputMethodBase[L"Tùy chỉnh"] = {
		L"Z0",	L"S1",	L"F2",	L"R3",	L"X4",	L"J5",	L"",	L"W8",	L"A6",	L"E6",	L"O6",	L"W7",	L"W7",	L"",	L"D9"
	};
}

void Variable::initListStateBase()
{
	listStateBase = { L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O' };
}

void Variable::initListOrderState()
{
	listOrderStatePush  = { 0, 1, 2, 3, 4, 5, 6, 10, 12, 11, 9, 8, 7, 13, 14 };
	listOrderStateQuick = { 0, 1, 2, 3, 4, 5, 6, 10, 12, 11, 9, 8, 7, 13, 14 };
}

void Variable::initDataCheckCharOnsetStartBase()
{
	dataCheckCharOnsetStartBase = {
		L"", L"b", L"c", L"ch", L"d", L"g", L"gh", L"gi", L"h", L"k", L"kh", L"l", L"m", L"n", L"ng", L"ngh", L"nh", L"p", L"ph", L"q", L"qu", L"r", L"s", L"t", L"th", L"tr", L"v", L"x"
	};
}

void Variable::initListDataMiddleBase()
{
	listDataMiddleBase = {
		 { L'G', L"0", 0, L"a",   L"A"   },
		 { L'H', L"0", 0, L"a",   L"B"   },
		 { L'I', L"0", 0, L"a",   L"C"   },
		 { L'G', L"0", 0, L"e",   L"D"   },
		 { L'J', L"0", 0, L"e",   L"E"   },
		 { L'G', L"0", 0, L"i",   L"F"   },
		 { L'G', L"0", 0, L"ia",  L"FA"  },
		 { L'G', L"0", 1, L"ie",  L"FD"  },
		 { L'J', L"0", 1, L"ie",  L"FE"  },
		 { L'G', L"0", 0, L"o",   L"G"   },
		 { L'K', L"0", 0, L"o",   L"H"   },
		 { L'L', L"0", 0, L"o",   L"I"   },
		 { L'G', L"0", 0, L"oa",  L"GA"  },
		 { L'G', L"1", 1, L"oa",  L"GA"  },
		 { L'H', L"0", 1, L"oa",  L"GB"  },
		 { L'G', L"0", 0, L"oe",  L"GD"  },
		 { L'G', L"1", 1, L"oe",  L"GD"  },
		 { L'G', L"0", 0, L"u",   L"J"   },
		 { L'M', L"0", 0, L"u",   L"K"   },
		 { L'G', L"0", 0, L"ua",  L"JA"  },
		 { L'M', L"0", 0, L"ua",  L"KA"  },
		 { L'I', L"0", 1, L"ua",  L"JC"  },
		 { L'G', L"0", 1, L"ue",  L"JD"  },
		 { L'J', L"0", 1, L"ue",  L"JE"  },
		 { L'G', L"0", 1, L"uo",  L"JG"  },
		 { L'K', L"0", 1, L"uo",  L"JH"  },
		 { L'M', L"0", 1, L"uo",  L"KI"  },
		 { L'L', L"0", 1, L"uo",  L"JI"  },
		 { L'G', L"0", 0, L"uy",  L"JL"  },
		 { L'G', L"1", 1, L"uy",  L"JL"  },
		 { L'G', L"0", 1, L"uya", L"JLA" },
		 { L'G', L"0", 2, L"uye", L"JLD" },
		 { L'J', L"0", 2, L"uye", L"JLE" },
		 { L'G', L"0", 0, L"y",   L"L"   },
		 { L'G', L"0", 1, L"ye",  L"LD"  },
		 { L'J', L"0", 1, L"ye",  L"LE"  }
	};
}

void Variable::initDataRemoveCharMiddle2()
{
	dataRemoveCharMiddle2 = {
		{ L"Ga",   L'G' },
		{ L"Ha",   L'G' },
		{ L"Ia",   L'G' },
		{ L"Ge",   L'G' },
		{ L"Je",   L'G' },
		{ L"Gi",   L'G' },
		{ L"Gia",  L'G' },
		{ L"Gie",  L'G' },
		{ L"Jie",  L'G' },
		{ L"Go",   L'G' },
		{ L"Ko",   L'G' },
		{ L"Lo",   L'G' },
		{ L"Goa",  L'G' },
		{ L"Hoa",  L'G' },
		{ L"Goe",  L'G' },
		{ L"Gu",   L'G' },
		{ L"Mu",   L'G' },
		{ L"Gua",  L'G' },
		{ L"Mua",  L'M' },
		{ L"Iua",  L'G' },
		{ L"Gue",  L'G' },
		{ L"Jue",  L'G' },
		{ L"Guo",  L'G' },
		{ L"Kuo",  L'G' },
		{ L"Muo",  L'M' },
		{ L"Luo",  L'G' },
		{ L"Guy",  L'G' },
		{ L"Guya", L'G' },
		{ L"Guye", L'G' },
		{ L"Juye", L'G' },
		{ L"Gy",   L'G' },
		{ L"Gye",  L'G' },
		{ L"Jye",  L'G' }
	};
}

void Variable::initMapDataEndBase()
{
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"nh", L"nh" },
			{ L"o",  L"o" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" },
			{ L"y",  L"y" }
		};
		mapDataEndBase[L"G"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"nh", L"nh" },
			{ L"o",  L"o" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" },
			{ L"y",  L"y" }
		};
		mapDataEndBase[L"Ga"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"c",  L"c" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" }
		};
		mapDataEndBase[L"Ha"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"nh", L"nh" },
			{ L"o",  L"o" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" },
			{ L"y",  L"y" }
		};
		mapDataEndBase[L"Ia"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"nh", L"nh" },
			{ L"o",  L"o" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Ge"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"nh", L"nh" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Je"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"nh", L"nh" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Gi"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"", L"" }
		};
		mapDataEndBase[L"Gia"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"c",  L"c" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Gie"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"c",  L"c" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Jie"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" }
		};
		mapDataEndBase[L"Go"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"p",  L"p" },
			{ L"t",  L"t" }
		};
		mapDataEndBase[L"Lo"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" }
		};
		mapDataEndBase[L"Ko"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"nh", L"nh" },
			{ L"o",  L"o" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"y",  L"y" }
		};
		mapDataEndBase[L"Goa"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"c",  L"c" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" }
		};
		mapDataEndBase[L"Hoa"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"n",  L"n" },
			{ L"o",  L"o" },
			{ L"p",  L"p" },
			{ L"t",  L"t" }
		};
		mapDataEndBase[L"Goe"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Gu"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Mu"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"t",  L"t" },
			{ L"y",  L"y" }
		};
		mapDataEndBase[L"Gua"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"", L"" }
		};
		mapDataEndBase[L"Mua"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"c",  L"c" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"t",  L"t" },
			{ L"y",  L"y" }
		};
		mapDataEndBase[L"Iua"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"n",  L"n" },
			{ L"nh", L"nh" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Gue"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"n",  L"n" },
			{ L"nh", L"nh" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Jue"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"c",  L"c" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Guo"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"c",  L"c" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Kuo"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"c",  L"c" },
			{ L"i",  L"i" },
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Muo"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"", L"" }
		};
		mapDataEndBase[L"Luo"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"n",  L"n" },
			{ L"nh", L"nh" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Guy"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"", L"" }
		};
		mapDataEndBase[L"Guya"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"n", L"n" },
			{ L"t", L"t" }
		};
		mapDataEndBase[L"Guye"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"n", L"n" },
			{ L"t", L"t" }
		};
		mapDataEndBase[L"Juye"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"",   L"" },
			{ L"c",  L"c" },
			{ L"ch", L"ch" },
			{ L"n",  L"n" },
			{ L"nh", L"nh" },
			{ L"p",  L"p" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Gy"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Gye"] = temp;
	}
	{
		std::map<std::wstring, std::wstring> temp = {
			{ L"m",  L"m" },
			{ L"n",  L"n" },
			{ L"ng", L"ng" },
			{ L"t",  L"t" },
			{ L"u",  L"u" }
		};
		mapDataEndBase[L"Jye"] = temp;
	}
}

void Variable::initSetVirtualKeyCodeValid() {
	setVirtualKeyCodeValid = {
		// Chữ cái
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A,
		0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
		0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,

		// Số
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,

		// Dấu câu và ký tự
		0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xDB, 0xDC, 0xDD,
		0xDE,

		// Dấu cách, Tab
		0x20, 0x09,

		// NumPad
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
		0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	};
}

void Variable::initSetCharSpaceSnippet()
{
	setCharSpaceSnippet = {
		L';', L':',
		L'=', L'+',
		L',', L'<',
		L'-', L'_',
		L'.', L'>',
		L'/', L'?',
		L'`', L'~',
		L'[', L'{',
		L'\\', L'|',
		L']', L'}',
		L'\'', L'"',
		L' ',
		L'\t'
	};
}

void Variable::initMapConvertCharset()
{
	std::map<wchar_t, std::vector<std::wstring>> dataUni = mapCharacterSetBase[L"Unicode"];
	std::map<wchar_t, std::vector<std::wstring>> dataVni = mapCharacterSetBase[L"VNI Windows"];
	std::map<wchar_t, std::vector<std::wstring>> dataAbc = mapCharacterSetBase[L"TCVN3 (ABC)"];

	for (const auto& pair : dataUni) {
		wchar_t key = pair.first;
		std::vector<std::wstring>& listCharUni = dataUni[key];
		std::vector<std::wstring>& listCharVni = dataVni[key];
		std::vector<std::wstring>& listCharAbc = dataAbc[key];

		for (int i = 0; i < listCharUni.size(); i++) {
			std::wstring& charUni = listCharUni[i];
			std::wstring& charVni = listCharVni[i];
			std::wstring& charAbc = listCharAbc[i];
			mapUniToVni[charUni] = charVni;
			mapUniToAbc[charUni] = charAbc;
		}
	}
}

void Variable::initSetPunctuation()
{
	setPunctuation = { L".", L"?", L"!" };
}

void Variable::initDataOnsetStartTeenCode()
{
	dataOnsetStartTeenCode =
	{ L"f",  L"j",  L"w", L"z" };
}

void Variable::initDataValidateKeyToneDiac()
{
	dataValidateKeyToneDiac = {
		QChar('0'), QChar('1'), QChar('2'), QChar('3'), QChar('4'),
		QChar('5'), QChar('6'), QChar('7'), QChar('8'), QChar('9'),
		QChar('A'), QChar('B'), QChar('C'), QChar('D'), QChar('E'),
		QChar('F'), QChar('G'), QChar('H'), QChar('I'), QChar('J'),
		QChar('K'), QChar('L'), QChar('M'), QChar('N'), QChar('O'),
		QChar('P'), QChar('Q'), QChar('R'), QChar('S'), QChar('T'),
		QChar('U'), QChar('V'), QChar('W'), QChar('X'), QChar('Y'),
		QChar('Z'), QChar('`'), QChar('-'), QChar('='), QChar('['),
		QChar(']'), QChar('\\'), QChar(';'), QChar('\''), QChar(','),
		QChar('.'), QChar('/')
	};
}

void Variable::initMapDataQuickVowel()
{
	mapDataQuickVowel =
	{
		{ L'`', L'~'},
		{ L'-', L'_'},
		{ L'=', L'+'},
		{ L'[', L'{'},
		{ L']', L'}'},
		{ L'\\', L'|'},
		{ L';', L':'},
		{ L'\'', L'"'},
		{ L',', L'<'},
		{ L'.', L'>'},
		{ L'/', L'?'},
	};
}

void Variable::loadDataAutoChangeLang(QString& nameApp)
{
	AccountManager* accountManager = AccountManager::getInstance();

	dataAutoChangeLang.clear();
	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/DataAutoChangeLang");
	if (settings.contains(nameApp)) {
		QStringList stored = settings.value(nameApp).toStringList();
		for (const QString& item : stored) {
			std::wstring lowerItem = item.toLower().toStdWString();
			if(lowerItem.size() > 1) {
				dataAutoChangeLang.insert(lowerItem);
			}
		}
	}
	settings.endGroup();
}

std::vector<std::wstring> Variable::createInputMethodBase() {
	AccountManager* accountManager = AccountManager::getInstance();
	Variable& variable = Variable::getInstance();
	std::vector<std::wstring> inputMethodBase;

	if (mapInputMethodBase.find(inputMethod) == mapInputMethodBase.end()) {
		inputMethod = INPUTMETHOD;
	}

	inputMethodBase = mapInputMethodBase[inputMethod];
	if (inputMethod == L"Tùy chỉnh") {
		inputMethodBase.clear();
		inputMethodBase.resize(15);
		QSettings settings(APP_NAME, "AccountManager");
		settings.beginGroup(accountManager->currentAccount + "/InputMethodCustom");
		if (settings.contains("data")) {
			QStringList stored = settings.value("data").toStringList();

			for (int i = 0; i < std::min(15, static_cast<int>(stored.size())); ++i) {
				QString stringRaw;
				if (i < stored.size()) {
					stringRaw = stored[i];
				}
				if (stringRaw.size() == 0) {
					stringRaw = QString::fromStdWString(variable.mapInputMethodBase[L"Tùy chỉnh"][i]);
				}
				addKeyInputMethod(stringRaw, i, inputMethodBase);
			}
		}
		settings.endGroup();
	}
	return inputMethodBase;
}

std::unordered_set<std::wstring> Variable::createDataCheckCharOnsetStart()
{
	std::unordered_set<std::wstring> dataCheckCharOnsetStart = dataCheckCharOnsetStartBase;
	if (modeTeenCode) {
		dataCheckCharOnsetStart.insert(dataOnsetStartTeenCode.begin(), dataOnsetStartTeenCode.end());
	}
	return dataCheckCharOnsetStart;
}

std::unordered_set<wchar_t> Variable::createDataAddKeyRemoveDiacToneD()
{
	std::unordered_set<wchar_t> dataAddKeyRemoveDiacToneD;

	const std::wstring& firstItem = inputMethodBase[0];
	for (wchar_t key : firstItem) {
		dataAddKeyRemoveDiacToneD.insert(key);
	}
	return dataAddKeyRemoveDiacToneD;
}

std::map<wchar_t, std::vector<wchar_t>> Variable::createDataAddKey(int numStart, int numEnd, std::vector<int> listOrderState)
{
	std::map<wchar_t, std::vector<wchar_t>> dataAddKey;
	wchar_t stateBase = listStateBase[numStart - 1];
	for (int i = numStart; i < numEnd; i++)
	{
		int pos = listOrderState[i];
		wchar_t state = listStateBase[pos];
		std::wstring& listKey = inputMethodBase[pos];
		for (wchar_t key : listKey)
		{
			if (dataAddKey.find(key) == dataAddKey.end())
			{
				dataAddKey[key].push_back(stateBase);
			}
			dataAddKey[key].push_back(state);
		}
	}
	return dataAddKey;
}

std::map<wchar_t, std::unordered_set<wchar_t>> Variable::createDataAddCharMiddle1()
{
	std::map<wchar_t, std::unordered_set<wchar_t>> dataAddCharMiddle1;
	for (int i = 7; i < 13; i++) {
		wchar_t stateDiac = listStateBase[i];
		std::wstring& strKey = inputMethodBase[i];
		for (wchar_t key : strKey) {
			dataAddCharMiddle1[stateDiac].insert(towupper(key));
		}
	}
	return dataAddCharMiddle1;
}

std::map<wchar_t, std::pair<wchar_t, wchar_t>> Variable::createDataAddCharMiddle2() {
	std::map<wchar_t, std::vector<wchar_t>> dataAddKeyDiac = createDataAddKey(7, 13, listOrderStateQuick);
	std::map<wchar_t, wchar_t> dataVowelQuick = {
		{ L'H', L'a' },
		{ L'I', L'a' },
		{ L'J', L'e' },
		{ L'K', L'o' },
		{ L'L', L'o' },
		{ L'M', L'u' }
	};

	std::set<wchar_t> dataKeyDiacQuickInvalid = { L'A', L'E', L'O', L'U' };
	std::map<wchar_t, std::pair<wchar_t, wchar_t>> dataAddCharMiddle2;

	for (const auto& entry : dataAddKeyDiac) {
		wchar_t keyDiac = entry.first;
		const std::vector<wchar_t>& listStateDiac = entry.second;

		if (dataKeyDiacQuickInvalid.find(keyDiac) == dataKeyDiacQuickInvalid.end()) {
			wchar_t stateDiac = listStateDiac[1];
			auto it = dataVowelQuick.find(stateDiac);
			if (it != dataVowelQuick.end()) {
				wchar_t keyVowel = it->second;
				wchar_t stringVowelLower = towlower(keyVowel);
				if (!(keyDiac >= L'0' && keyDiac <= L'9')) {
					wchar_t keyDiac1 = towupper(keyDiac);
					wchar_t keyDiac2 = towlower(keyDiac);
					dataAddCharMiddle2[keyDiac1] = { stateDiac, stringVowelLower };
					dataAddCharMiddle2[keyDiac2] = { stateDiac, stringVowelLower };
				}
				if (mapDataQuickVowel.find(keyDiac) != mapDataQuickVowel.end()) {
					wchar_t keyDiac1 = keyDiac;
					wchar_t keyDiac2 = mapDataQuickVowel[keyDiac];
					dataAddCharMiddle2[keyDiac1] = { stateDiac, stringVowelLower };
					dataAddCharMiddle2[keyDiac2] = { stateDiac, stringVowelLower };
				}
			}
		}
	}
	return dataAddCharMiddle2;
}

std::map<std::wstring, std::vector<wchar_t>> Variable::createDataCheckMiddle1() {
	std::map<std::wstring, std::vector<wchar_t>> dataCheckMiddle1;

	for (const auto& tupleTemp : listDataMiddleBase) {
		wchar_t stateDiac = std::get<0>(tupleTemp);
		std::wstring stringVowel = std::get<3>(tupleTemp);

		if (dataCheckMiddle1.find(stringVowel) != dataCheckMiddle1.end()) {
			std::vector<wchar_t>& listStateDiac = dataCheckMiddle1[stringVowel];
			if (find(listStateDiac.begin(), listStateDiac.end(), stateDiac) == listStateDiac.end()) {
				listStateDiac.push_back(stateDiac);
			}
		}
		else {
			dataCheckMiddle1[stringVowel] = { stateDiac };
		}
	}
	return dataCheckMiddle1;
}


std::map<std::wstring, std::vector<std::wstring>> Variable::createDataCheckMiddle2() {
	std::map<std::wstring, std::vector<std::wstring>> dataCheckMiddle2;
	for (const auto& tupleTemp : listDataMiddleBase) {
		wchar_t stateDiac = std::get<0>(tupleTemp);
		std::wstring indexTone = std::get<1>(tupleTemp);
		int posTone = std::get<2>(tupleTemp);
		std::wstring stringVowel = std::get<3>(tupleTemp);
		std::wstring keyCharacterSet = std::get<4>(tupleTemp);

		int lenVowel = static_cast<int>(stringVowel.size());
		std::vector<wchar_t> listStateTone(listStateBase.begin(), listStateBase.begin() + 6);

		for (int i = 0; i < static_cast<int>(listStateTone.size()); i++) {
			wchar_t stateTone = listStateTone[i];
			std::vector<std::wstring> listMask;

			if (lenVowel == 1) {
				listMask = { L"0", L"1" };
			}
			else if (lenVowel == 2) {
				listMask = { L"00", L"11", L"10", L"01" };
			}
			else if (lenVowel == 3) {
				listMask = { L"000", L"111", L"100", L"010", L"001", L"011", L"101", L"110" };
			}

			for (const std::wstring& mask : listMask) {
				std::wstring stringVowelTemp;
				std::vector<std::wstring> listCharVietMiddleTemp;

				for (int posMask = 0; posMask < lenVowel; posMask++) {
					int numDelta = (mask[posMask] == L'1') ? 13 : 0;

					wchar_t charVowel = stringVowel[posMask];
					if (numDelta == 13) {
						stringVowelTemp.push_back(towupper(charVowel));
					}
					else {
						stringVowelTemp.push_back(charVowel);
					}

					wchar_t keyTemp1 = static_cast<wchar_t>(keyCharacterSet[posMask] + numDelta);

					if (posMask == posTone) {
						listCharVietMiddleTemp.push_back(characterSetBase[keyTemp1][i]);
					}
					else {
						listCharVietMiddleTemp.push_back(characterSetBase[keyTemp1][0]);
					}
				}

				std::wstring keyTemp2 = std::wstring(1, stateTone)
					+ indexTone
					+ std::wstring(1, stateDiac)
					+ stringVowelTemp;
				dataCheckMiddle2[keyTemp2] = listCharVietMiddleTemp;
			}
		}
	}
	return dataCheckMiddle2;
}

std::unordered_set<std::wstring> Variable::createDataCheckCharGI1() {
	std::unordered_set<std::wstring> dataCheckCharGI1;
	std::wstring stringGI;
	for (const auto& charTemp : characterSetBase[L'E']) {
		stringGI = L"gi" + charTemp;
		dataCheckCharGI1.insert(stringGI);
	}
	for (const auto& charTemp : characterSetBase[L'F']) {
		stringGI = L"g" + charTemp;
		dataCheckCharGI1.insert(stringGI);
	}
	return dataCheckCharGI1;
}

std::unordered_set<wchar_t> Variable::createDataCheckCharGI2() {
	std::unordered_set<wchar_t> dataCheckCharGI2 = { L'a', L'e', L'o', L'u' };
	for (const auto& it : dataAddCharMiddle2) {
		dataCheckCharGI2.insert(std::tolower(it.first));
	}
	return dataCheckCharGI2;
}

std::map<std::wstring, std::map<std::wstring, std::wstring>> Variable::createDataCheckEnd() {
	std::map<std::wstring, std::map<std::wstring, std::wstring>> dataCheckEnd;
	std::map<std::wstring, std::map<std::wstring, std::wstring>> mapDataEndBaseTotal = mapDataEndBase;

	for (const auto& keyTempPair : mapDataEndBaseTotal) {
		const std::wstring& keyTemp = keyTempPair.first;
		const std::map<std::wstring, std::wstring>& mapOnsetBase = keyTempPair.second;
		std::map<std::wstring, std::wstring> mapOnsetTemp;

		for (const auto& onsetPair : mapOnsetBase) {
			std::wstring onset1 = onsetPair.first;
			std::wstring onset2 = onsetPair.second;

			int length = static_cast<int>(onset1.size());
			std::vector<std::wstring> listMask;

			if (length == 1) {
				listMask = { L"00", L"11" };
			}
			else if (length == 2) {
				listMask = { L"00", L"11", L"10", L"01" };
			}

			for (const std::wstring& mask : listMask) {
				std::wstring onset1Temp, onset2Temp;

				for (size_t i = 0; i < onset1.size(); i++) {
					if (mask[i] == L'0') {
						onset1Temp.push_back(onset1[i]);
					}
					else {
						onset1Temp.push_back(towupper(onset1[i]));
					}
				}

				for (size_t i = 0; i < onset2.size(); i++) {
					if (mask[i] == L'0') {
						onset2Temp.push_back(onset2[i]);
					}
					else {
						onset2Temp.push_back(towupper(onset2[i]));
					}
				}
				mapOnsetTemp[onset1Temp] = onset2Temp;
			}
		}
		dataCheckEnd[keyTemp] = mapOnsetTemp;
	}
	return dataCheckEnd;
}

std::unordered_set<std::wstring> Variable::createDataRemoveCharMiddle1()
{
	std::unordered_set<std::wstring> dataRemoveCharMiddle1;
	for (auto& kv : characterSetBase)
	{
		const std::vector<std::wstring>& vec = kv.second;
		for (int i = 1; i < static_cast<int>(vec.size()); i++)
		{
			dataRemoveCharMiddle1.insert(vec[i]);
		}
	}
	return dataRemoveCharMiddle1;
}

std::map<std::wstring, std::vector<std::wstring>> Variable::createDataCheckPosTone()
{
	return createDataCheckMiddle2();
}

std::unordered_set<wchar_t> Variable::createDataAddNewWord()
{
	std::unordered_set<wchar_t> dataAddNewWord;

	std::vector<wchar_t> listCharTemp = { L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y', L'z', L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9'};
	for (auto& charTemp : listCharTemp)
	{
		dataAddNewWord.insert(tolower(charTemp));
		dataAddNewWord.insert(towupper(charTemp));
	}

	for (auto& stringTemp : inputMethodBase)
	{
		for (auto& charTemp : stringTemp)
		{
			if (mapDataQuickVowel.find(charTemp) != mapDataQuickVowel.end()) {
				dataAddNewWord.insert(charTemp);
				dataAddNewWord.insert(mapDataQuickVowel[charTemp]);
			}
			else {
				dataAddNewWord.insert(tolower(charTemp));
				dataAddNewWord.insert(towupper(charTemp));
			}

		}
	}

	if (modeTeenCode) {
		for (const std::wstring& item : dataOnsetStartTeenCode) {
			for (wchar_t charTemp : item) {
				dataAddNewWord.insert(tolower(charTemp));
				dataAddNewWord.insert(towupper(charTemp));
			}
		}
	}

	return dataAddNewWord;
}

std::unordered_set<wchar_t> Variable::createDataAddCharSpace()
{
	std::unordered_set<wchar_t> dataAddCharSpace;

	std::vector<wchar_t> listCharTemp = { L'a', L'b', L'c', L'd', L'e', L'g', L'h', L'i', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r', L's', L't', L'u', L'v', L'x', L'y' };
	for (auto& charTemp : listCharTemp)
	{
		dataAddCharSpace.insert(tolower(charTemp));
		dataAddCharSpace.insert(towupper(charTemp));
	}

	for (const auto& stringTemp : inputMethodBase) {
		for (wchar_t charTemp : stringTemp) {
			dataAddCharSpace.insert(tolower(charTemp));
			dataAddCharSpace.insert(towupper(charTemp));
		}
	}

	for (const auto& pair : dataAddCharMiddle2) {
		wchar_t charTemp = pair.first;
		dataAddCharSpace.insert(charTemp);
		dataAddCharSpace.insert(towupper(charTemp));
	}

	if (modeTeenCode) {
		for (const std::wstring& item : dataOnsetStartTeenCode) {
			for (wchar_t charTemp : item) {
				dataAddCharSpace.insert(tolower(charTemp));
				dataAddCharSpace.insert(towupper(charTemp));
			}
		}
	}

	return dataAddCharSpace;
}

std::unordered_set<wchar_t> Variable::createDataCheckModeRestore()
{
	std::unordered_set<wchar_t> dataCheckModeRestore;
	std::vector<wchar_t> listCharTemp = { L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y', L'z' };
	for (auto& charTemp : listCharTemp)
	{
		dataCheckModeRestore.insert(tolower(charTemp));
		dataCheckModeRestore.insert(towupper(charTemp));
	}

	for (auto& stringTemp : inputMethodBase)
	{
		for (auto& charTemp : stringTemp)
		{
			dataCheckModeRestore.insert(tolower(charTemp));
			dataCheckModeRestore.insert(towupper(charTemp));
		}
	}
	
	return dataCheckModeRestore;
}

std::map<std::wstring, std::wstring> Variable::createDataChangeCaseLower()
{
	std::map<std::wstring, std::wstring> dataChangeCaseLower;
	for (int pos = 0; pos < 13; pos++) {
		std::vector<std::wstring> listCharLower = characterSetBase[L'A' + pos];
		std::vector<std::wstring> listCharUpper = characterSetBase[L'N' + pos];
		for (int i = 0; i < listCharLower.size(); i++) {
			std::wstring charLower = listCharLower[i];
			std::wstring charUpper = listCharUpper[i];
			dataChangeCaseLower[charUpper] = charLower;
		}
	}
	return dataChangeCaseLower;
}

std::map<std::wstring, std::wstring> Variable::createDataChangeCaseUpper()
{
	std::map<std::wstring, std::wstring> dataChangeCaseUpper;
	for (int pos = 0; pos < 13; pos++) {
		std::vector<std::wstring> listCharLower = characterSetBase[L'A' + pos];
		std::vector<std::wstring> listCharUpper = characterSetBase[L'N' + pos];
		for (int i = 0; i < listCharLower.size(); i++) {
			std::wstring charLower = listCharLower[i];
			std::wstring charUpper = listCharUpper[i];
			dataChangeCaseUpper[charLower] = charUpper;
		}
	}
	return dataChangeCaseUpper;
}

void Variable::saveDataAutoChangeLang(QString& nameApp)
{
	AccountManager* accountManager = AccountManager::getInstance();

	QStringList list;
	for (const std::wstring& item : dataAutoChangeLang) {
		QString lower = QString::fromStdWString(item).toLower();
		list.append(lower);
	}

	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/DataAutoChangeLang");
	settings.setValue(nameApp, list);
	settings.endGroup();
}

bool Variable::loadSettingsWindow()
{
	nameCurrentWindow = getActiveWindowAppName();
	if (nameCurrentWindow != namePreviousWindow) {
		TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
		SnippetEditor* snippetEditor = SnippetEditor::getInstance();
		TypeWord& typeWord = TypeWord::getInstance();
		AccountManager* accountManager = AccountManager::getInstance();
		LangRegistryWatcher& langRegistryWatcher = LangRegistryWatcher::getInstance();
		CharsetRegistryWatcher& charsetRegistryWatcher = CharsetRegistryWatcher::getInstance();

		typeWord.numChangeCase = 0;
		bool modeLangVietGlobalDefault = listAppLangVietGlobal.contains(nameCurrentWindow.toLower()) ? true : false;
		bool modeClipboardDefault = listAppUseClipboard.contains(nameCurrentWindow.toLower()) ? true : false;
		bool modeFixAutoSuggestDefault = listAppFixAutoSuggest.contains(nameCurrentWindow.toLower()) ? true : false;
		QString nameTaskAIDefault = taskAIDatabase.listNameTaskAI.value(0, QString());

		QSettings settings(APP_NAME, "AccountManager");
		settings.beginGroup(accountManager->currentAccount + "/ConfigUi/" + nameCurrentWindow);

		if (langRegistryWatcher.dataLangWatcher.contains(nameCurrentWindow)) {
			flagLangVietGlobal = langRegistryWatcher.dataLangWatcher[nameCurrentWindow];
		}
		else {
			flagLangVietGlobal = settings.value("flagLangVietGlobal", modeLangVietGlobalDefault).toBool();
		}

		if (charsetRegistryWatcher.dataCharsetWatcher.contains(nameCurrentWindow)) {
			characterSet = charsetRegistryWatcher.dataCharsetWatcher[nameCurrentWindow].toStdWString();
		}
		else {
			characterSet = settings.value("characterSet", QString::fromStdWString(CHARACTERSET)).toString().toStdWString();
		}

		nameTaskAI = settings.value("nameTaskAI", nameTaskAIDefault).toString();
		nameSnippetString = settings.value("nameSnippetString", NAMESNIPPETSTRING).toString();
		nameSnippetWords = settings.value("nameSnippetWords", NAMESNIPPETWORDS).toString();
		modeUseDynamic = settings.value("modeUseDynamic", MODEUSEDYNAMIC).toBool();
		modeTypeSimple = settings.value("modeTypeSimple", MODETYPESIMPLE).toBool();
		modeClipboard = settings.value("modeClipboard", modeClipboardDefault).toBool();
		modeFixAutoSuggest = settings.value("modeFixAutoSuggest", modeFixAutoSuggestDefault).toBool();
		modeCheckCase = settings.value("modeCheckCase", MODECHECKCASE).toBool();
		modeTeenCode = settings.value("modeTeenCode", MODETEENCODE).toBool();
		modeUseLeftRight = settings.value("modeUseLeftRight", MODEUSELEFTRIGHT).toBool();
		settings.endGroup();

		if (mapCharacterSetBase.find(characterSet) == mapCharacterSetBase.end()) {
			characterSet = CHARACTERSET;
		}

		if (!taskAIDatabase.listNameTaskAI.contains(nameTaskAI)) {
			nameTaskAI = nameTaskAIDefault;
		}
		snippetEditor->loadSnippetForApp(nameSnippetString, nameSnippetWords);
		if (namePreviousWindow != "") {
			saveDataAutoChangeLang(namePreviousWindow);
		}
		loadDataAutoChangeLang(nameCurrentWindow);

		update();
		namePreviousWindow = nameCurrentWindow;
		return true;
	}
	if ((nameCurrentWindow == "acad") && (getActiveWindowTitle() == "Enhanced Attribute Editor")) {
		modeClipboard = true;
	}
	return false;
}

void Variable::addKeyInputMethod(const QString& stringRaw, int indexChar, std::vector<std::wstring>& inputMethodBase)
{
	Variable& variable = Variable::getInstance();
	QSet<QChar> stringSeen;
	std::vector<int> listIndexSeen;

	if (indexChar == 0) {
		listIndexSeen = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
	}
	if ((indexChar >= 1) && (indexChar <= 5)) {
		listIndexSeen = { 0, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
	}
	if ((indexChar >= 7) && (indexChar <= 12)) {
		listIndexSeen = { 0, 1, 2, 3, 4, 5, 6, 13, 14 };
	}
	if (indexChar == 14) {
		listIndexSeen = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	}

	for (int i : listIndexSeen) {
		const QString inputString = QString::fromStdWString(inputMethodBase[i]);
		for (QChar character : inputString)
			stringSeen.insert(character);
	}

	const QString stringTemp = stringRaw.toUpper();
	QString stringResult;
	for (QChar character : stringTemp) {
		if (dataValidateKeyToneDiac.contains(character) && !stringSeen.contains(character)) {
			stringResult += std::wstring(1, character.unicode());
			stringSeen.insert(character);
		}
	}

	if (stringResult.size() >= 5) {
		stringResult = stringResult.left(5);
	}
	inputMethodBase[indexChar] = stringResult.toStdWString();
}
