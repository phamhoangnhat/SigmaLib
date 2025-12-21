// TaskAIDatabase.cpp

#include "TaskAIDatabase.h"
#include "Variable.h"
#include "AccountManager.h"

#include <QSettings>
#include <algorithm>

TaskAIDatabase& TaskAIDatabase::getInstance() {
	static TaskAIDatabase instance;
	return instance;
}

TaskAIDatabase::TaskAIDatabase() {
	dataCheckSpell = qMakePair(
		"Kiểm tra chính tả",
		"Sửa lỗi chính tả và ngữ pháp trong đoạn sau:\n{text}\n"
		"Chỉ trả về kết quả đã sửa, không thêm chú thích.\n"
		"Tự động nhận diện ngôn ngữ của văn bản và sửa theo ngôn ngữ đó.\n"
		"Giữ nguyên cách viết hoa (UPPERCASE hoặc Capitalized) nếu không chắc chắn đó là lỗi.\n"
		"Các thuật ngữ tiếng Anh không cần dịch, chỉ sửa chính tả các thuật ngữ đó nếu sai.\n"
		"Nếu không có lỗi nào, hãy trả về nguyên văn đoạn gốc."
	);

	dataNameModel =
	{
		{"Gemini 3 Flash Preview", "gemini-3-flash-preview" },
		//{"Gemini 3 Pro Preview", "gemini-3-pro-preview"},
		{"Gemini 2.5 Flash", "gemini-2.5-flash"},
		{"Gemini 2.5 Flash Lite", "gemini-2.5-flash-lite"},
		//{"Gemini 2.5 Pro", "gemini-2.5-pro"},
		{"Gemini 2.0 Flash", "gemini-2.0-flash"},
		{"Gemini 2.0 Flash Lite", "gemini-2.0-flash-lite"},
	};
	listNameModel = dataNameModel.keys();
	numModelDefault = 3;

	listShortcutAI.clear();
	listShortcutAI.append("None");
	for (int i = 1; i <= 12; ++i) {
		const QString shortcut = "Shift → F" + QString::number(i);
		listShortcutAI.append(shortcut);
	}

	loadDataTaskAI();
	listNameTaskAI = createListNameTaskAI(dataShortcutAI, dataShortcutAICheck);
}

void TaskAIDatabase::loadDataTaskAI()
{
	AccountManager* accountManager = AccountManager::getInstance();
	listNameDefaultTaskAI = QSet<QString>({});

	dataTaskAI.clear();
	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/TaskAIDatabase");
	const QStringList allKeys = settings.allKeys();
	QSet<QString> seenKeys;
	for (const QString& nameRaw : allKeys) {
		QString nameTaskAI = nameRaw.trimmed();
		QString nameUpper = nameTaskAI.toUpper();
		QString prompt = settings.value(nameRaw).toString();

		if (seenKeys.contains(nameUpper) ||
			!isValidTaskName(nameTaskAI, dataTaskAI, QString()) ||
			!isValidTaskContent(prompt))
		{
			settings.remove(nameRaw);
			continue;
		}

		seenKeys.insert(nameUpper);
		dataTaskAI[nameUpper] = qMakePair(nameTaskAI, prompt);
	}
	settings.endGroup();

	if (dataTaskAI.isEmpty()) {
		addDataTaskAIDefault(dataTaskAI, dataModelAI, dataShortcutAI, dataShortcutAICheck);
	}

	loadDataModel();
	loadDataShortcut();
	listNameTaskAI = createListNameTaskAI(dataShortcutAI, dataShortcutAICheck);
	saveDataTaskAI();
}

void TaskAIDatabase::loadDataModel()
{
	AccountManager* accountManager = AccountManager::getInstance();

	dataModelAI.clear();
	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/ModelAIDatabase");
	const QStringList allKeys = settings.allKeys();
	for (const QString& nameRaw : allKeys) {
		QString nameTaskAI = nameRaw.trimmed();
		QString nameUpper = nameTaskAI.toUpper();
		QString nameModel = settings.value(nameRaw).toString();

		if (!dataTaskAI.contains(nameUpper) || !listNameModel.contains(nameModel)) {
			settings.remove(nameRaw);
			continue;
		}

		nameTaskAI = dataTaskAI[nameUpper].first;
		dataModelAI[nameTaskAI] = nameModel;
	}
	settings.endGroup();

	for (const auto& pair : dataTaskAI.values()) {
		const QString& nameTaskAI = pair.first;
		if (!dataModelAI.contains(nameTaskAI)) {
			dataModelAI[nameTaskAI] = listNameModel[numModelDefault];
		}
	}
}

