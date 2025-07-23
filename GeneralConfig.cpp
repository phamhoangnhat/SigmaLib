#include "GeneralConfig.h"
#include "Variable.h"
#include <Util.h>
#include <ShortcutKeyEditor.h>
#include <SigmaLib.h>

#include <QPropertyAnimation>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFrame>
#include <QSettings>
#include <QLabel>
#include <QComboBox>
#include <QProcess>
#include <QThread>

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
		ui->updateShortcutLabels();
		ui->fadeIn();
		ui->show();
		ui->raise();
		ui->activateWindow();
	}
}

void GeneralConfig::hideWindow() {
	if (m_instance && m_instance->isVisible()) {
		if (m_instance->popup) {
			m_instance->popup->closeWindow();
			m_instance->popup.clear();
		}
		m_instance->fadeOut();
	}
}

void GeneralConfig::closeWindow() {
	if (m_instance) {
		if (m_instance->popup) {
			m_instance->popup->closeWindow();
			m_instance->popup.clear();
		}

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
	Variable& variable = Variable::getInstance();

	setWindowTitle("Cấu hình chung");
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
	setWindowIcon(QIcon(":/icon.png"));
	setFixedWidth(420);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowOpacity(0.0);

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

        QFrame {
            background-color: #CCCCCC;
        }

		QToolTip {
			background-color: #D0D0D0;
			color: #000000;
			border: 1px solid #AAAAAA;
			padding: 5px;
			border-radius:10px;
		}
	)");

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor("#E0E0E0"));
	palette.setColor(QPalette::WindowText, Qt::black);
	setPalette(palette);

	QVBoxLayout* layout = new QVBoxLayout(this);

	auto addComboBoxRow = [&](QComboBox*& comboBox, QLabel*& labelShortcut, const QString& labelText) {
		QHBoxLayout* row = new QHBoxLayout();

		QLabel* label = new QLabel(labelText, this);
		label->setFixedWidth(60);
		label->setFixedHeight(25);
		label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label->setStyleSheet("font-weight: bold; background: transparent;");

		comboBox = new QComboBox(this);
		comboBox->setFixedHeight(25);
	
		labelShortcut = new QLabel(this);
		labelShortcut->setFixedHeight(25);
		labelShortcut->setFixedWidth(75);
		labelShortcut->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		labelShortcut->setStyleSheet("color: #888888; font-weight: normal; background: transparent;");

		row->addWidget(label);
		row->addWidget(comboBox, 1);
		row->addWidget(labelShortcut);

		layout->addLayout(row);
		};
	
	addComboBoxRow(comboInputMethod, labelShortcutInputMethod, "Kiểu gõ");

	auto addCheckboxRow = [&](QCheckBox*& checkbox, QLabel*& labelShortcut, const QString& labelText) {
		QHBoxLayout* row = new QHBoxLayout();
		checkbox = new QCheckBox(labelText, this);
		checkbox->setFixedHeight(25);

		labelShortcut = new QLabel(this);
		labelShortcut->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		labelShortcut->setFixedHeight(25);
		labelShortcut->setFixedWidth(75);
		labelShortcut->setStyleSheet("color: #888888; font-weight: normal; background: transparent;");

		row->addWidget(checkbox);
		row->addWidget(labelShortcut, 1);
		layout->addLayout(row);
		};

	addCheckboxRow(checkBoxAutoStart, labelShortcutAutoStart, "Khởi động cùng Windows");
	if (variable.verSigmaExe > 0.0) {
		addCheckboxRow(checkBoxAdmin, labelShortcutAdmin, "Chạy Sigma với quyền quản trị admin");
	}
	//addCheckboxRow(checkBoxAutoUpdate, labelShortcutAutoUpdate, "Tự động cập nhật khi khởi động");
	addCheckboxRow(checkBoxRestore, labelShortcutRestore, "Khôi phục từ gốc khi gõ sai chính tả");
	addCheckboxRow(checkBoxRemoveDiacTone, labelShortcutRemoveDiacTone, "Xóa toàn bộ dấu khi nhấn phím bỏ dấu");
	addCheckboxRow(checkBoxLoopDiacTone, labelShortcutLoopDiacTone, "Cho phép bỏ dấu xoay vòng");
	addCheckboxRow(checkBoxInsertChar, labelShortcutInsertChar, "Cho phép chèn ký tự bị thiếu");
	addCheckboxRow(checkBoxAutoChangeLang, labelShortcutAutoChangeLang, "Tự động chuyển từ tiếng Anh đã ghi nhớ");

	connect(checkBoxRemoveDiacTone, &QCheckBox::checkStateChanged, this, [this](int state) {
		updateCheckBoxStyle("modeRemoveDiacTone", state == Qt::Checked);
		});
	connect(checkBoxLoopDiacTone, &QCheckBox::checkStateChanged, this, [this](int state){
		updateCheckBoxStyle("modeLoopDiacTone", state == Qt::Checked);
		});
	connect(checkBoxInsertChar, &QCheckBox::checkStateChanged, this, [this](int state) {
		updateCheckBoxStyle("modeInsertChar", state == Qt::Checked);
		});
	connect(checkBoxAutoChangeLang, &QCheckBox::checkStateChanged, this, [this](int state) {
		updateCheckBoxStyle("modeAutoChangeLang", state == Qt::Checked);
		});
	
	auto* separator = new QFrame(this);
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Plain);
	separator->setFixedHeight(1);
	layout->addWidget(separator);
	layout->addSpacing(5);

	QLabel* noteLabel = new QLabel("Các tùy chọn màu đỏ mang đến trải nghiệm gõ mới. Người dùng mới\nnên cân nhắc trước khi dùng. Để dùng tốt nó, người dùng phải quen\nvới việc nhấn phím Ctrl bên trái bàn phím để chuyển từ đang gõ sang\ntiếng Anh.", this);
	noteLabel->setStyleSheet(R"(
		color: #666666;
		font-style: italic;
		background-color: rgba(220, 220, 220, 255);
		text-align: center;
	)");
	layout->addWidget(noteLabel);

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
	if (variable.verSigmaExe > 0.0) {
		if (isUserAdmin()) {
			checkBoxAdmin->setEnabled(true);
			checkBoxAdmin->setChecked(variable.modeAdmin);
		}
		else {
			variable.modeAdmin = false;
			checkBoxAdmin->setEnabled(false);
			checkBoxAdmin->setChecked(variable.modeAdmin);
		}
	}
	//checkBoxAutoUpdate->setChecked(variable.modeAutoUpdate);
	checkBoxRestore->setChecked(variable.modeRestore);
	checkBoxRemoveDiacTone->setChecked(variable.modeRemoveDiacTone);
	checkBoxLoopDiacTone->setChecked(variable.modeLoopDiacTone);
	checkBoxInsertChar->setChecked(variable.modeInsertChar);
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

