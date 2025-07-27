#include "TaskAI.h"
#include "Clipboard.h"
#include "Variable.h"
#include "NoticeUi.h"
#include "TaskAIEditor.h"
#include "TaskAIResult.h"
#include "CustomMessageBox.h"
#include "MessageApiKeyBox.h"
#include "KeyAPIManage.h"
#include "TypeWord.h"

#include <QApplication>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QRandomGenerator>
#include <thread>
#include <chrono>
#include <iostream>
#include <Util.h>

TaskAI* instance = nullptr;

TaskAI& TaskAI::getInstance() {
	if (!instance) {
		instance = new TaskAI(QApplication::instance());
	}
	return *instance;
}

TaskAI::TaskAI(QObject* parent) : QObject(parent) {
}

TaskAI::~TaskAI() {
}

void TaskAI::run(QPair<QString, QString> dataAI, QString inputBase, bool flagShowNotice) {
	Clipboard& clipboard = Clipboard::getInstance();
	Variable& variable = Variable::getInstance();
	TypeWord& typeWord = TypeWord::getInstance();

	if (popup1) {
		popup1->hide();
	}
	if (popup2) {
		popup2->hide();
	}
	interrupted = false;
	flagIsSending = true;

	QString nameTaskAI = dataAI.first;
	QString promptTaskAI = dataAI.second;
	QString input;
	QString stringTemp = inputBase.trimmed();
	int numSpace = 0;
	
	if(!stringTemp.isEmpty()) {
		input = stringTemp;
		numSpace = inputBase.length() - stringTemp.length();
	}

	TaskAIResult* taskAIResult = TaskAIResult::getInstance();
	QString windowTitle = getActiveWindowTitle();
	if ((variable.nameCurrentWindow == "Sigma") && (windowTitle == "Xử lý tác vụ AI")) {
		if (input.isEmpty()) {
			input = taskAIResult->taskAIResultEditor->textCursor().selectedText();
		}
		flagInWindow = true;
	}
	else {
		if (input.isEmpty()) {
			variable.flagSendingKey = true;
			clipboard.simulateCopy();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			input = QString::fromStdWString(clipboard.getClipboardText()).trimmed();
		}
		flagInWindow = false;
	}

	if (input.isEmpty()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		clipboard.setBaseClipboard();
		flagOpenWindow = false;
		flagIsSending = false;
		variable.flagSendingKey = false;
		typeWord.reset(true);
		return;
	}

	QString prompt = promptTaskAI;
	prompt.replace("{text}", input);

	sendRequest(prompt, inputBase, numSpace);
	if (flagShowNotice) {
		showNotice(nameTaskAI);
	}
}

void TaskAI::closeWindow() {
	if (instance) {
		if (popup1) {
			popup1->closeWindow();
			popup1.clear();
		}
		if (popup2) {
			popup2->closeWindow();
			popup2.clear();
		}
		delete instance;
		instance = nullptr;
	}
}

