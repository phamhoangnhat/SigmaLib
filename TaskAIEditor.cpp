#include "TaskAIEditor.h"
#include <Variable.h>
#include <RenameDialog.h>
#include "CustomMessageBox.h"
#include "TaskAIDatabase.h"

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
	addTaskBtn = new QPushButton("");
	removeTaskBtn = new QPushButton("");
	renameTaskBtn = new QPushButton("");
	labelShortcut = new QLabel("");
	taskEditor = new QTextEdit();
	taskEditor->setPlaceholderText("Nhập câu lệnh AI. Dùng {text} để thay thế nội dung văn bản người dùng quét chọn khi thực hiện các tác vụ AI.");

	saveBtn = new QPushButton("Lưu và đóng");
	defaultBtn = new QPushButton("Mặc định");
	cancelBtn = new QPushButton("Hủy");

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addWidget(taskCombo, 1);
	topLayout->addWidget(renameTaskBtn);
	topLayout->addWidget(addTaskBtn);
	topLayout->addWidget(removeTaskBtn);
	topLayout->addWidget(labelShortcut);

	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->addWidget(saveBtn);
	btnLayout->addWidget(defaultBtn);
	btnLayout->addWidget(cancelBtn);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(topLayout);
	layout->addWidget(taskEditor);
	layout->addLayout(btnLayout);

	addTaskBtn->setIcon(QIcon(":/iconAddTaskAI.png"));
	renameTaskBtn->setIcon(QIcon(":/iconRenameTaskAI.png"));
	removeTaskBtn->setIcon(QIcon(":/iconDeleteTaskAI.png"));

	labelShortcut->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	labelShortcut->setFixedWidth(80);

	connect(addTaskBtn, &QPushButton::clicked, this, &TaskAIEditor::onAddTaskClicked);
	connect(renameTaskBtn, &QPushButton::clicked, this, &TaskAIEditor::onRenameTaskClicked);
	connect(removeTaskBtn, &QPushButton::clicked, this, &TaskAIEditor::onRemoveTaskClicked);
	connect(taskCombo, &QComboBox::currentTextChanged, this, &TaskAIEditor::onTaskComboChanged);
	connect(taskEditor, &QTextEdit::textChanged, this, &TaskAIEditor::onEditorChanged);
	connect(saveBtn, &QPushButton::clicked, this, &TaskAIEditor::onSaveButtonClicked);
	connect(defaultBtn, &QPushButton::clicked, this, &TaskAIEditor::onDefaultButtonClicked);
	connect(cancelBtn, &QPushButton::clicked, this, &TaskAIEditor::onCancelButtonClicked);
}

TaskAIEditor::~TaskAIEditor() {}

void TaskAIEditor::loadWindow(const QString& _nameTaskAI) {
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
	isInternalChange = true;

	QString nameTaskAI;
	QString key;
	bool isDefault;

	listNameTaskAITemp = taskAIDatabase.createListNameTaskAI(dataTaskAITemp);
	if (listNameTaskAITemp.isEmpty()) {
		taskCombo->clear();
		taskEditor->clear();
		isDefault = true;
	}
	else {
		taskCombo->clear();
		for (const QString& name : listNameTaskAITemp) {
			taskCombo->addItem(name);
		}

		if (_nameTaskAI.isEmpty()) {
			taskCombo->setCurrentIndex(0);
		}
		else {
			key = _nameTaskAI.toUpper();
			if (dataTaskAITemp.contains(key)) {
				int index = taskCombo->findText(_nameTaskAI);
				if (index != -1) {
					taskCombo->setCurrentIndex(index);
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

		int index = listNameTaskAITemp.indexOf(nameTaskAI) + 1;
		if ((index >= 1) && (index <= 12)) {
			labelShortcut->setText("Shift → F" + QString::number(index));
		}
		else {
			labelShortcut->setText("None");
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
	QString baseName = "Tác vụ";
	int index = 1;
	QString newName;
	QString upperName;

	do {
		newName = QString("%1 %2").arg(baseName).arg(index, 2, 10, QChar('0'));
		upperName = newName.toUpper();
		++index;
	} while (dataTaskAITemp.contains(upperName));

	QString currentContent = taskEditor->toPlainText();
	dataTaskAITemp[upperName] = qMakePair(newName, currentContent);
	loadWindow(newName);
}

void TaskAIEditor::onRenameTaskClicked() {
	QString oldName = taskCombo->currentText();
	if (oldName.isEmpty()) return;

	if (!renameDialog) {
		renameDialog = new RenameDialog(this);

		connect(renameDialog, &QDialog::accepted, this, [this]() {
			QString oldName = renameDialog->oldName;
			QString newName = renameDialog->getNewName();
			QString oldKey = oldName.toUpper();
			QString newKey = newName.toUpper();

			QPair<QString, QString> data = dataTaskAITemp[oldKey];
			data.first = newName;
			dataTaskAITemp.remove(oldKey);
			dataTaskAITemp[newKey] = data;

			loadWindow(newName);
			renameDialog.clear();
			});

		connect(renameDialog, &QDialog::rejected, this, [this]() {
			renameDialog.clear();
			});
	}

	renameDialog->setOldName(oldName);
	renameDialog->showWindow();
}

void TaskAIEditor::onRemoveTaskClicked() {
	QString current = taskCombo->currentText();
	QString key = current.toUpper();

	if (dataTaskAITemp.contains(key)) {
		dataTaskAITemp.remove(key);
		loadWindow();
	}
}

void TaskAIEditor::onTaskComboChanged() {
	if (isInternalChange) return;

	QString currentName = taskCombo->currentText().trimmed();
	loadWindow(currentName);
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
	taskAIDatabase.listNameTaskAI = listNameTaskAITemp;
	taskAIDatabase.saveDataTaskAI();
	hideWindow();
}

void TaskAIEditor::onDefaultButtonClicked() {
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();

	dataTaskAITemp.clear();
	taskAIDatabase.addDataTaskAIDefault(dataTaskAITemp);
	loadWindow();
}

void TaskAIEditor::onCancelButtonClicked() {
	hideWindow();
}