void GeneralConfig::updateShortcutLabels() {
	ShortcutKeyEditor* shortcutKeyEditor = ShortcutKeyEditor::getInstance();
	Variable& variable = Variable::getInstance();

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

	updateLabel(labelShortcutInputMethod, "Chuyển đổi kiểu gõ");
	updateLabel(labelShortcutAutoStart, "Bật / tắt khởi động cùng Windows");
	if (variable.verSigmaExe > 0.0) {
		updateLabel(labelShortcutAdmin, "");
	}
	updateLabel(labelShortcutRestore, "Bật / tắt khôi phục từ gốc khi gõ sai chính tả");
	updateLabel(labelShortcutRemoveDiacTone, "Bật / tắt xóa toàn bộ dấu khi nhấn phím bỏ dấu");
	updateLabel(labelShortcutLoopDiacTone, "Bật / tắt cho phép bỏ dấu xoay vòng");
	updateLabel(labelShortcutInsertChar, "Bật / tắt cho phép chèn ký tự bị thiếu");
	updateLabel(labelShortcutAutoChangeLang, "Bật / tắt tự động chuyển từ tiếng Anh đã ghi nhớ");
}

void GeneralConfig::onSaveButtonClicked() {
	Variable& variable = Variable::getInstance();

	variable.inputMethod = comboInputMethod->currentText().toStdWString();
	variable.modeAutoStart = checkBoxAutoStart->isChecked();
	if (variable.verSigmaExe > 0.0) {
		if (isUserAdmin()) {
			variable.modeAdmin = checkBoxAdmin->isChecked();
		}
		else {
			variable.modeAdmin = false;
		}
	}
	else {
		variable.modeAdmin = variable.MODEADMIN;
	}
	//variable.modeAutoUpdate = checkBoxAutoUpdate->isChecked();
	variable.modeRestore = checkBoxRestore->isChecked();
	variable.modeRemoveDiacTone = checkBoxRemoveDiacTone->isChecked();
	variable.modeInsertChar = checkBoxInsertChar->isChecked();
	variable.modeLoopDiacTone = checkBoxLoopDiacTone->isChecked();
	variable.modeAutoChangeLang = checkBoxAutoChangeLang->isChecked();

	QSettings settings(variable.appName, "Config");
	settings.setValue("inputMethod", QString::fromStdWString(variable.inputMethod));
	settings.setValue("modeAutoStart", variable.modeAutoStart);
	settings.setValue("modeAdmin", variable.modeAdmin);
	createAdminTaskInScheduler(variable.modeAutoStart, variable.modeAdmin);
	//settings.setValue("modeAutoUpdate", variable.modeAutoUpdate);
	settings.setValue("modeRestore", variable.modeRestore);
	settings.setValue("modeRemoveDiacTone", variable.modeRemoveDiacTone);
	settings.setValue("modeLoopDiacTone", variable.modeLoopDiacTone);
	settings.setValue("modeInsertChar", variable.modeInsertChar);
	settings.setValue("modeAutoChangeLang", variable.modeAutoChangeLang);
	variable.update();

	if (variable.modeAdmin && !isRunningAsAdmin()) {
		if (!popup) {
			popup = new CustomMessageBox("Thông báo", this);
		}
		popup->setMessage(QString("Bạn hãy tắt và khởi động lại Sigma để chạy ứng dụng với quyền admin."));
		popup->showWindow();
	}
	hideWindow();
}