void TaskAIDatabase::loadDataShortcut()
{
	AccountManager* accountManager = AccountManager::getInstance();

	dataShortcutAICheck.clear();
	for (int i = 1; i <= 12; i++) {
		QString shortcut = listShortcutAI[i];
		dataShortcutAICheck.insert(shortcut, QString());
	}

	dataShortcutAI.clear();
	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/ShortcutAIDatabase");
	const QStringList allKeys = settings.allKeys();
	for (const QString& nameRaw : allKeys) {
		QString nameTaskAI = nameRaw.trimmed();
		const QString nameUpper = nameTaskAI.toUpper();
		const QString shortcut = settings.value(nameRaw).toString().trimmed();

		if (!dataTaskAI.contains(nameUpper)) {
			settings.remove(nameRaw);
			continue;
		}

		nameTaskAI = dataTaskAI.value(nameUpper).first;

		if (shortcut == "None") {
			dataShortcutAI.insert(nameTaskAI, "None");
			continue;
		}

		if (!dataShortcutAICheck.contains(shortcut) ||
			!dataShortcutAICheck.value(shortcut).isEmpty())
		{
			settings.remove(nameRaw);
			continue;
		}

		dataShortcutAI.insert(nameTaskAI, shortcut);
		dataShortcutAICheck[shortcut] = nameTaskAI;
	}
	settings.endGroup();

	for (const auto& pair : dataTaskAI.values()) {
		const QString& nameTaskAI = pair.first;
		if (dataShortcutAI.contains(nameTaskAI)) {
			continue;
		}

		bool assigned = false;
		for (int i = 1; i <= 12; i++) {
			QString shortcut = listShortcutAI[i];
			if (dataShortcutAICheck[shortcut].isEmpty()) {
				dataShortcutAI.insert(nameTaskAI, shortcut);
				dataShortcutAICheck[shortcut] = nameTaskAI;
				assigned = true;
				break;
			}
		}

		if (!assigned) {
			dataShortcutAI.insert(nameTaskAI, "None");
		}
	}
}

void TaskAIDatabase::addDataTaskAIDefault(
	QMap<QString, QPair<QString, QString>>& dataTaskAI,
	QMap<QString, QString>& dataModelAI,
	QMap<QString, QString>& dataShortcutAI,
	QMap<QString, QString>& dataShortcutAICheck)
{
	dataTaskAI.clear();
	QStringList listNameTaskAI;

	std::vector<QPair<QString, QString>> listDataRaw = {
		qMakePair(
			"Kiểm tra chính tả tiếng Việt",
			"Sửa lỗi chính tả và ngữ pháp trong đoạn sau:\n{text}\n"
			"Chỉ trả về kết quả đã sửa, không thêm chú thích.\n"
			"Giữ nguyên cách viết hoa (UPPERCASE hoặc Capitalized) nếu không chắc chắn đó là lỗi.\n"
			"Các thuật ngữ tiếng Anh không dịch sang tiếng Việt, chỉ sửa chính tả các thuật ngữ nếu sai. Nếu không có lỗi nào, hãy trả về nguyên văn đoạn gốc.\n"
			"Nếu không có lỗi nào, hãy trả về nguyên văn đoạn gốc"),

		qMakePair(
			"Kiểm tra chính tả tiếng Anh",
			"Correct spelling and grammar in the following English passage:\n{text}\n"
			"Return only the corrected version, without any explanations or annotations.\n"
			"Preserve the original capitalization (UPPERCASE or Title Case) unless clearly incorrect.\n"
			"If there are no errors, return the original passage exactly as is."),

		qMakePair(
			"Dịch sang tiếng Việt",
			"Dịch đoạn sau sang tiếng Việt:\n{text}\n"
			"Chỉ trả về bản dịch, không thêm giải thích hay chú thích nào.\n"
			"Giữ nguyên các thuật ngữ chuyên ngành tiếng Anh. Chỉ sửa chính tả các thuật ngữ này nếu chúng sai chính tả.\n"
			"Giữ nguyên định dạng, viết hoa và cấu trúc của văn bản gốc nếu có."),

		qMakePair(
			"Dịch sang tiếng Anh",
			"Translate the following text into English:\n{text}\n"
			"Return only the translated result. Do not add comments or explanations.\n"
			"Preserve the original formatting and capitalization."),

		qMakePair(
			"Chuyển mã phông chữ",
			"Chuyển đoạn văn bản sau sang mã Unicode chuẩn. Văn bản có thể chứa các mã tiếng Việt hỗn hợp như Unicode, TCVN3, VNI-Windows:\n{text}\n"
			"Chỉ trả về đoạn văn đã chuyển sang Unicode, không thêm lời giải thích, không chú thích.\n"
			"Giữ nguyên nội dung, định dạng, khoảng trắng và cách dòng gốc. Nếu không có lỗi nào, hãy trả về nguyên văn đoạn gốc."),

		qMakePair(
			"Email lịch sự",
			"Viết lại đoạn sau thành một email với văn phong lịch sự, trang trọng : \n{text}\n"
			"Chỉ trả về nội dung email, không thêm chú thích hoặc giải thích."),

		qMakePair(
			"Văn phong rõ ràng",
			"Viết lại đoạn sau rõ ràng, tự nhiên hơn:\n{text}\n"
			"Chỉ trả về kết quả, không thêm lời giải thích."),

		qMakePair(
			"Triển khai chi tiết",
			"Triển khai ý tưởng đoạn sau chi tiết hơn:\n{text}\n"
			"Chỉ trả về phần nội dung đã triển khai, không thêm gì khác."),

		qMakePair(
			"Trả lời câu hỏi",
			"Hãy trả lời câu hỏi sau: \n{text}\n"
			"Chỉ trả về câu trả lời, không thêm chú thích hoặc giải thích."),

		qMakePair(
			"Hóm hỉnh hài hước",
			"Chuyển đoạn sau theo phong cách hóm hỉnh, hài hước và thông minh, pha trò dí dỏm nhưng không mất đi tính lịch sự:\n{text}\n"
			"Chỉ trả về nội dung đã chỉnh sửa, không thêm lời giải thích hay mô tả."),

		qMakePair(
			"Văn phong Gen Z",
			"Viết lại đoạn sau theo phong cách Gen Z: ngắn gọn, vui nhộn, thông minh, có thể pha chút meme, tiếng lóng, icon hoặc từ trending. Vẫn giữ ý chính, không lố lăng, không mất lịch sự, không hashtag:\n{text}\n"
			"Chỉ trả về kết quả đã chuyển phong cách, không thêm giải thích hay chú thích."),

		qMakePair(
			"Viết thơ",
			"Chuyển đoạn nội dung văn bản sau thành đoạn thơ lãng mạn, ngôn từ bay bổng:\n{text}\n"
			"Chỉ trả về bài thơ, không thêm chú thích hay lời giải thích, tiêu đề."),
	};

	for (auto& dataRaw : listDataRaw) {
		dataTaskAI[dataRaw.first.toUpper()] = dataRaw;
		listNameTaskAI.append(dataRaw.first);
	}

	dataModelAI.clear();
	for (auto& nameTaskAI : listNameTaskAI) {
		dataModelAI[nameTaskAI] = listNameModel[numModelDefault];
	}

	dataShortcutAI.clear();
	dataShortcutAICheck.clear();
	for (int i = 1; i <= 12; i++) {
		QString shortcut = listShortcutAI[i];
		if (i <= listNameTaskAI.size()) {
			QString nameTaskAI = listNameTaskAI[i - 1];
			dataShortcutAI[nameTaskAI] = shortcut;
			dataShortcutAICheck[shortcut] = nameTaskAI;
		}
		else {
			break;
		}
	}
}

