#include "GeneralConfig.h"
#include "Variable.h"
#include <Util.h>

#include <QPropertyAnimation>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFrame>
#include <QSettings>
#include <QLabel>
#include <QComboBox>

QPointer<GeneralConfig> GeneralConfig::m_instance = nullptr;

GeneralConfig* GeneralConfig::getInstance() {
	if (!m_instance) {
		m_instance = new GeneralConfig();
	}
	return m_instance;
}

void GeneralConfig::showWindow() {
	GeneralConfig* ui = getInstance();
	if (!ui->isVisible()) {
		ui->loadWindow();
		ui->fadeIn();
		ui->show();
		ui->raise();
		ui->activateWindow();
	}
}

void GeneralConfig::hideWindow() {
	if (m_instance && m_instance->isVisible()) {
		m_instance->fadeOut();
	}
}

void GeneralConfig::closeWindow() {
	if (m_instance) {
		QPropertyAnimation* anim = new QPropertyAnimation(m_instance, "windowOpacity");
		anim->setDuration(300);
		anim->setStartValue(m_instance->windowOpacity());
		anim->setEndValue(0.0);

		QObject::connect(anim, &QPropertyAnimation::finished, m_instance, []() {
			m_instance->hide();
			m_instance->deleteLater();
			m_instance = nullptr;
			});

		anim->start(QAbstractAnimation::DeleteWhenStopped);
	}
}

void GeneralConfig::closeEvent(QCloseEvent* event) {
	event->ignore();
	hideWindow();
}

void GeneralConfig::reject() {
	hideWindow();
}

GeneralConfig::GeneralConfig(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("Cấu hình chung");
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
	setWindowIcon(QIcon(":/icon.png"));
	resize(450, 300);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowOpacity(0.0);

	setStyleSheet(R"(
		QWidget {
			background-color: rgba(220, 220, 220, 255);
			font-size: 12px;
			border-radius: 10px;
		}

		QLabel {
			color: #222222; /* Chữ đen xám đậm */
			font-weight: bold;
			background-color: #e0e0e0; /* Nền xám nhạt */
			padding-left: 5px;
			padding-right: 5px;
			border-radius: 10px;
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
			outline: none;
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

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor("#E0E0E0"));
	palette.setColor(QPalette::WindowText, Qt::black);
	setPalette(palette);

	QVBoxLayout* layout = new QVBoxLayout(this);

	auto addComboBoxRow = [&](QComboBox*& comboBox, const QString& labelText, const QString& shortcutText) {
		QHBoxLayout* row = new QHBoxLayout();

		QLabel* label = new QLabel(labelText, this);
		label->setFixedWidth(70);

		comboBox = new QComboBox(this);
		
		QLabel* labelShortcut = new QLabel(shortcutText, this);
		labelShortcut->setFixedWidth(75);
		labelShortcut->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelShortcut->setStyleSheet("color: #666666; font-weight: normal;");

		row->addWidget(label);
		row->addWidget(comboBox, 1);
		row->addWidget(labelShortcut);

		layout->addLayout(row);
		};

	addComboBoxRow(comboInputMethod, "Kiểu gõ", "Shift → G");

	auto addCheckboxRow = [&](QCheckBox*& checkbox, const QString& labelText, const QString& shortcutText) {
		QHBoxLayout* row = new QHBoxLayout();
		checkbox = new QCheckBox(labelText, this);
		QLabel* labelShortcut = new QLabel(shortcutText, this);
		labelShortcut->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelShortcut->setFixedWidth(75);
		labelShortcut->setStyleSheet("color: #666666; font-weight: normal;");
		row->addWidget(checkbox);
		row->addWidget(labelShortcut, 1);
		layout->addLayout(row);
		};

	addCheckboxRow(checkBoxAutoStart, "Khởi động cùng Windows", "Shift → 6");
	// addCheckboxRow(checkBoxAutoUpdate,  "Tự động cập nhật khi khởi động", "");
	addCheckboxRow(checkBoxUseSnippet, "Cho phép gõ tắt", "Shift → 7");
	addCheckboxRow(checkBoxLoopDiacTone, "Cho phép bỏ dấu xoay vòng", "Shift → 8");
	addCheckboxRow(checkBoxAutoAddVowel, "Tự động thêm dấu mũ các vần \"iê\" \"uê\" \"uô\" \"uyê\" \"yê\"", "Shift → 9");
	addCheckboxRow(checkBoxShortcutLast, "Cho phép gõ tắt các âm cuối \"ch\" \"ng\" \"nh\"", "Shift → 0");
	addCheckboxRow(checkBoxUseLeftRight, "Dùng phím ← → để điều hướng từng từ", "Shift → -");
	addCheckboxRow(checkBoxAutoChangeLang, "Tự động chuyển từ tiếng Anh đã ghi nhớ", "Shift → =");

	auto* separator = new QFrame(this);
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Plain);
	separator->setFixedHeight(1);
	separator->setStyleSheet("background-color: #CCCCCC;");
	layout->addWidget(separator);
	layout->addSpacing(5);

	saveBtn = new QPushButton("Lưu", this);
	defaultBtn = new QPushButton("Mặc định", this);
	cancelBtn = new QPushButton("Hủy", this);

	connect(saveBtn, &QPushButton::clicked, this, &GeneralConfig::onSaveButtonClicked);
	connect(defaultBtn, &QPushButton::clicked, this, &GeneralConfig::onDefaultButtonClicked);
	connect(cancelBtn, &QPushButton::clicked, this, &GeneralConfig::onCancelButtonClicked);
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	saveBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	defaultBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	cancelBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	saveBtn->setMinimumHeight(30);
	defaultBtn->setMinimumHeight(30);
	cancelBtn->setMinimumHeight(30);
	buttonLayout->addWidget(saveBtn);
	buttonLayout->addWidget(defaultBtn);
	buttonLayout->addWidget(cancelBtn);
	layout->addLayout(buttonLayout);

	layout->addStretch();
	setLayout(layout);

	loadWindow();
}

