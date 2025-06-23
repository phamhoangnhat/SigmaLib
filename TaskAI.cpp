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
		"AIzaSyBR5HZThIVgaX_lGeHUYCHgGU4z8qyYGpU",
		"AIzaSyCGdRvUsP9STjffXwEosziGkeyfRi-OLI4",
		"AIzaSyBjVKq1sL4D04crFsixhl-SR-qf9tsSYvs",
		"AIzaSyDTq25nwRJl_AdWzKoz6YLOyHnT0SMrHv4",
		"AIzaSyDWTIdDCUky1WvWxCCj-dUjFtmK20fcA10",

		"AIzaSyBAgwb-Y5lhWJYjG84uO_aW1w8bJXx-3qg",
		"AIzaSyBfHMgivHaXKFbq2CdQJ36XQAA4dI2nS_A",
		"AIzaSyAiVWU07_6HcGtj_qUeHPk-zNi2Kj5hmG8",
		"AIzaSyCV-QCic55XqIU0bOTSeh6tb5awkfInXgg",
		"AIzaSyDrb6jLSDZvyY8oOd0pkKuPK4jIvP-5pTI",

		"AIzaSyDr15OiNpTDMAQ12AvpjD2KBTw-vAwARpo",
		"AIzaSyBjfQOzbn-akMO8WC2xDiLqL91NR91E_1U",
		"AIzaSyDZSibe4jLStI1vPn-2k9hpnPh0E2yP37A",
		"AIzaSyBXfEnl6VvMPcNlDiAkwB2PtpgDPcTuLZU",
		"AIzaSyD80sR_yaaiz08sF2haW46aAR_wfG9jRs0",

		"AIzaSyAmZ2l4IHvMd7R3dIYsAUJq7K50ckoI4Bw",
		"AIzaSyDxVfqIJSUHwuuZnjreeLZhiuFdaNSoPK4",
		"AIzaSyBlXNOJYGEDoEShLwtTQUdvVf14GYlJCCk",
		"AIzaSyBhXRpRS-71x52615oqc-vQUyZcrT2mzZ4",
		"AIzaSyDlttUhAVdqw3BuoDdogvvkb0pi4P9KIwI",

		"AIzaSyBiMje7QE62FO-GhtCpUQCOUOfXIqGNEwM",
		"AIzaSyC4ZEwn496U3I88mMFex-B_K1PVL8jxyOY",
		"AIzaSyDReHlbVqmNHiikBTEqLr9jF3dgeDo0VZ4",
		"AIzaSyBXXYYR2dlg5dMRf-XI_RBTe_4WZpLggig",
		"AIzaSyBLFjBheHJQKQm1eURLl4-qn3KOtJQQCuo",

		"AIzaSyB037J61HLZDF2sAjUhBOjIROJCbCCXxOk",
		"AIzaSyBv8hNzk9uSfRtSCmY2LJ4pqiTMXqBqI30",
		"AIzaSyCP99OUKEg6h-KAChcNynRrOrt3rVF1-f4",
		"AIzaSyC-fH6o7izhgkRscqGd-pZb_Xv4NWauG0c",
		"AIzaSyAWrtDh7VHnbmh_Gq7Bacfrzyzc6Dv-WhA",

		"AIzaSyCKRdaSMDVNzD99NifhKtOKxFZRvu7EPT4",
		"AIzaSyAirM_SlfhLDs82rT6YzrOYG1M1rbxfeAA",
		"AIzaSyC19rqXxfbieevu3hh7caZwSzTf7aUuwLc",
		"AIzaSyCW18f-O3cbYGuCbSRbYiR8NqQckVNILJ0",
		"AIzaSyA78XG2asgtrXv9zrXSlABXdx7hPqTs8yQ",

		"AIzaSyC1bgcoVPhptukyHf7r7p4QA17jce4Uf30",
		"AIzaSyBMeT5GPTHmVkahlfVqtu5AuPmk8hL6eRM",
		"AIzaSyDLAHCsKrcp2by6xezmTlFMWVIFtsion90",
		"AIzaSyAPxCiWMrlnyqz0za3Wlg1DmUjRBo9mfwI",
		"AIzaSyBoFLa4AxiR6s8o24u7ypI-Pq8_DO27ecE",

		"AIzaSyBj2ti5HIwPtdG7dLMuo0h_MR1lJ91mCdA",
		"AIzaSyBFdQuH53GuNeVBgOBv7-iHq8fppXI9Nqk",
		"AIzaSyA2dOopBlumtSsPicmbRBrQgQxlabz7LMc",
		"AIzaSyBXxp2yLgTR3T4dK-Z5vsWxF3AQQWn2V84",
		"AIzaSyBcamKaAc1ornzBrowWm-FcrFUHqWf6WbI",

		"AIzaSyDqLfI2HaY1tH87kCQJqPoJx-73pscskzE",
		"AIzaSyC3L5viSAKdwxxs8dD_aMkJJPn_Gt9jlnU",
		"AIzaSyBfgNxCJRhakeCl1zUI42QztjKAta61ufw",
		"AIzaSyCy-82CS3y9gv2D0Xq4Sy5AK9U2pF7WTYA",
		"AIzaSyBDIJoXkOeDpXE-2xKSXT7-R7XwyosUnV8"
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