void TaskAIDatabase::saveDataTaskAI() {
	AccountManager* accountManager = AccountManager::getInstance();
	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/TaskAIDatabase");
	settings.remove("");

	for (const auto& pair : dataTaskAI.values()) {
		const QString& nameTaskAI = pair.first;
		const QString& prompt = pair.second;
		settings.setValue(nameTaskAI, prompt);
	}
	listNameTaskAI = createListNameTaskAI(dataShortcutAI, dataShortcutAICheck);
	settings.endGroup();

	saveDataModel();
	saveDataShortcut();
}

void TaskAIDatabase::saveDataModel()
{
	AccountManager* accountManager = AccountManager::getInstance();
	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/ModelAIDatabase");
	settings.remove("");

	for (const auto& nameTaskAI : dataModelAI.keys()) {
		const QString& nameModel = dataModelAI[nameTaskAI];
		settings.setValue(nameTaskAI, nameModel);
	}
	settings.endGroup();
}

void TaskAIDatabase::saveDataShortcut()
{
	AccountManager* accountManager = AccountManager::getInstance();
	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/ShortcutAIDatabase");
	settings.remove("");

	for (const auto& nameTaskAI : dataShortcutAI.keys()) {
		const QString& shortcut = dataShortcutAI[nameTaskAI];
		settings.setValue(nameTaskAI, shortcut);
	}
	settings.endGroup();
}

bool TaskAIDatabase::isValidTaskContent(const QString& content) {
	return content.contains("{text}");
}

bool TaskAIDatabase::isValidTaskName(const QString& name, const QMap<QString, QPair<QString, QString>>& dataTaskAI, const QString& current) {
	QString trimmed = name.trimmed();
	if (trimmed.isEmpty()) return false;

	QString upperName = trimmed.toUpper();
	QString upperCurrent = current.trimmed().toUpper();

	bool result = (upperName == upperCurrent) || (!dataTaskAI.contains(upperName));
	return result;
}

QStringList TaskAIDatabase::createListNameTaskAI(QMap<QString, QString>& dataShortcutAI, QMap<QString, QString>& dataShortcutAICheck) {
	QStringList listNameTaskAI;

	for (int i = 1; i <= 12; i++) {
		QString shortcut = listShortcutAI[i];
		if (!dataShortcutAICheck[shortcut].isEmpty()) {
			QString nameTaskAI = dataShortcutAICheck[shortcut];
			listNameTaskAI.append(nameTaskAI);
		}
	}

	for (const auto& nameTaskAI : dataShortcutAI.keys()) {
		if (!listNameTaskAI.contains(nameTaskAI)) {
			listNameTaskAI.append(nameTaskAI);
		}
	}

	return listNameTaskAI;
}
