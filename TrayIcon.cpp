#include "TrayIcon.h"
#include "SigmaLib.h"
#include "Util.h"
#include "Variable.h"
#include "CustomInputMethod.h"
#include "CustomConfirmBox.h"
#include "TaskAIEditor.h"
#include "ShortcutKeyEditor.h"
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

	actionGeneralConfig = addActionToMenu("Cấu hình chung", ":/iconGeneralConfig.png", this, SLOT(onGeneralConfig()));
	actionConfigInput = addActionToMenu("Tùy chỉnh kiểu gõ tích hợp", ":/iconConfigInput.png", this, SLOT(onConfigInput()));
	actionConfigTaskAI = addActionToMenu("Trình chỉnh sửa tác vụ AI", ":/iconTaskAI.png", this, SLOT(onConfigTaskAI()));
	actionShortcutKeyEditor = addActionToMenu("Trình quản lý phím tắt", ":/iconShortcutKey.png", this, SLOT(onShortcutKeyEditor()));
	actionSnippetEditor = addActionToMenu("Trình quản lý gõ tắt", ":/iconSnippetEdittor.png", this, SLOT(onConfigSnippetEditor()));
	actionResetConfigUi = addActionToMenu("Xóa thiết lập đã lưu", ":/iconReset.png", this, SLOT(onResetConfigUi()));
	actionFeedback = addActionToMenu("Đóng góp ý kiến", ":/iconFeedback.png", this, SLOT(onFeedback()));
	actionSigmaGroup = addActionToMenu("Tham gia cộng đồng", ":/iconSigmaGroup.png", this, SLOT(onSigmaGroup()));
	actionHelp = addActionToMenu("Hướng dẫn sử dụng", ":/iconHelp.png", this, SLOT(onHelp()));
	actionQuit = addActionToMenu("Đóng ứng dụng Sigma", ":/iconQuit.png", this, SLOT(onQuit()));

	connect(menu, &QMenu::aboutToShow, this, &TrayIcon::updateMenuShortcutText);
	connect(tray, &QSystemTrayIcon::activated, this, &TrayIcon::onTrayIconActivated);
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

QAction* TrayIcon::addActionToMenu(const QString& text, const QString& iconPath, const QObject* receiver, const char* slot)
{
	QAction* action = new QAction(QIcon(iconPath), text, const_cast<QObject*>(receiver));
	connect(action, SIGNAL(triggered()), receiver, slot);
	menu->addAction(action);
	menu->addSeparator();
	return action;
}

void TrayIcon::updateMenuShortcutText() {
	ShortcutKeyEditor* shortcutKeyEditor = ShortcutKeyEditor::getInstance();

	auto updateAction = [&](QAction* action, const QString& title, const QString& keyName) {
		QString text = title;
		QString shortcutText = shortcutKeyEditor->getShortcutKey(keyName);
		if (!shortcutText.isEmpty()) {
			text += "\t" + shortcutText;
		}
		action->setText(text);
		};

	updateAction(actionGeneralConfig, "Cấu hình chung", "Gọi bảng cấu hình chung");
	updateAction(actionConfigInput, "Tùy chỉnh kiểu gõ tích hợp", "Gọi bảng tùy chỉnh kiểu gõ tích hợp");
	updateAction(actionConfigTaskAI, "Trình chỉnh sửa tác vụ AI", "Gọi bảng trình chỉnh sửa tác vụ AI");
	updateAction(actionShortcutKeyEditor, "Trình quản lý phím tắt", "Gọi bảng trình quản lý phím tắt");
	updateAction(actionSnippetEditor, "Trình quản lý gõ tắt", "Gọi bảng trình quản lý gõ tắt");
	updateAction(actionResetConfigUi, "Xóa thiết lập đã lưu", "Gọi bảng xóa các tiết lập đã lưu cho từng ứng dụng");
	updateAction(actionFeedback, "Đóng góp ý kiến", "Gọi bảng đóng góp ý kiến");
	updateAction(actionSigmaGroup, "Tham gia cộng đồng", "Tham gia cộng đồng");
	updateAction(actionHelp, "Hướng dẫn sử dụng", "Gọi bảng hướng dẫn sử dụng");
	updateAction(actionQuit, "Đóng ứng dụng Sigma", "Đóng ứng dụng Sigma");
}

void TrayIcon::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger) {
		if (menu && tray) {
			QPoint pos = QCursor::pos();
			int menuHeight = menu->sizeHint().height();
			pos.setY(pos.y() - menuHeight);
			menu->popup(pos);
		}
	}
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

void TrayIcon::onShortcutKeyEditor()
{
	ShortcutKeyEditor::showWindow();
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

void TrayIcon::onSigmaGroup() {
	QDesktopServices::openUrl(QUrl("https://zalo.me/g/xlhxrd938"));
}

void TrayIcon::onQuit() {
	SigmaLib::getInstance().stop();
}