GeneralConfig::~GeneralConfig() {}

void GeneralConfig::loadWindow() {
	Variable& variable = Variable::getInstance();

	comboInputMethod->clear();
	for (const auto& pair : variable.mapInputMethodBase) {
		comboInputMethod->addItem(QString::fromStdWString(pair.first));
	}

	int indexInput = comboInputMethod->findText(QString::fromStdWString(variable.inputMethod));
	if (indexInput != -1) comboInputMethod->setCurrentIndex(indexInput);

	checkBoxAutoStart->setChecked(variable.modeAutoStart);
	//checkBoxAutoUpdate->setChecked(variable.modeAutoUpdate);
	checkBoxUseSnippet->setChecked(variable.modeUseSnippet);
	checkBoxLoopDiacTone->setChecked(variable.modeLoopDiacTone);
	checkBoxAutoAddVowel->setChecked(variable.modeAutoAddVowel);
	checkBoxShortcutLast->setChecked(variable.modeShortcutLast);
	checkBoxUseLeftRight->setChecked(variable.modeUseLeftRight);
	checkBoxAutoChangeLang->setChecked(variable.modeAutoChangeLang);
}

void GeneralConfig::fadeIn() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(1000);
	anim->setStartValue(0.0);
	anim->setEndValue(0.95);
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void GeneralConfig::fadeOut() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(300);
	anim->setStartValue(0.95);
	anim->setEndValue(0.0);
	connect(anim, &QPropertyAnimation::finished, this, [=]() {
		this->hide();
		this->setWindowOpacity(1.0);
		});
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void GeneralConfig::onSaveButtonClicked() {
	Variable& variable = Variable::getInstance();

	variable.inputMethod = comboInputMethod->currentText().toStdWString();
	variable.modeAutoStart = checkBoxAutoStart->isChecked();
	//variable.modeAutoUpdate = checkBoxAutoUpdate->isChecked();
	variable.modeUseSnippet = checkBoxUseSnippet->isChecked();
	variable.modeLoopDiacTone = checkBoxLoopDiacTone->isChecked();
	variable.modeAutoAddVowel = checkBoxAutoAddVowel->isChecked();
	variable.modeShortcutLast = checkBoxShortcutLast->isChecked();
	variable.modeUseLeftRight = checkBoxUseLeftRight->isChecked();
	variable.modeAutoChangeLang = checkBoxAutoChangeLang->isChecked();

	QSettings settings(variable.appName, "Config");
	settings.setValue("inputMethod", QString::fromStdWString(variable.inputMethod));
	settings.setValue("modeAutoStart", variable.modeAutoStart);
	setAutoStartApp(variable.modeAutoStart);
	//settings.setValue("modeAutoUpdate", variable.modeAutoUpdate);
	settings.setValue("modeUseSnippet", variable.modeUseSnippet);
	settings.setValue("modeLoopDiacTone", variable.modeLoopDiacTone);
	settings.setValue("modeAutoAddVowel", variable.modeAutoAddVowel);
	settings.setValue("modeShortcutLast", variable.modeShortcutLast);
	settings.setValue("modeUseLeftRight", variable.modeUseLeftRight);
	settings.setValue("modeAutoChangeLang", variable.modeAutoChangeLang);

	variable.update();
	hideWindow();
}

void GeneralConfig::onDefaultButtonClicked()
{
	Variable& variable = Variable::getInstance();

	int indexInput = comboInputMethod->findText(QString::fromStdWString(variable.INPUTMETHOD));
	if (indexInput != -1) comboInputMethod->setCurrentIndex(indexInput);

	checkBoxAutoStart->setChecked(variable.MODEAUTOSTART);
	//checkBoxAutoUpdate->setChecked(variable.MODEAUTOUPDATE);
	checkBoxUseSnippet->setChecked(variable.MODEUSESNIPPET);
	checkBoxLoopDiacTone->setChecked(variable.MODELOOPDIACTONE);
	checkBoxAutoAddVowel->setChecked(variable.MODEAUTOADDVOWEL);
	checkBoxShortcutLast->setChecked(variable.MODESHORTCUTLAST);
	checkBoxUseLeftRight->setChecked(variable.MODEUSELEFTRIGHT);
	checkBoxAutoChangeLang->setChecked(variable.MODEAUTOCHANGELANG);
}

void GeneralConfig::onCancelButtonClicked() {
	hideWindow();
}