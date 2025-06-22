#include "TrayIcon.h"
#include "SigmaLib.h"
#include "Util.h"
#include "Variable.h"
#include "CustomInputMethod.h"
#include "CustomConfirmBox.h"
#include "TaskAIEditor.h"
#include "SnippetEditor.h"
#include "Feedback.h"
#include "GeneralConfig.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QIcon>
#include <QMessageBox>
#include <QSettings>
#include <iostream>
#include <QFileInfo>
#include <QProcess>
#include <QPointer>
#include <QTemporaryFile>
#include <QDir>

TrayIcon::TrayIcon(QObject* parent)
	: QObject(parent) {
	Variable& variable = Variable::getInstance();

	tray = new QSystemTrayIcon(QIcon(":/icon.png"), this);
	tray->setToolTip(Variable::getInstance().appNameFull);

	menu = new QMenu();

	addActionToMenu("Cấu hình chung", "Shift → C", ":/iconGeneralConfig.png", this, SLOT(onGeneralConfig()));
	addActionToMenu("Tùy chỉnh kiểu gõ tích hợp", "Shift → K", ":/iconConfigInput.png", this, SLOT(onConfigInput()));
	addActionToMenu("Trình chỉnh sửa tác vụ AI", "Shift → A", ":/iconTaskAI.png", this, SLOT(onConfigTaskAI()));
	addActionToMenu("Trình quản lý gõ tắt", "Shift → T", ":/iconSnippetEdittor.png", this, SLOT(onConfigSnippetEditor()));
	addActionToMenu("Xóa thiết lập đã lưu", "Shift → X", ":/iconReset.png", this, SLOT(onResetConfigUi()));
	addActionToMenu("Đóng góp ý kiến", "Shift → Y", ":/iconFeedback.png", this, SLOT(onFeedback()));
	addActionToMenu("Hướng dẫn sử dụng", "Shift → S", ":/iconHelp.png", this, SLOT(onHelp()));
	addActionToMenu("Đóng ứng dụng Sigma", "", ":/iconQuit.png", this, SLOT(onQuit()));

	tray->setContextMenu(menu);
}

TrayIcon& TrayIcon::getInstance() {
	static TrayIcon trayInstance;
	return trayInstance;
}

void TrayIcon::showWindow() {
	getInstance().doShow();
}

void TrayIcon::hideWindow() {
	TrayIcon& trayIcon = TrayIcon::getInstance();
	if (trayIcon.confirmResetConfigUi) {
		trayIcon.confirmResetConfigUi->closeWindow();
		trayIcon.confirmResetConfigUi.clear();
	}
	getInstance().doHide();
}

void TrayIcon::closeWindow() {
	TrayIcon& trayIcon = TrayIcon::getInstance();
	if (trayIcon.confirmResetConfigUi) {
		trayIcon.confirmResetConfigUi->closeWindow();
		trayIcon.confirmResetConfigUi.clear();
	}
	trayIcon.doClose();
}

void TrayIcon::doShow() {
	if (tray && !tray->isVisible()) {
		tray->show();
	}
}

void TrayIcon::doHide() {
	if (tray && tray->isVisible()) {
		tray->hide();
	}
}

void TrayIcon::doClose() {
	if (tray) {
		tray->hide();
		tray->setContextMenu(nullptr);

		delete menu;
		menu = nullptr;

		tray->deleteLater();
		tray = nullptr;
	}
}

QAction* TrayIcon::addActionToMenu(const QString& text, const QString& shortcut, const QString& iconPath, const QObject* receiver, const char* slot)
{
	QString textDisplay = text + "\t" + shortcut;
	QAction* action = new QAction(QIcon(iconPath), textDisplay, const_cast<QObject*>(receiver));
	connect(action, SIGNAL(triggered()), receiver, slot);
	menu->addAction(action);
	menu->addSeparator();
	return action;
}

void TrayIcon::onGeneralConfig() {
	GeneralConfig::showWindow();
}

void TrayIcon::onConfigInput() {
	CustomInputMethod::showWindow();
}

void TrayIcon::onConfigTaskAI() {
	TaskAIEditor::showWindow();
}

void TrayIcon::onConfigSnippetEditor()
{
	SnippetEditor::showWindow();
}

void TrayIcon::onResetConfigUi()
{
	if (!confirmResetConfigUi) {
		confirmResetConfigUi = new CustomConfirmBox("Xác nhận", "Bạn có chắc muốn xóa thiết lập của Sigma đã lưu cho từng cửa sổ ứng dụng?", nullptr);
	}
	confirmResetConfigUi->showWindow();
	if (confirmResetConfigUi) {
		Variable& variable = Variable::getInstance();

		int confirm = confirmResetConfigUi->confirm;
		if (confirm == 1) {
			QSettings settings(variable.appName, "ConfigUi");
			settings.clear();
		}
	}
}

void TrayIcon::onFeedback() {
	Feedback::showWindow();
}

void TrayIcon::onHelp() {
	QFile resourceFile(":/Sigma User Guide.html");
	if (!resourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(nullptr, "Lỗi", "Không thể mở tài nguyên hướng dẫn.");
		return;
	}

	QByteArray htmlData = resourceFile.readAll();
	resourceFile.close();

	QString tempPath = QDir::tempPath() + "/SigmaHelp.html";
	QFile tempFile(tempPath);
	if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(nullptr, "Lỗi", "Không thể tạo file tạm.");
		return;
	}

	tempFile.write(htmlData);
	tempFile.close();

	QDesktopServices::openUrl(QUrl::fromLocalFile(tempPath));
}

void TrayIcon::onQuit() {
	SigmaLib::getInstance().stop();
}
