#include "ConfigUi.h"
#include "Variable.h"
#include "Util.h"
#include "Listener.h"
#include "TaskAIEditor.h"
#include "TypeWord.h"
#include "TaskAIDatabase.h"
#include "ShortcutKeyEditor.h"

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
	setFixedSize(400, 350);
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
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
	)");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(10, 10, 10, 10);

	auto addComboBoxRow = [&](QComboBox*& comboBox, QLabel*& labelShortcut, const QString& labelText) {
		QHBoxLayout* row = new QHBoxLayout();

		QLabel* label = new QLabel(labelText, this);
		label->setFixedHeight(25);
		label->setFixedWidth(60);
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

	addComboBoxRow(m_comboCharacterSet, m_labelShortcutCharacterSet, "Bộ mã");
	connect(m_comboCharacterSet, &QComboBox::currentTextChanged, this, [this](const QString& characterSet) {
		saveSettings(m_AppNameConfig, "characterSet", characterSet);
		});

	addComboBoxRow(m_comboNameTaskAI, m_labelShortcutTaskAI, "Tác vụ AI");
	connect(m_comboNameTaskAI, &QComboBox::currentTextChanged, this, [this](const QString& nameTaskAI) {
		saveSettings(m_AppNameConfig, "nameTaskAI", nameTaskAI);
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

	addCheckboxRow(m_checkBoxModeUseDynamic, m_labelShortcutModeUseDynamic, "Sử dụng chế độ tiếng Việt chủ động");
	addCheckboxRow(m_checkBoxModeClipboard, m_labelShortcutModeClipboard, "Sử dụng clipboard khi gửi phím");
	addCheckboxRow(m_checkBoxModeFixAutoSuggest, m_labelShortcutModeFixAutoSuggest, "Tương thích với ứng dụng có gợi ý từ");
	addCheckboxRow(m_checkBoxModeCheckCase, m_labelShortcutModeCheckCase, "Tự động viết hoa thông minh");
	addCheckboxRow(m_checkBoxModeTeenCode, m_labelShortcutModeTeenCode, "Cho phép gõ ký tự Teencode");
	addCheckboxRow(m_checkBoxModeInsertChar, m_labelShortcutModeInsertChar, "Cho phép chèn ký tự bị thiếu");
	addCheckboxRow(m_checkBoxModeUseLeftRight, m_labelShortcutModeUseLeftRight, "Dùng phím ← → để điều hướng từng từ");

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

	connect(m_checkBoxModeUseDynamic, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeUseDynamic", state == Qt::Checked ? "true" : "false");
		});
	connect(m_checkBoxModeClipboard, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeClipboard", state == Qt::Checked ? "true" : "false");
		});
	connect(m_checkBoxModeFixAutoSuggest, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeFixAutoSuggest", state == Qt::Checked ? "true" : "false");
		});
	connect(m_checkBoxModeCheckCase, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeCheckCase", state == Qt::Checked ? "true" : "false");
		});
	connect(m_checkBoxModeTeenCode, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeTeenCode", state == Qt::Checked ? "true" : "false");
		});
	connect(m_checkBoxModeInsertChar, &QCheckBox::checkStateChanged, this, [this](int state) {
		saveSettings(m_AppNameConfig, "modeInsertChar", state == Qt::Checked ? "true" : "false");
		});
	connect(m_checkBoxModeUseLeftRight, &QCheckBox::checkStateChanged, this, [this](int state) {
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
	Variable& variable = Variable::getInstance();
	QSettings settings(variable.appName, "ConfigUi");
	settings.beginGroup(m_AppNameConfig);
	settings.remove("");
	settings.endGroup();
	loadSettings();
}

void ConfigUi::onResetAllButtonClicked()
{
	Variable& variable = Variable::getInstance();
	QSettings settings(variable.appName, "ConfigUi");
	QStringList groups = settings.childGroups();
	for (const QString& group : groups) {
		settings.beginGroup(group);
		settings.remove("");
		settings.endGroup();
	}
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

	updateLabel(m_labelShortcutCharacterSet, "Chuyển đổi bộ mã");
	updateLabel(m_labelShortcutTaskAI, "Thực hiện tác vụ AI mặc định");
	updateLabel(m_labelShortcutModeUseDynamic, "Bật / tắt sử dụng chế độ tiếng Việt chủ động");
	updateLabel(m_labelShortcutModeClipboard, "Bật / tắt Sử dụng clipboard khi gửi phím");
	updateLabel(m_labelShortcutModeFixAutoSuggest, "Bật / tắt tương thích với ứng dụng có gợi ý từ");
	updateLabel(m_labelShortcutModeCheckCase, "Bật / tắt tự động viết hoa thông minh");
	updateLabel(m_labelShortcutModeTeenCode, "Bật / tắt cho phép gõ ký tự Teencode");
	updateLabel(m_labelShortcutModeInsertChar, "Bật / tắt cho phép chèn ký tự bị thiếu");
	updateLabel(m_labelShortcutModeUseLeftRight, "Bật / tắt dùng phím ← → để điều hướng từng từ");
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
	QSettings settings(variable.appName, "ConfigUi");
	settings.beginGroup(appNameConfig);
	settings.setValue(nameValue, value);
	settings.endGroup();
}

void ConfigUi::loadSettings() {
	Variable& variable = Variable::getInstance();
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();

	m_isLoading = true;
	m_comboCharacterSet->clear();
	for (const auto& pair : variable.mapCharacterSetBase) {
		m_comboCharacterSet->addItem(QString::fromStdWString(pair.first));
	}

	m_comboNameTaskAI->clear();
	for (const QString& item : taskAIDatabase.listNameTaskAI) {
		m_comboNameTaskAI->addItem(item);
	}
	m_isLoading = false;

	bool modeClipboardDefault = variable.listAppUseClipboard.contains(m_AppNameConfig.toLower()) ? true : false;
	bool modeFixAutoSuggestDefault = variable.listAppFixAutoSuggest.contains(m_AppNameConfig.toLower()) ? true : false;
	bool modeUseLeftRightDefault = variable.listAppNotUseLeftRight.contains(m_AppNameConfig.toLower()) ? false : true;

	QSettings settings(variable.appName, "ConfigUi");
	settings.beginGroup(m_AppNameConfig);
	std::wstring characterSet = settings.value("characterSet", QString::fromStdWString(variable.CHARACTERSET)).toString().toStdWString();
	variable.nameTaskAI = settings.value("nameTaskAI", variable.NAMETASKAI).toString();
	bool modeUseDynamic = settings.value("modeUseDynamic", variable.MODEUSEDYNAMIC).toBool();
	bool modeClipboard = settings.value("modeClipboard", modeClipboardDefault).toBool();
	bool modeFixAutoSuggest = settings.value("modeFixAutoSuggest", modeFixAutoSuggestDefault).toBool();
	bool modeCheckCase = settings.value("modeCheckCase", variable.MODECHECKCASE).toBool();
	bool modeTeenCode = settings.value("modeTeenCode", variable.MODETEENCODE).toBool();
	bool modeInsertChar = settings.value("modeInsertChar", variable.MODEINSERTCHAR).toBool();
	bool modeUseLeftRight = settings.value("modeUseLeftRight", modeUseLeftRightDefault).toBool();
	settings.endGroup();

	if (variable.mapCharacterSetBase.find(characterSet) == variable.mapCharacterSetBase.end()) {
		characterSet = variable.CHARACTERSET;
	}
	if (!taskAIDatabase.listNameTaskAI.contains(variable.nameTaskAI)) {
		variable.nameTaskAI = variable.NAMETASKAI;
	}

	QString charSetStr = QString::fromStdWString(characterSet);
	QString nameTaskAIStr = variable.nameTaskAI;

	int indexChar = m_comboCharacterSet->findText(charSetStr);
	if (indexChar != -1) m_comboCharacterSet->setCurrentIndex(indexChar);

	int indexTask = m_comboNameTaskAI->findText(nameTaskAIStr);
	if (indexTask != -1) m_comboNameTaskAI->setCurrentIndex(indexTask);

	m_checkBoxModeClipboard->setChecked(modeClipboard);
	m_checkBoxModeUseDynamic->setChecked(modeUseDynamic);
	m_checkBoxModeFixAutoSuggest->setChecked(modeFixAutoSuggest);
	m_checkBoxModeCheckCase->setChecked(modeCheckCase);
	m_checkBoxModeTeenCode->setChecked(modeTeenCode);
	m_checkBoxModeInsertChar->setChecked(modeInsertChar);
	m_checkBoxModeUseLeftRight->setChecked(modeUseLeftRight);
}
