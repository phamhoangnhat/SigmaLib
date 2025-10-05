#include "ConfigUi.h"
#include "Variable.h"
#include "Util.h"
#include "Listener.h"
#include "TaskAIEditor.h"
#include "TypeWord.h"
#include "TaskAIDatabase.h"
#include "SnippetEditor.h"
#include "ShortcutKeyEditor.h"
#include "AccountManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include <QSettings>
#include <QApplication>
#include <QScreen>

#include <windows.h>
#include <iostream>
#include <algorithm>
#include <psapi.h>

ConfigUi& ConfigUi::getInstance() {
	static ConfigUi instance;
	return instance;
}

void ConfigUi::showWindow(QWidget* parent) {
	getInstance().doShow(parent);
}

void ConfigUi::hideWindow() {
	getInstance().doHide();
}

void ConfigUi::closeWindow() {
	getInstance().doClose();
}

ConfigUi::ConfigUi(QWidget* parent)
	: QWidget(parent) {
	Variable& variable = Variable::getInstance();

	setWindowTitle(variable.appNameFull);
	setFixedWidth(420);
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setFocusPolicy(Qt::StrongFocus);
	setWindowIcon(QIcon(":/icon.png"));
	setWindowOpacity(0.95);

	setStyleSheet(R"(
		QWidget {
			background-color: rgba(220, 220, 220, 255);
			font-size: 12px;
			border-radius: 10px;
		}

        QLabel {
            background: transparent;
            color: #000000;
            font-weight: bold;
            border-radius: 10px;
            padding-left: 0px;
            padding-right: 0px;
            min-height: 25px;
        }

		QComboBox {
			background-color: #D0D0D0;
			color: #000;
			font-weight: bold;
			border: 1px solid #aaa;
			border-radius: 10px;
			padding-left: 10px;
			min-height: 25px;
		}

        QComboBox:hover {
            background-color: #CCCCCC;
            border: 1px solid #FFFFFF;
        }

        QComboBox::drop-down {
            border: none;
            width: 0px;
        }

        QComboBox::down-arrow {
            image: none;
        }

		QComboBox QAbstractItemView {
			background-color: #F0F0F0;
			selection-background-color: #C0C0C0;
			border: 1px solid #AAAAAA;
			padding: 5px;
			outline: 0;
			font-weight: normal;
			border-radius: 10px;
		}

		QComboBox QAbstractItemView::item {
			height: 15px;
			padding: 5px;
			border-radius: 5px;
			color: #888888;
			font-weight: normal;
		}

		QComboBox QAbstractItemView::item:selected {
			background-color: #BBBBBB;
			color: #000000;
			font-weight: bold;
		}

		QScrollBar:vertical {
			border: none;
			background: #E0E0E0;
			width: 10px;
			margin: 2px 0;
			border-radius: 5px;
		}

		QScrollBar::handle:vertical {
			background: #A0A0A0;
			min-height: 20px;
			border-radius: 5px;
		}

		QScrollBar::handle:vertical:hover {
			background: #888888;
		}

		QScrollBar::add-line:vertical,
		QScrollBar::sub-line:vertical {
			height: 0px;
			subcontrol-origin: margin;
		}

		QScrollBar::add-page:vertical,
		QScrollBar::sub-page:vertical {
			background: none;
		}

		QScrollBar:horizontal {
			border: none;
			background: #E0E0E0;
			height: 10px;
			margin: 0 2px;
			border-radius: 5px;
		}

		QScrollBar::handle:horizontal {
			background: #A0A0A0;
			min-width: 20px;
			border-radius: 5px;
		}

		QScrollBar::handle:horizontal:hover {
			background: #888888;
		}

		QScrollBar::add-line:horizontal,
		QScrollBar::sub-line:horizontal {
			width: 0px;
			subcontrol-origin: margin;
		}

		QScrollBar::add-page:horizontal,
		QScrollBar::sub-page:horizontal {
			background: none;
		}

		QCheckBox {
			color: #222222;
			font-weight: bold;
			outline: none;
		}

		QCheckBox:hover {
			color: #111111;
		}

		QCheckBox::indicator {
			width: 15px;
			height: 15px;
			border: 1px solid #AAAAAA;
			background: #DDDDDD;
			border-radius: 5px;
		}

		QCheckBox::indicator:checked {
			background-color: #AAAAAA;
		}

		QCheckBox::indicator:hover {
			border: 1px solid #FFFFFF;
		}

       QPushButton {
            background-color: #D0D0D0;
            color: black;
            border-radius: 10px;
            font-weight: bold;
            min-width: 30px;
            min-height: 30px;
			outline: none;
        }

        QPushButton:hover {
            background-color: #c0c0c0;
            border: 1px solid #FFFFFF;
        }

        QPushButton:pressed {
            background-color: #B0B0B0;
        }

		QToolTip {
			background-color: #D0D0D0;
			color: #000000;
			border: 1px solid #AAAAAA;
			padding: 5px;
			border-radius:10px;
		}
	)");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(10, 10, 10, 10);

	auto addComboBoxRow = [&](QComboBox*& comboBox, QLabel*& labelShortcut, const QString& labelText) {
		QHBoxLayout* row = new QHBoxLayout();

		QLabel* label = new QLabel(labelText, this);
		label->setFixedHeight(25);
		label->setFixedWidth(85);
		label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label->setStyleSheet("font-weight: bold; background: transparent;");

		comboBox = new QComboBox(this);
		comboBox->setFixedHeight(25);

		labelShortcut = new QLabel(this);
		labelShortcut->setFixedHeight(25);
		labelShortcut->setFixedWidth(70);
		labelShortcut->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		labelShortcut->setStyleSheet("color: #888888; font-weight: normal; background: transparent;");

		row->addWidget(label);
		row->addWidget(comboBox, 1);
		row->addWidget(labelShortcut);

		layout->addLayout(row);
		};

	addComboBoxRow(comboLangVietGlobal, labelShortcutLangVietGlobal, "Chế độ");
	connect(comboLangVietGlobal, &QComboBox::currentTextChanged, this, [this](const QString& langVietGlobal) {
		QString strFlagLangVietGlobal = (langVietGlobal == "Tiếng Việt") ? "true" : "false";
		saveSettings(m_AppNameConfig, "flagLangVietGlobal", strFlagLangVietGlobal);
		});

	addComboBoxRow(comboCharacterSet, labelShortcutCharacterSet, "Bộ mã");
	connect(comboCharacterSet, &QComboBox::currentTextChanged, this, [this](const QString& characterSet) {
		saveSettings(m_AppNameConfig, "characterSet", characterSet);
		});

	addComboBoxRow(comboNameTaskAI, labelShortcutTaskAI, "Tác vụ AI");
	connect(comboNameTaskAI, &QComboBox::currentTextChanged, this, [this](const QString& nameTaskAI) {
		saveSettings(m_AppNameConfig, "nameTaskAI", nameTaskAI);
		});

	addComboBoxRow(comboNameSnippetString, labelShortcutNameSnippetString, "Gõ tắt tức thì");
	connect(comboNameSnippetString, &QComboBox::currentTextChanged, this, [this](const QString& nameSnippetString) {
		saveSettings(m_AppNameConfig, "nameSnippetString", nameSnippetString);
		});

	addComboBoxRow(comboNameSnippetWords, labelShortcutNameSnippetWords, "Gõ tắt khi cách");
	connect(comboNameSnippetWords, &QComboBox::currentTextChanged, this, [this](const QString& nameSnippetWords) {
		saveSettings(m_AppNameConfig, "nameSnippetWords", nameSnippetWords);
		});

	QFrame* spacer = new QFrame();
	spacer->setFixedHeight(0);
	layout->addWidget(spacer);

	auto addCheckboxRow = [&](QCheckBox*& checkbox, QLabel*& labelShortcut,  const QString& labelText) {
		QHBoxLayout* row = new QHBoxLayout();
		checkbox = new QCheckBox(labelText, this);
		checkbox->setFixedHeight(25);

		labelShortcut = new QLabel(this);
		labelShortcut->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		labelShortcut->setFixedHeight(25);
		labelShortcut->setFixedWidth(70);
		labelShortcut->setStyleSheet("color: #888888; font-weight: normal; background: transparent;");
		row->addWidget(checkbox);
		row->addWidget(labelShortcut, 1);
		layout->addLayout(row);
		};

	addCheckboxRow(checkBoxUseDynamic, labelShortcutUseDynamic, "Sử dụng chế độ tiếng Việt chủ động");
	addCheckboxRow(checkBoxTypeSimple, labelShortcutTypeSimple, "Sử dụng kiểu gõ giản lược");
	addCheckboxRow(checkBoxClipboard, labelShortcutClipboard, "Sử dụng clipboard khi gửi phím");
	addCheckboxRow(checkBoxFixAutoSuggest, labelShortcutFixAutoSuggest, "Tương thích với ứng dụng có gợi ý từ");
	addCheckboxRow(checkBoxCheckCase, labelShortcutCheckCase, "Tự động viết hoa thông minh");
	addCheckboxRow(checkBoxTeenCode, labelShortcutTeenCode, "Cho phép dùng phụ âm đầu \"f\" \"j\" \"w\" \"z\"");
	addCheckboxRow(checkBoxUseLeftRight, labelShortcutUseLeftRight, "Dùng phím ← → để điều hướng từng từ");

	auto* separator = new QFrame(this);
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Plain);
	separator->setFixedHeight(1);
	separator->setStyleSheet("background-color: #CCCCCC;");
	layout->addWidget(separator);
	layout->addSpacing(5);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	resetBtn = new QPushButton("Mặc định", this);
	resetAllBtn = new QPushButton("Mặc định toàn bộ ứng dụng", this);

	resetBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	resetAllBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	resetBtn->setMinimumHeight(30);
	resetAllBtn->setMinimumHeight(30);
	buttonLayout->addWidget(resetBtn);
	buttonLayout->addWidget(resetAllBtn);
	layout->addLayout(buttonLayout);

	connect(checkBoxUseDynamic, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeUseDynamic", state == Qt::Checked ? "true" : "false");
		});
	connect(checkBoxTypeSimple, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeTypeSimple", state == Qt::Checked ? "true" : "false");
		});
	connect(checkBoxClipboard, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeClipboard", state == Qt::Checked ? "true" : "false");
		});
	connect(checkBoxFixAutoSuggest, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeFixAutoSuggest", state == Qt::Checked ? "true" : "false");
		});
	connect(checkBoxCheckCase, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeCheckCase", state == Qt::Checked ? "true" : "false");
		});
	connect(checkBoxTeenCode, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeTeenCode", state == Qt::Checked ? "true" : "false");
		});
	connect(checkBoxUseLeftRight, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeUseLeftRight", state == Qt::Checked ? "true" : "false");
		});
	connect(resetBtn, &QPushButton::clicked, this, &ConfigUi::onResetButtonClicked);
	connect(resetAllBtn, &QPushButton::clicked, this, &ConfigUi::onResetAllButtonClicked);

	layout->addStretch();
	setLayout(layout);
}

