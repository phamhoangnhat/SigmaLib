// TaskAIDatabase.cpp

#include "TaskAIDatabase.h"
#include "Variable.h"

#include <QSettings>
#include <algorithm>

TaskAIDatabase& TaskAIDatabase::getInstance() {
	static TaskAIDatabase instance;
	return instance;
}

TaskAIDatabase::TaskAIDatabase() {
	loadDataTaskAI();
}

void TaskAIDatabase::loadDataTaskAI()
{
	Variable& variable = Variable::getInstance();
	QSettings settings(variable.appName, "TaskAIDatabase");
	const QStringList allKeys = settings.allKeys();

	QSet<QString> seenKeys;
	for (const QString& rawName : allKeys) {
		QString trimmed = rawName.trimmed();
		QString upperName = trimmed.toUpper();
		QString content = settings.value(rawName).toString();

		if (!isValidTaskName(trimmed, dataTaskAI, QString()) || !isValidTaskContent(content))
			continue;

		if (seenKeys.contains(upperName))
			continue;

		seenKeys.insert(upperName);
		dataTaskAI[upperName] = qMakePair(trimmed, content);
	}

	addDataTaskAIDefault(dataTaskAI);
	listNameTaskAI = createListNameTaskAI(dataTaskAI);
}

void TaskAIDatabase::addDataTaskAIDefault(QMap<QString, QPair<QString, QString>>& dataTaskAI)
{
	if (dataTaskAI.isEmpty()) {
		dataTaskAI["06. EMAIL LỊCH SỰ"] = qMakePair("06. Email lịch sự", "Viết lại đoạn sau thành một email với văn phong lịch sự, trang trọng : \n{text}\nChỉ trả về nội dung email, không thêm chú thích hoặc giải thích.");
		dataTaskAI["07. VĂN PHONG RÕ RÀNG"] = qMakePair("07. Văn phong rõ ràng", "Viết lại đoạn sau rõ ràng, tự nhiên hơn:\n{text}\nChỉ trả về kết quả, không thêm lời giải thích.");
		dataTaskAI["08. TRIỂN KHAI CHI TIẾT"] = qMakePair("08. Triển khai chi tiết", "Triển khai ý tưởng đoạn sau chi tiết hơn:\n{text}\nChỉ trả về phần nội dung đã triển khai, không thêm gì khác.");
		dataTaskAI["09. HÓM HỈNH HÀI HƯỚC"] = qMakePair("09. Hóm hỉnh hài hước", "Chuyển đoạn sau theo phong cách hóm hỉnh, hài hước và thông minh, pha trò dí dỏm nhưng không mất đi tính lịch sự:\n{text}\nChỉ trả về nội dung đã chỉnh sửa, không thêm lời giải thích hay mô tả.");
		dataTaskAI["10. VĂN PHONG GEN Z"] = qMakePair("10. Văn phong Gen Z", "Viết lại đoạn sau theo phong cách Gen Z: ngắn gọn, vui nhộn, thông minh, có thể pha chút meme, tiếng lóng, icon hoặc từ trending. Vẫn giữ ý chính, không lố lăng, không mất lịch sự, không hashtag:\n{text}\nChỉ trả về kết quả đã chuyển phong cách, không thêm giải thích hay chú thích.");
		dataTaskAI["11. VIẾT THƠ"] = qMakePair("11. Viết thơ", "Chuyển đoạn nội dung văn bản sau thành đoạn thơ lãng mạn, ngôn từ bay bổng:\n{text}\nChỉ trả về bài thơ, không thêm chú thích hay lời giải thích, tiêu đề.");
	}

	listNameDefaultTaskAI = QSet<QString>({
	"01. KIỂM TRA CHÍNH TẢ TIẾNG VIỆT",
	"02. KIỂM TRA CHÍNH TẢ TIẾNG ANH",
	"03. DỊCH SANG TIẾNG VIỆT",
	"04. DỊCH SANG TIẾNG ANH",
	"05. CHUYỂN MÃ UNICODE"
		});

	dataTaskAI.remove("01. KIỂM TRA CHÍNH TẢ TIẾNG VIỆT");
	dataTaskAI["01. KIỂM TRA CHÍNH TẢ TIẾNG VIỆT"] = qMakePair(
		"01. Kiểm tra chính tả tiếng Việt",
		"Sửa lỗi chính tả và ngữ pháp trong đoạn sau:\n{text}\n"
		"Chỉ trả về kết quả đã sửa, không thêm chú thích.\n"
		"Giữ nguyên cách viết hoa (UPPERCASE hoặc Capitalized) nếu không chắc chắn đó là lỗi.\n"
		"Không viết lại toàn bộ câu trừ khi cần thiết.\n"
		"Các từ nước ngoài thì không dịch sang tiếng Việt, chỉ sửa chính tả theo ngôn ngữ gốc nếu sai."
	);

	dataTaskAI.remove("02. KIỂM TRA CHÍNH TẢ TIẾNG ANH");
	dataTaskAI["02. KIỂM TRA CHÍNH TẢ TIẾNG ANH"] = qMakePair(
		"02. Kiểm tra chính tả tiếng Anh",
		"Correct spelling and grammar in the following passage:\n{text}\n"
		"Return only the corrected result, without any comments.\n"
		"Preserve the original capitalization (uppercase, title case) of words unless clearly incorrect.\n"
		"Do not reformat or paraphrase sentences unnecessarily."
	);

	dataTaskAI.remove("03. DỊCH SANG TIẾNG VIỆT");
	dataTaskAI["03. DỊCH SANG TIẾNG VIỆT"] = qMakePair(
		"03. Dịch sang tiếng Việt",
		"Dịch đoạn sau sang tiếng Việt:\n{text}\n"
		"Chỉ trả về bản dịch, không thêm giải thích hay chú thích nào.\n"
		"Giữ nguyên các thuật ngữ chuyên ngành, viết tắt, tên riêng hoặc cụm từ tiếng Anh không thông dụng. "
		"Chỉ sửa chính tả các thuật ngữ này nếu chúng sai chính tả.\n"
		"Giữ nguyên định dạng, viết hoa và cấu trúc của văn bản gốc nếu có."
	);

	dataTaskAI.remove("04. DỊCH SANG TIẾNG ANH");
	dataTaskAI["04. DỊCH SANG TIẾNG ANH"] = qMakePair(
		"04. Dịch sang tiếng Anh",
		"Translate the following text into English:\n{text}\n"
		"Return only the translated result. Do not add comments or explanations.\n"
		"Preserve the original formatting and capitalization.\n"
		"Do not translate technical terms, proper names, or acronyms unless their English equivalents are well known."
	);

	dataTaskAI.remove("05. CHUYỂN MÃ UNICODE");
	dataTaskAI["05. CHUYỂN MÃ UNICODE"] = qMakePair(
		"05. Chuyển mã Unicode",
		"Chuyển đoạn văn bản sau sang mã Unicode chuẩn. Văn bản có thể chứa các mã tiếng Việt hỗn hợp như Unicode, TCVN3, VNI-Windows:\n{text}\n"
		"Chỉ trả về đoạn văn đã chuyển sang Unicode, không thêm lời giải thích, không chú thích.\n"
		"Giữ nguyên nội dung, định dạng, khoảng trắng và cách dòng gốc. Không sửa lỗi chính tả hoặc ngữ pháp."
	);
}

void TaskAIDatabase::saveDataTaskAI() {
	Variable& variable = Variable::getInstance();
	QSettings settings(variable.appName, "TaskAIDatabase");
	settings.clear();

	for (const auto& pair : dataTaskAI.values()) {
		const QString& originalName = pair.first;
		const QString& content = pair.second;
		settings.setValue(originalName, content);
	}
	listNameTaskAI = createListNameTaskAI(dataTaskAI);
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

QStringList TaskAIDatabase::createListNameTaskAI(QMap<QString, QPair<QString, QString>>& dataTaskAI) {
	QStringList listNameTaskAI;
	for (const auto& pair : dataTaskAI.values()) {
		listNameTaskAI << pair.first;
	}

	std::sort(listNameTaskAI.begin(), listNameTaskAI.end(), [](const QString& a, const QString& b) {
		return a.toUpper() < b.toUpper();
		});

	return listNameTaskAI;
}