void GeneralConfig::onDefaultButtonClicked()
{
	Variable& variable = Variable::getInstance();

	int indexInput = comboInputMethod->findText(QString::fromStdWString(variable.INPUTMETHOD));
	if (indexInput != -1) comboInputMethod->setCurrentIndex(indexInput);

	checkBoxAutoStart->setChecked(variable.MODEAUTOSTART);
	if (variable.verSigmaExe > 0.0) {
		if (isUserAdmin()) {
			checkBoxAdmin->setChecked(variable.MODEADMIN);
		}
		else {
			checkBoxAdmin->setEnabled(false);
		}
	}
	//checkBoxAutoUpdate->setChecked(variable.MODEAUTOUPDATE);
	checkBoxRestore->setChecked(variable.MODERESTORE);
	checkBoxRemoveDiacTone->setChecked(variable.MODEREMOVEDIACTONE);
	checkBoxLoopDiacTone->setChecked(variable.MODELOOPDIACTONE);
	checkBoxInsertChar->setChecked(variable.MODEINSERTCHAR);
	checkBoxAutoChangeLang->setChecked(variable.MODEAUTOCHANGELANG);
}

void GeneralConfig::onCancelButtonClicked() {
	hideWindow();
}

void GeneralConfig::updateCheckBoxStyle(QString modeName, bool modeValue)
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

	QCheckBox* checkBox = nullptr;
	QString stringWarning = "";

	if (modeName == "modeRemoveDiacTone") {
		checkBox = checkBoxRemoveDiacTone;
		if (modeValue) {
			stringWarning = "Phù hợp khi bạn muốn nhanh chóng chuyển từ có dấu sang không dấu hoàn toàn\nchỉ bằng một lần nhấn phím.";
		}
	}

	if (modeName == "modeLoopDiacTone") {
		checkBox = checkBoxLoopDiacTone;
		if (modeValue) {
			stringWarning = "Cho phép bạn gõ chuyển đổi nhiều dấu, bỏ dấu theo kiểu xoay vòng bằng cách\nnhấn phím dấu liên tục. Khi đang ở chế độ gõ tiếng Việt, nếu bạn gõ từ tiếng\nAnh, hãy nhấn phím Ctrl bên trái bàn phím để chuyển chế độ tiếng Anh cho từ\nđang gõ.";
		}
	}

	if (modeName == "modeInsertChar") {
		checkBox = checkBoxInsertChar;
		if (modeValue) {
			stringWarning = "Cho phép bạn chèn thêm ký tự vào đầu hoặc giữa từ đang gõ. Khi đang ở chế độ\ngõ tiếng Việt, nếu bạn gõ từ tiếng Anh, hệ thống có thể nhận nhầm thành từ tiếng\nViệt và không tự chuyển. Trong trường hợp đó, hãy nhấn phím Ctrl bên trái bàn\nphím để chuyển từ đang gõ sang tiếng Anh.";
		}
	}

	if (modeName == "modeAutoChangeLang") {
		checkBox = checkBoxAutoChangeLang;
		if (modeValue) {
			stringWarning = "Hệ thống có thể tự động chuyển từ không theo ý muốn của bạn.\nLúc đó, hãy nhấn phím Ctrl bên trái bàn phím để quay trở lại.";
		}
	}

	if (stringWarning.isEmpty()) {
		checkBox->setStyleSheet(CHECKBOX_DEFAULT_STYLE);
	}
	else {
		checkBox->setStyleSheet(CHECKBOX_WARNING_STYLE);
	}
	checkBox->setToolTip(stringWarning);

}