ConfigUi::~ConfigUi() {
	doClose();
}

void ConfigUi::doShow(QWidget* parent) {
	Variable& variable = Variable::getInstance();
	m_AppNameConfig = variable.nameCurrentWindow;
	if (m_AppNameConfig == "Sigma.exe") {
		return;
	}

	loadSettings();
	updateShortcutLabels();
	setWindowTitle("Cấu hình ứng dụng -  " + toTitle(getFileName(m_AppNameConfig)));

	HWND hwndFocused = GetForegroundWindow();
	HWND hwndTop = GetAncestor(hwndFocused, GA_ROOT);
	int winW = width(), winH = height();

	if (hwndTop) {
		RECT rect;
		if (GetWindowRect(hwndTop, &rect)) {
			HMONITOR hMonitor = MonitorFromWindow(hwndTop, MONITOR_DEFAULTTONEAREST);
			MONITORINFO monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFO);
			if (GetMonitorInfo(hMonitor, &monitorInfo)) {
				RECT visibleRect;
				visibleRect.left = valueMax(rect.left, monitorInfo.rcMonitor.left);
				visibleRect.top = valueMax(rect.top, monitorInfo.rcMonitor.top);
				visibleRect.right = valueMin(rect.right, monitorInfo.rcMonitor.right);
				visibleRect.bottom = valueMin(rect.bottom, monitorInfo.rcMonitor.bottom);

				QRect monitorRect(
					monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.top,
					monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top
				);

				QScreen* screen = nullptr;
				for (QScreen* scr : QGuiApplication::screens()) {
					if (scr->geometry().intersects(monitorRect)) {
						screen = scr;
						break;
					}
				}

				if (screen) {
					qreal scale = screen->devicePixelRatio();

					int vx = (visibleRect.left + visibleRect.right) / 2;
					int vy = visibleRect.top + ((visibleRect.bottom - visibleRect.top) / 2);
					QPoint logicalPos(int(vx / scale), int(vy / scale));

					QScreen* confirmedScreen = QGuiApplication::screenAt(logicalPos);
					if (!confirmedScreen) {
						confirmedScreen = screen;
					}
					QRect screenGeom = confirmedScreen->geometry();

					int x = logicalPos.x() - winW / 2;
					int y = logicalPos.y() - winH / 2;

					x = std::clamp(x, screenGeom.left(), screenGeom.right() - winW);
					y = std::clamp(y, screenGeom.top(), screenGeom.bottom() - winH);

					move(x, y);
				}
			}
		}
		else {
			move(100, 100);
		}
	}
	else {
		move(100, 100);
	}

	setWindowOpacity(0.0);
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(1000);
	anim->setStartValue(0.0);
	anim->setEndValue(0.95);
	anim->start(QAbstractAnimation::DeleteWhenStopped);

	show();
	raise();
	activateWindow();
	setFocus();
}

