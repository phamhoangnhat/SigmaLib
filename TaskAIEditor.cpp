#include "TaskAIEditor.h"
#include <Variable.h>
#include <RenameDialog.h>
#include "CustomMessageBox.h"
#include "TaskAIDatabase.h"
#include "KeyAPIManage.h"
#include "TabRightDelegate.h"

#include <QApplication>
#include <QJsonValue>
#include <QMessageBox>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QSettings>
#include <QStringList>
#include <QPointer>
#include <QAbstractItemView>

QPointer<TaskAIEditor> TaskAIEditor::m_instance = nullptr;

TaskAIEditor* TaskAIEditor::getInstance() {
	if (!m_instance) {
		m_instance = new TaskAIEditor();
	}
	return m_instance;
}

void TaskAIEditor::showWindow() {
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
	TaskAIEditor* ui = getInstance();
	if (!ui->isVisible()) {
		ui->dataTaskAITemp = taskAIDatabase.dataTaskAI;
		ui->dataModelAITemp = taskAIDatabase.dataModelAI;
		ui->dataShortcutAITemp = taskAIDatabase.dataShortcutAI;
		ui->dataShortcutAICheckTemp = taskAIDatabase.dataShortcutAICheck;
		ui->listNameTaskAITemp = taskAIDatabase.listNameTaskAI;
		ui->fadeIn();
		ui->show();
		ui->raise();
		ui->activateWindow();
		ui->loadWindow();
	}
}

void TaskAIEditor::hideWindow() {
	if (m_instance && m_instance->isVisible()) {
		if (m_instance->renameDialog) {
			m_instance->renameDialog->closeWindow();
			m_instance->renameDialog.clear();
		}
		if (m_instance->popup) {
			m_instance->popup->closeWindow();
			m_instance->popup.clear();
		}
		m_instance->fadeOut();
	}
}