void TaskAI::sendRequest(const QString& prompt, QString inputBase, int numSpace) {
	KeyAPIManage* keyAPIManage = KeyAPIManage::getInstance();
	Clipboard& clipboard = Clipboard::getInstance();
	Variable& variable = Variable::getInstance();
	QStringList listKeyAPI = keyAPIManage->listKeyAPI;

	int randomIndex = QRandomGenerator::global()->bounded(listKeyAPI.size());
	QString randomKey = listKeyAPI.at(randomIndex);
	QUrl url(QString("https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=%1").arg(randomKey));
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QJsonObject part;
	part["text"] = prompt;

	QJsonObject content;
	content["parts"] = QJsonArray{ part };

	QJsonObject body;
	body["contents"] = QJsonArray{ content };

	QByteArray data = QJsonDocument(body).toJson();
	QNetworkReply* reply = networkManager.post(request, data);

	QObject::connect(reply, &QNetworkReply::finished, [=]() {
		TypeWord& typeWord = TypeWord::getInstance();
		Variable& variable = Variable::getInstance();
		QApplication::instance()->removeEventFilter(this);
		Clipboard& clipboard = Clipboard::getInstance();

		if (interrupted) {
			if (!popup1) {
				popup1 = new CustomMessageBox("Lỗi", nullptr);
			}
			popup1->setMessage("Bạn đã hủy tác vụ!");
			popup1->showWindow();

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			clipboard.setBaseClipboard();
			flagOpenWindow = false;
			flagIsSending = false;
			variable.flagSendingKey = false;
			typeWord.reset(true);

			reply->abort();
			reply->deleteLater();
			return;
		}

		if (reply->error() == QNetworkReply::NoError) {
			QByteArray response = reply->readAll();
			QJsonDocument doc = QJsonDocument::fromJson(response);
			QString result = doc["candidates"].toArray()
				.first().toObject()["content"]
				.toObject()["parts"]
				.toArray().first().toObject()["text"].toString();

			result = result.trimmed();
			while (result.endsWith('\n') || result.endsWith('\r')) {
				result.chop(1);
			}
			if (result.startsWith('\"') && result.endsWith('\"') && result.length() >= 2) {
				result = result.mid(1, result.length() - 2);
			}
			result = removeMarkdownFormatting(result);

			if (!result.isEmpty()) {
				int numCharMax = 1;
				std::wstring stringTemp = result.toStdWString();
				if (variable.characterSet == L"VNI Windows") {
					stringTemp = convertCharset(stringTemp, variable.mapUniToVni, numCharMax);
					result = QString::fromStdWString(stringTemp);
				}
				if (variable.characterSet == L"TCVN3 (ABC)") {
					stringTemp = convertCharset(stringTemp, variable.mapUniToAbc, numCharMax);
					result = QString::fromStdWString(stringTemp);
				}
			}

			if (!interrupted) {
				TaskAIResult* taskAIResult = TaskAIResult::getInstance();
				if (flagInWindow) {
					taskAIResult->taskAIResultEditor->textCursor().insertText(result);
				}
				else {
					if (flagOpenWindow) {
						taskAIResult->showWindow();
						taskAIResult->setResultText(result);
					}
					else {
						variable.flagSendingKey = true;

						if(!inputBase.isEmpty()) {
							for (int i = 0; i < numSpace; i++) {
								result = L" " + result;
							}

							int numBack = inputBase.length();
							int numMin = std::min(inputBase.length(), result.length());
							int samePrefixLen = 0;
							for (int i = 0; i < inputBase.length(); ++i) {
								if ((i < numMin) && inputBase[i] == result[i]) {
									numBack--;
									samePrefixLen++;
								}
								else {
									break;
								}
							}

							if (samePrefixLen > 0) {
								result = result.mid(samePrefixLen);
							}

							if (numBack > 0) {
								std::vector<INPUT> inputs;
								for (int i = 0; i < numBack; i++) {
									typeWord.addInput(VK_BACK, inputs);
								}
								SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));

								int timeDelay = std::clamp(numBack * 10, 100, 1000);
								std::this_thread::sleep_for(std::chrono::milliseconds(timeDelay));
							}
						}
						clipboard.sendUnicodeText(result.toStdWString(), 100);
					}
				}
			}
		}
		else {
			if (!popup2) {
				popup2 = new MessageApiKeyBox(nullptr);
			}
			popup2->showWindow();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		clipboard.setBaseClipboard();
		flagOpenWindow = false;
		flagIsSending = false;
		variable.flagSendingKey = false;
		typeWord.reset(true);
		reply->deleteLater();
		}
	);
}

void TaskAI::showNotice(QString& name) {
	NoticeUi& notice = NoticeUi::getInstance();
	notice.currentFontSize = 10;
	notice.backgroundColor = QColor(34, 103, 107);
	notice.displayText = name;

	QFont font;
	font.setPointSize(notice.currentFontSize);
	QFontMetrics fm(font);
	notice.iconWidth = fm.horizontalAdvance(notice.displayText) + 100;

	NoticeUi::updateWindow();
}

QString TaskAI::removeMarkdownFormatting(const QString& input) {
	QString output = input;
	output.replace(QRegularExpression(R"(\*\*(.*?)\*\*)"), "\\1");
	output.replace(QRegularExpression(R"(\*(.*?)\*)"), "\\1");
	output.replace(QRegularExpression(R"(__([^_]+)__)"), "\\1");
	output.replace(QRegularExpression(R"(_([^_]+)_)"), "\\1");
	output.replace(QRegularExpression(R"(`([^`]+)`)"), "\\1");
	output.replace(QRegularExpression(R"(~~([^~]+)~~)"), "\\1");
	return output;
}