void ConfigUi::doHide() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(300);
	anim->setStartValue(this->windowOpacity());
	anim->setEndValue(0.0);
	connect(anim, &QPropertyAnimation::finished, this, [this]() {
		hide();
		});
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void ConfigUi::doClose() {
	hide();
}

void ConfigUi::onResetButtonClicked()
{
	AccountManager* accountManager = AccountManager::getInstance();
	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/ConfigUi/" + m_AppNameConfig);
	settings.remove("");
	settings.endGroup();
	loadSettings();
}

void ConfigUi::onResetAllButtonClicked()
{
	AccountManager* accountManager = AccountManager::getInstance();
	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/ConfigUi");
	settings.remove("");
	settings.endGroup();
	loadSettings();
}

void ConfigUi::updateShortcutLabels() {
	ShortcutKeyEditor* shortcutKeyEditor = ShortcutKeyEditor::getInstance();

	auto updateLabel = [&](QLabel* label, const QString& actionName) {
		QString shortcut = shortcutKeyEditor->getShortcutKey(actionName);
		if (shortcut.isEmpty()) {
			label->hide();
		}
		else {
			label->setText(shortcut);
			label->show();
		}
	};

	labelShortcutLangVietGlobal->setText("Ctrl + Shift");
	updateLabel(labelShortcutCharacterSet, "Chuyển đổi bộ mã");
	updateLabel(labelShortcutTaskAI, "Thực hiện tác vụ AI mặc định");
	updateLabel(labelShortcutNameSnippetString, "");
	updateLabel(labelShortcutNameSnippetWords, "");
	updateLabel(labelShortcutUseDynamic, "Bật | tắt sử dụng chế độ tiếng Việt chủ động");
	updateLabel(labelShortcutTypeSimple, "Bật | tắt sử dụng kiểu gõ giản lược");
	updateLabel(labelShortcutClipboard, "Bật | tắt sử dụng clipboard khi gửi phím");
	updateLabel(labelShortcutFixAutoSuggest, "Bật | tắt tương thích với ứng dụng có gợi ý từ");
	updateLabel(labelShortcutCheckCase, "Bật | tắt tự động viết hoa thông minh");
	updateLabel(labelShortcutTeenCode, "Bật | tắt cho phép dùng phụ âm đầu \"f\" \"j\" \"w\" \"z\"");
	updateLabel(labelShortcutUseLeftRight, "Bật | tắt dùng phím ← → để điều hướng từng từ");
}