void TaskAIEditor::closeWindow() {
	if (m_instance) {
		if (m_instance->renameDialog) {
			m_instance->renameDialog->closeWindow();
			m_instance->renameDialog.clear();
		}
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

void TaskAIEditor::closeEvent(QCloseEvent* event) {
	event->ignore();
	hideWindow();
}

void TaskAIEditor::reject() {
	hideWindow();
}

TaskAIEditor::TaskAIEditor(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("Trình chỉnh sửa tác vụ AI");
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
	setWindowIcon(QIcon(":/icon.png"));
	resize(500, 300);

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor("#E0E0E0"));
	palette.setColor(QPalette::WindowText, Qt::black);
	this->setPalette(palette);

	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowOpacity(0.0);

	setStyleSheet(R"(
        QDialog { background-color: #E0E0E0; }

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
			border-radius: 10px;
			font-weight: normal;
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

        QPushButton {
            background-color: #D0D0D0;
            color: black;
            border-radius: 10px;
            font-weight: bold;
            min-width: 28px;
            min-height: 28px;
			outline: none;
        }

        QPushButton:hover {
            background-color: #c0c0c0;
            border: 1px solid #FFFFFF;
        }

        QPushButton:pressed {
            background-color: #B0B0B0;
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

        QTextEdit {
            background-color: #DADADA;
			color: #000000;
            font-weight: bold;
            border: 1px solid #999;
            border-radius: 10px;
            padding: 10px;
        }

        QTextEdit:hover {
            background-color: #CCCCCC;
            border: 1px solid #FFFFFF;
        }

		QTextEdit:disabled {
			font-weight: bold;
		}
    )");
	
	taskCombo = new QComboBox();
	modelCombo = new QComboBox();
	addTaskBtn = new QPushButton("");
	removeTaskBtn = new QPushButton("");
	renameTaskBtn = new QPushButton("");
	shortcutCombo = new QComboBox();
	taskEditor = new QTextEdit();
	taskEditor->setPlaceholderText("Nhập câu lệnh AI. Dùng {text} để thay thế nội dung văn bản người dùng quét chọn khi thực hiện các tác vụ AI.");

	saveBtn = new QPushButton("Lưu và đóng");
	defaultBtn = new QPushButton("Mặc định");
	cancelBtn = new QPushButton("Hủy");
	keyAPIBtn = new QPushButton("Khóa API Gemini");

	QHBoxLayout* firstLayout = new QHBoxLayout();
	firstLayout->addWidget(taskCombo, 1);
	firstLayout->addWidget(renameTaskBtn);
	firstLayout->addWidget(addTaskBtn);
	firstLayout->addWidget(removeTaskBtn);

	QHBoxLayout* secondLayout = new QHBoxLayout();
	secondLayout->addWidget(modelCombo, 1);
	secondLayout->addWidget(shortcutCombo);

	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->addWidget(saveBtn);
	btnLayout->addWidget(defaultBtn);
	btnLayout->addWidget(cancelBtn);
	btnLayout->addWidget(keyAPIBtn);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(firstLayout);
	layout->addLayout(secondLayout);
	layout->addWidget(taskEditor);
	layout->addLayout(btnLayout);

	addTaskBtn->setIcon(QIcon(":/iconAddItem.png"));
	renameTaskBtn->setIcon(QIcon(":/iconRenameItem.png"));
	removeTaskBtn->setIcon(QIcon(":/iconDeleteItem.png"));

	taskCombo->view()->setItemDelegate(new TabRightDelegate(taskCombo));
	const int buttonsWidth =
		addTaskBtn->sizeHint().width() +
		renameTaskBtn->sizeHint().width() +
		removeTaskBtn->sizeHint().width();

	const int gaps = firstLayout->spacing() * 2;
	shortcutCombo->setFixedWidth(buttonsWidth + gaps);

	connect(taskCombo, &QComboBox::currentTextChanged, this, &TaskAIEditor::onTaskComboChanged);
	connect(modelCombo, &QComboBox::currentTextChanged, this, &TaskAIEditor::onModelComboChanged);
	connect(shortcutCombo, &QComboBox::currentTextChanged, this, &TaskAIEditor::onShortcutComboChanged);
	connect(renameTaskBtn, &QPushButton::clicked, this, &TaskAIEditor::onRenameTaskClicked);
	connect(addTaskBtn, &QPushButton::clicked, this, &TaskAIEditor::onAddTaskClicked);
	connect(removeTaskBtn, &QPushButton::clicked, this, &TaskAIEditor::onRemoveTaskClicked);
	connect(taskEditor, &QTextEdit::textChanged, this, &TaskAIEditor::onEditorChanged);
	connect(saveBtn, &QPushButton::clicked, this, &TaskAIEditor::onSaveButtonClicked);
	connect(defaultBtn, &QPushButton::clicked, this, &TaskAIEditor::onDefaultButtonClicked);
	connect(cancelBtn, &QPushButton::clicked, this, &TaskAIEditor::onCancelButtonClicked);
	connect(keyAPIBtn, &QPushButton::clicked, this, &TaskAIEditor::onKeyAPIButtonClicked);
}

TaskAIEditor::~TaskAIEditor() {}

void TaskAIEditor::loadWindow(const QString& _nameTaskAI) {
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
	isInternalChange = true;

	QString nameTaskAI;
	QString key;
	bool isDefault;

	listNameTaskAITemp = taskAIDatabase.createListNameTaskAI(dataShortcutAITemp, dataShortcutAICheckTemp);
	if (listNameTaskAITemp.isEmpty()) {
		taskCombo->clear();
		modelCombo->clear();
		shortcutCombo->clear();
		taskEditor->clear();
		isDefault = true;
	}
	else {
		taskCombo->clear();
		for (const QString& nameTaskAI : listNameTaskAITemp) {
			taskCombo->addItem(nameTaskAI);
			int row = taskCombo->count() - 1;
			QString shortcut = dataShortcutAITemp[nameTaskAI];
			if (shortcut == "None") {
				shortcut = QString();
			}
			taskCombo->setItemData(row, shortcut, Qt::UserRole);
		}

		modelCombo->clear();
		for (const QString& model : taskAIDatabase.listNameModel) {
			modelCombo->addItem(model);
		}

		shortcutCombo->clear();
		for (const QString& shortcut : taskAIDatabase.listShortcutAI) {
			shortcutCombo->addItem(shortcut);
		}

		if (_nameTaskAI.isEmpty()) {
			taskCombo->setCurrentIndex(0);
		}
		else {
			key = _nameTaskAI.toUpper();
			if (dataTaskAITemp.contains(key)) {
				int indexTask = taskCombo->findText(_nameTaskAI);
				if (indexTask != -1) {
					taskCombo->setCurrentIndex(indexTask);
				}
				else {
					taskCombo->setCurrentIndex(0);
				}
			}
			else {
				taskCombo->setCurrentIndex(0);
			}
		}
		nameTaskAI = taskCombo->currentText();
		key = nameTaskAI.toUpper();
		taskEditor->setPlainText(dataTaskAITemp[key].second);
		isDefault = taskAIDatabase.listNameDefaultTaskAI.contains(key);

		QString model = dataModelAITemp[nameTaskAI];
		int indexModel = taskAIDatabase.listNameModel.indexOf(model);
		if (indexModel != -1) {
			modelCombo->setCurrentIndex(indexModel);
		}
		else {
			modelCombo->setCurrentIndex(0);
		}

		QString shortcut = dataShortcutAITemp[nameTaskAI];
		int indexShortcut = taskAIDatabase.listShortcutAI.indexOf(shortcut);
		if (indexShortcut != -1) {
			shortcutCombo->setCurrentIndex(indexShortcut);
		}
		else {
			shortcutCombo->setCurrentIndex(0);
		}
	}

	taskEditor->setReadOnly(isDefault);
	renameTaskBtn->setEnabled(!isDefault);
	removeTaskBtn->setEnabled(!isDefault);

	if (isDefault) {
		taskEditor->setStyleSheet(R"(
			QTextEdit {
				background-color: #DADADA;
				color: #888888;
				font-weight: normal;
				border: 1px solid #999;
				border-radius: 10px;
				padding: 10px;
			}
		)");
	}
	else {
		taskEditor->setStyleSheet(R"(
			QTextEdit {
				background-color: #DADADA;
				color: #000000;
				font-weight: bold;
				border: 1px solid #999;
				border-radius: 10px;
				padding: 10px;
			}

			QTextEdit:hover {
				background-color: #CCCCCC;
				border: 1px solid #FFFFFF;
			}
		)");
	}

	isInternalChange = false;
}

void TaskAIEditor::fadeIn() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(1000);
	anim->setStartValue(0.0);
	anim->setEndValue(0.95);
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void TaskAIEditor::fadeOut() {
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

void TaskAIEditor::onAddTaskClicked() {
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();

	QString baseName = "Tác vụ";
	int index = 1;
	QString nameTaskAI;
	QString key;

	do {
		nameTaskAI = QString("%1 %2").arg(baseName).arg(index, 2, 10, QChar('0'));
		key = nameTaskAI.toUpper();
		++index;
	} while (dataTaskAITemp.contains(key));

	QString prompt = taskEditor->toPlainText();
	dataTaskAITemp[key] = qMakePair(nameTaskAI, prompt);

	QString nameModel = modelCombo->currentText();
	dataModelAITemp[nameTaskAI] = nameModel;

	bool assigned = false;
	for (int i = 1; i <= 12; i++) {
		QString shortcut = taskAIDatabase.listShortcutAI[i];
		if (dataShortcutAICheckTemp[shortcut].isEmpty()) {
			dataShortcutAITemp.insert(nameTaskAI, shortcut);
			dataShortcutAICheckTemp[shortcut] = nameTaskAI;
			assigned = true;
			break;
		}
	}
	if (!assigned) {
		dataShortcutAITemp.insert(nameTaskAI, "None");
	}

	loadWindow(nameTaskAI);
}

void TaskAIEditor::onRenameTaskClicked() {
	QString nameTaskAIOld = taskCombo->currentText();
	if (nameTaskAIOld.isEmpty()) return;

	if (!renameDialog) {
		renameDialog = new RenameDialog(this);

		connect(renameDialog, &QDialog::accepted, this, [this]() {
			QString nameTaskAIOld = renameDialog->oldName;
			QString nameTaskAINew = renameDialog->getNewName();
			QString oldKey = nameTaskAIOld.toUpper();
			QString newKey = nameTaskAINew.toUpper();
			QString nameModel = dataModelAITemp[nameTaskAIOld];
			QString shortcut = dataShortcutAITemp[nameTaskAIOld];

			QPair<QString, QString> data = dataTaskAITemp[oldKey];
			data.first = nameTaskAINew;
			dataTaskAITemp.remove(oldKey);
			dataTaskAITemp[newKey] = data;

			dataModelAITemp.remove(nameTaskAIOld);
			dataModelAITemp[nameTaskAINew] = nameModel;

			dataShortcutAITemp.remove(nameTaskAIOld);
			dataShortcutAITemp[nameTaskAINew] = shortcut;
			if (shortcut != "None") {
				dataShortcutAICheckTemp[shortcut] = nameTaskAINew;
			}

			loadWindow(nameTaskAINew);
			renameDialog.clear();
			});

		connect(renameDialog, &QDialog::rejected, this, [this]() {
			renameDialog.clear();
			});
	}

	renameDialog->setOldName(nameTaskAIOld);
	renameDialog->showWindow();
}

void TaskAIEditor::onRemoveTaskClicked() {
	QString nameTaskAI = taskCombo->currentText();
	QString key = nameTaskAI.toUpper();

	if (dataTaskAITemp.contains(key)) {
		dataTaskAITemp.remove(key);

		dataModelAITemp.remove(nameTaskAI);

		QString shortcut = dataShortcutAITemp[nameTaskAI];
		dataShortcutAITemp.remove(nameTaskAI);
		if (dataShortcutAICheckTemp.contains(shortcut)) {
			dataShortcutAICheckTemp[shortcut] = QString();
		}

		loadWindow();
	}
}

void TaskAIEditor::onTaskComboChanged() {
	if (isInternalChange) return;

	QString nameTaskAI = taskCombo->currentText();
	loadWindow(nameTaskAI);
}

void TaskAIEditor::onModelComboChanged()
{
	if (isInternalChange) return;

	QString nameTaskAI = taskCombo->currentText();
	QString nameModel = modelCombo->currentText();
	dataModelAITemp[nameTaskAI] = nameModel;
}

void TaskAIEditor::onShortcutComboChanged()
{
	if (isInternalChange) return;

	QString nameTaskAICurrent = taskCombo->currentText();
	QString shortcutCurrent = dataShortcutAITemp[nameTaskAICurrent];
	QString shortcutNew = shortcutCombo->currentText();
	if (shortcutNew != "None") {
		QString nameTaskAIChange = dataShortcutAICheckTemp[shortcutNew];
		if (!nameTaskAIChange.isEmpty()) {
			dataShortcutAITemp[nameTaskAIChange] = "None";
		}
		dataShortcutAICheckTemp[shortcutNew] = nameTaskAICurrent;
	}
	dataShortcutAITemp[nameTaskAICurrent] = shortcutNew;
	if (shortcutCurrent != "None") {
		dataShortcutAICheckTemp[shortcutCurrent] = QString();
	}

	loadWindow(nameTaskAICurrent);
}

void TaskAIEditor::onEditorChanged() {
	QString currentName = taskCombo->currentText();
	QString key = currentName.toUpper();
	if (dataTaskAITemp.contains(key)) {
		dataTaskAITemp[key].second = taskEditor->toPlainText();
	}
}

void TaskAIEditor::onSaveButtonClicked() {
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();

	for (auto it = dataTaskAITemp.begin(); it != dataTaskAITemp.end(); ++it) {
		QString& name = it.value().first;
		QString& content = it.value().second;

		QString lowerContent = content.toLower();
		int index = lowerContent.indexOf("{text}");

		if (index == -1) {
			taskCombo->setCurrentText(name);
			taskEditor->setPlainText(content);

			if (!popup) {
				popup = new CustomMessageBox("Lỗi xác thực", this);
			}
			popup->setMessage(QString("Tác vụ \"%1\" đang thiếu trường {text}.").arg(name));
			popup->showWindow();
			return;
		}
		else {
			content.replace(index, 6, "{text}");
			it.value().second = content;
		}
	}

	taskAIDatabase.dataTaskAI = dataTaskAITemp;
	taskAIDatabase.dataModelAI = dataModelAITemp;
	taskAIDatabase.listNameTaskAI = listNameTaskAITemp;
	taskAIDatabase.dataShortcutAI = dataShortcutAITemp;
	taskAIDatabase.dataShortcutAICheck = dataShortcutAICheckTemp;
	taskAIDatabase.saveDataTaskAI();
	hideWindow();
}

void TaskAIEditor::onDefaultButtonClicked() {
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
	taskAIDatabase.addDataTaskAIDefault(dataTaskAITemp, dataModelAITemp, dataShortcutAITemp, dataShortcutAICheckTemp);
	listNameTaskAITemp = taskAIDatabase.createListNameTaskAI(dataShortcutAITemp, dataShortcutAICheckTemp);
	loadWindow();
}

void TaskAIEditor::onCancelButtonClicked() {
	hideWindow();
}

void TaskAIEditor::onKeyAPIButtonClicked()
{
	KeyAPIManage::showWindow();
}
