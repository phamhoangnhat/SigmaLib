#include "TaskAI.h"
#include "Clipboard.h"
#include "Variable.h"
#include "NoticeUi.h"
#include "TaskAIEditor.h"
#include "TaskAIResult.h"
#include "CustomMessageBox.h"
#include "TaskAIDatabase.h"

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
	listKeyAPI = {
		//"KEY API GEMINI 1",
		//"KEY API GEMINI 2",
		//"KEY API GEMINI 3",
		//...
	};
}

TaskAI::~TaskAI() {
}

void TaskAI::run(QString keyTaskAI) {
	Clipboard& clipboard = Clipboard::getInstance();
	Variable& variable = Variable::getInstance();
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();

	if (popup) {
		popup->hide();
	}
	interrupted = false;

	if (!taskAIDatabase.dataTaskAI.contains(keyTaskAI) || flagIsSending) {
		return;
	}
	flagIsSending = true;

	QPair<QString, QString> dataTemp = taskAIDatabase.dataTaskAI[keyTaskAI];
	QString nameTaskAI = dataTemp.first;
	QString promptTaskAI = dataTemp.second;
	QString input;

	TaskAIResult* taskAIResult = TaskAIResult::getInstance();
	QString windowTitle = getActiveWindowTitle();
	if ((variable.nameCurrentWindow == "Sigma") && (windowTitle == "Xử lý tác vụ AI")) {
		input = taskAIResult->taskAIResultEditor->textCursor().selectedText();
		flagInWindow = true;
	}
	else {
		clipboard.flagUpdateBaseClipboard = false;
		clipboard.flagBaseClipboard = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		clipboard.simulateCopy();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		input = QString::fromStdWString(clipboard.getClipboardText()).trimmed();
		flagInWindow = false;
	}

	QString prompt = promptTaskAI;
	prompt.replace("{text}", input);

	sendRequest(prompt);
	showNotice(nameTaskAI);
}

void TaskAI::closeWindow() {
	if (instance) {
		if (popup) {
			popup->closeWindow();
			popup.clear();
		}
		delete instance;
		instance = nullptr;
	}
}

void TaskAI::sendRequest(const QString& prompt) {
	Clipboard& clipboard = Clipboard::getInstance();
	Variable& variable = Variable::getInstance();

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
		QApplication::instance()->removeEventFilter(this);

		Clipboard& clipboard = Clipboard::getInstance();
		if (interrupted) {
			if (!popup) {
				popup = new CustomMessageBox("Lỗi", nullptr);
			}
			popup->setMessage("Bạn đã hủy tác vụ!");
			popup->showWindow();

			reply->abort();
			reply->deleteLater();

			flagIsSending = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			clipboard.setBaseClipboard();
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
						clipboard.sendUnicodeText(result.toStdWString());
					}
				}
			}
		}
		else {
			if (!popup) {
				popup = new CustomMessageBox("Lỗi", nullptr);
			}
			popup->setMessage("Kết nối tới máy chủ thất bại.\nHãy kiểm tra Internet và thử lại.");
			popup->showWindow();
		}

		flagIsSending = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		clipboard.setBaseClipboard();
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