void ConfigUi::closeEvent(QCloseEvent* event) {
	doHide();
	event->ignore();
}

bool ConfigUi::event(QEvent* e) {
	if (e->type() == QEvent::WindowDeactivate) {
		doHide();
	}
	return QWidget::event(e);
}

void ConfigUi::saveSettings(QString appNameConfig, QString nameValue, QString value) {
	if (m_isLoading) {
		return;
	}
	Variable& variable = Variable::getInstance();
	AccountManager* accountManager = AccountManager::getInstance();

	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/ConfigUi/" + appNameConfig);
	settings.setValue(nameValue, value);
	settings.endGroup();

	bool mode = (value == "true") ? true : false;
	QCheckBox* checkBox = nullptr;
	QString stringWarning = "";

	if (nameValue == "modeUseDynamic") {
		checkBox = checkBoxUseDynamic;
		if (mode) {
			stringWarning = "Luôn ở chế độ tiếng Anh, nhấn Ctrl + Shift trái để chuyển sang tiếng Việt.\nkhi thao tác chuột sẽ tự chuyển lại tiếng Anh.";
		}
	}

	if (nameValue == "modeClipboard") {
		checkBox = checkBoxClipboard;
		if (variable.listAppUseClipboard.contains(m_AppNameConfig.toLower())) {
			if (!mode) {
				stringWarning = "Tắt thiết lập này có thể gây lỗi gõ trong một số trường hợp";
			}
		}
		else {
			if (mode) {
				stringWarning = "Bật thiết lập này là không cần thiết với ứng dụng này.\nNó có thể gây trễ và giật chữ khi gõ.";
			}
		}
	}

	if (nameValue == "modeFixAutoSuggest") {
		checkBox = checkBoxFixAutoSuggest;
		if (variable.listAppFixAutoSuggest.contains(m_AppNameConfig.toLower())) {
			if (!mode) {
				stringWarning = "Tắt thiết lập này có thể gây lỗi gõ trong một số trường hợp";
			}
		}
		else {
			if (mode) {
				stringWarning = "Bật thiết lập này là không cần thiết với ứng dụng này.\nNó có thể gây trễ và giật chữ khi gõ.";
			}
		}
	}

	if (checkBox) {
		updateCheckBoxStyle(checkBox, stringWarning);
	}
}

void ConfigUi::loadSettings() {
	Variable& variable = Variable::getInstance();
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
	SnippetEditor* snippetEditor = SnippetEditor::getInstance();
	AccountManager* accountManager = AccountManager::getInstance();

	m_isLoading = true;

	comboLangVietGlobal->clear();
	comboLangVietGlobal->addItem("Tiếng Anh");
	comboLangVietGlobal->addItem("Tiếng Việt");

	comboCharacterSet->clear();
	for (const auto& pair : variable.mapCharacterSetBase) {
		comboCharacterSet->addItem(QString::fromStdWString(pair.first));
	}

	comboNameTaskAI->clear();
	for (const QString& item : taskAIDatabase.listNameTaskAI) {
		comboNameTaskAI->addItem(item);
	}

	QStringList listNameSnippetTemp = QStringList(snippetEditor->listNameSnippet.begin(), snippetEditor->listNameSnippet.end());
	std::sort(listNameSnippetTemp.begin(), listNameSnippetTemp.end(), [](const QString& a, const QString& b) {
		return a.compare(b, Qt::CaseInsensitive) < 0;
		});

	comboNameSnippetString->clear();
	comboNameSnippetString->addItem("Không sử dụng");
	for (const QString& item : listNameSnippetTemp) {
		comboNameSnippetString->addItem(item);
	}

	comboNameSnippetWords->clear();
	comboNameSnippetWords->addItem("Không sử dụng");
	for (const QString& item : listNameSnippetTemp) {
		comboNameSnippetWords->addItem(item);
	}

	bool modeLangVietGlobalDefault = variable.listAppLangVietGlobal.contains(m_AppNameConfig.toLower()) ? true : false;
	bool modeClipboardDefault = variable.listAppUseClipboard.contains(m_AppNameConfig.toLower()) ? true : false;
	bool modeFixAutoSuggestDefault = variable.listAppFixAutoSuggest.contains(m_AppNameConfig.toLower()) ? true : false;
	QString nameTaskAIDefault = taskAIDatabase.listNameTaskAI.value(0, QString());

	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(accountManager->currentAccount + "/ConfigUi/" + m_AppNameConfig);
	bool flagLangVietGlobal = settings.value("flagLangVietGlobal", modeLangVietGlobalDefault).toBool();
	std::wstring characterSet = settings.value("characterSet", QString::fromStdWString(variable.CHARACTERSET)).toString().toStdWString();
	variable.nameTaskAI = settings.value("nameTaskAI", nameTaskAIDefault).toString();
	variable.nameSnippetString = settings.value("nameSnippetString", variable.NAMESNIPPETSTRING).toString();
	variable.nameSnippetWords = settings.value("nameSnippetWords", variable.NAMESNIPPETWORDS).toString();
	bool modeUseDynamic = settings.value("modeUseDynamic", variable.MODEUSEDYNAMIC).toBool();
	bool modeTypeSimple = settings.value("modeTypeSimple", variable.MODETYPESIMPLE).toBool();
	bool modeClipboard = settings.value("modeClipboard", modeClipboardDefault).toBool();
	bool modeFixAutoSuggest = settings.value("modeFixAutoSuggest", modeFixAutoSuggestDefault).toBool();
	bool modeCheckCase = settings.value("modeCheckCase", variable.MODECHECKCASE).toBool();
	bool modeTeenCode = settings.value("modeTeenCode", variable.MODETEENCODE).toBool();
	bool modeUseLeftRight = settings.value("modeUseLeftRight", variable.MODEUSELEFTRIGHT).toBool();
	settings.endGroup();

	if (variable.mapCharacterSetBase.find(characterSet) == variable.mapCharacterSetBase.end()) {
		characterSet = variable.CHARACTERSET;
	}
	if (!taskAIDatabase.listNameTaskAI.contains(variable.nameTaskAI)) {
		variable.nameTaskAI = nameTaskAIDefault;
	}
	if (!snippetEditor->listNameSnippet.contains(variable.nameSnippetString)) {
		variable.nameSnippetString = variable.NAMESNIPPETSTRING;
	}
	if (!snippetEditor->listNameSnippet.contains(variable.nameSnippetWords)) {
		variable.nameSnippetWords = variable.NAMESNIPPETWORDS;
	}

	QString charSetStr = QString::fromStdWString(characterSet);
	QString nameTaskAIStr = variable.nameTaskAI;
	QString nameSnippetStringStr = variable.nameSnippetString;
	QString nameSnippetWordsStr = variable.nameSnippetWords;

	int indexLang = flagLangVietGlobal ? 1 : 0;
	comboLangVietGlobal->setCurrentIndex(indexLang);

	int indexChar = comboCharacterSet->findText(charSetStr);
	if (indexChar != -1) comboCharacterSet->setCurrentIndex(indexChar);

	int indexTask = comboNameTaskAI->findText(nameTaskAIStr);
	if (indexTask != -1) comboNameTaskAI->setCurrentIndex(indexTask);

	int indexSnippetString = comboNameSnippetString->findText(nameSnippetStringStr);
	if (indexSnippetString != -1) comboNameSnippetString->setCurrentIndex(indexSnippetString);

	int indexSnippetWords = comboNameSnippetWords->findText(nameSnippetWordsStr);
	if (indexSnippetWords != -1) comboNameSnippetWords->setCurrentIndex(indexSnippetWords);

	checkBoxUseDynamic->setChecked(modeUseDynamic);
	checkBoxTypeSimple->setChecked(modeTypeSimple);
	checkBoxClipboard->setChecked(modeClipboard);
	checkBoxFixAutoSuggest->setChecked(modeFixAutoSuggest);
	checkBoxCheckCase->setChecked(modeCheckCase);
	checkBoxTeenCode->setChecked(modeTeenCode);
	checkBoxUseLeftRight->setChecked(modeUseLeftRight);

	bool mode;
	QCheckBox* checkBox;
	QString stringWarning;

	mode = modeUseDynamic;
	checkBox = checkBoxUseDynamic;
	stringWarning = "";
	if (mode) {
		stringWarning = "Luôn ở chế độ tiếng Anh, nhấn Ctrl + Shift trái để chuyển sang tiếng Việt.\nkhi thao tác chuột sẽ tự chuyển lại tiếng Anh.";
	}
	updateCheckBoxStyle(checkBox, stringWarning);

	mode = modeClipboard;
	checkBox = checkBoxClipboard;
	stringWarning = "";
	if (variable.listAppUseClipboard.contains(m_AppNameConfig.toLower())) {
		if (!mode) {
			stringWarning = "Tắt thiết lập này có thể gây lỗi gõ trong một số trường hợp";
		}
	}
	else {
		if (mode) {
			stringWarning = "Bật thiết lập này là không cần thiết với ứng dụng này, có thể gây trễ và giật chữ khi gõ.";
		}
	}
	updateCheckBoxStyle(checkBox, stringWarning);

	mode = modeFixAutoSuggest;
	checkBox = checkBoxFixAutoSuggest,
	stringWarning = "";
	if (variable.listAppFixAutoSuggest.contains(m_AppNameConfig.toLower())) {
		if (!mode) {
			stringWarning = "Tắt thiết lập này có thể gây lỗi gõ trong một số trường hợp";
		}
	}
	else {
		if (mode) {
			stringWarning = "Bật thiết lập này là không cần thiết với ứng dụng này, có thể gây trễ và giật chữ khi gõ.";
		}
	}
	updateCheckBoxStyle(checkBox, stringWarning);

	m_isLoading = false;
}

void ConfigUi::updateCheckBoxStyle(QCheckBox* checkBox, QString stringWarning)
{
	const char* CHECKBOX_WARNING_STYLE = R"(
		QCheckBox {
			color: #A63F0D;
			font-weight: bold;
			outline: none;
		}

		QCheckBox:hover {
			color: #A63F0D;
		}

		QCheckBox::indicator {
			width: 15px;
			height: 15px;
			border: 1px solid #A63F0D;
			background: #DDDDDD;
			border-radius: 5px;
		}

		QCheckBox::indicator:checked {
			background-color: #A63F0D;
		}

		QCheckBox::indicator:hover {
			border: 1px solid #FFFFFF;
		}
    )";

	const char* CHECKBOX_DEFAULT_STYLE = R"(
		QCheckBox {
			color: #222222;
			font-weight: bold;
			outline: none;
		}

		QCheckBox:hover {
			color: #111111;
		}

		QCheckBox::indicator {
			width: 15px;
			height: 15px;
			border: 1px solid #AAAAAA;
			background: #DDDDDD;
			border-radius: 5px;
		}

		QCheckBox::indicator:checked {
			background-color: #AAAAAA;
		}

		QCheckBox::indicator:hover {
			border: 1px solid #FFFFFF;
		}
    )";

	if (stringWarning.isEmpty()) {
		checkBox->setStyleSheet(CHECKBOX_DEFAULT_STYLE);
	}
	else {
		checkBox->setStyleSheet(CHECKBOX_WARNING_STYLE);
	}
	checkBox->setToolTip(stringWarning);
}
