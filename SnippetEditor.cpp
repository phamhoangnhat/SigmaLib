#include "SnippetEditor.h"
#include "Variable.h"
#include "Util.h"

#include <cstdlib>

#include <QHeaderView>
#include <QIcon>
#include <QDebug>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QChar>

QPointer<SnippetEditor> SnippetEditor::m_instance = nullptr;

SnippetEditor* SnippetEditor::getInstance() {
	if (!m_instance) {
		m_instance = new SnippetEditor();
	}
	return m_instance;
}

void SnippetEditor::showWindow() {
	SnippetEditor* ui = getInstance();
	if (!ui->isVisible()) {
		ui->loadSnippetFromFile();
		ui->loadNameSnippetCombo();
		ui->loadTable();

		ui->fadeIn();
		ui->show();
		ui->raise();
		ui->activateWindow();
	}
}

void SnippetEditor::hideWindow() {
	if (m_instance && m_instance->isVisible()) {
		if (m_instance->renameDialog) {
			m_instance->renameDialog->closeWindow();
			m_instance->renameDialog.clear();
		}
		m_instance->fadeOut();
	}
}

void SnippetEditor::closeWindow() {
	if (m_instance) {
		if (m_instance->renameDialog) {
			m_instance->renameDialog->closeWindow();
			m_instance->renameDialog.clear();
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

void SnippetEditor::closeEvent(QCloseEvent* event) {
	event->ignore();
	hideWindow();
}

void SnippetEditor::reject() {
	hideWindow();
}

SnippetEditor::SnippetEditor(QWidget* parent)
	: QDialog(parent) {
	setWindowTitle("Trình quản lý gõ tắt");
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
	setWindowIcon(QIcon(":/icon.png"));
	resize(500, 400);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowOpacity(0.0);

	nameSnippetCombo = new QComboBox();
	addSnippetBtn = new QPushButton("");
	removeSnippetBtn = new QPushButton("");
	renameSnippetBtn = new QPushButton("");

	renameSnippetBtn->setIcon(QIcon(":/iconRenameItem.png"));
	addSnippetBtn->setIcon(QIcon(":/iconAddItem.png"));
	removeSnippetBtn->setIcon(QIcon(":/iconDeleteItem.png"));

	table = new QTableWidget(this);
	table->setColumnCount(2);
	table->setHorizontalHeaderLabels({ "Viết tắt", "Kết quả" });
	table->horizontalHeader()->setStretchLastSection(true);
	table->verticalHeader()->setVisible(false);
	table->setEditTriggers(QAbstractItemView::AllEditTriggers);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->setShowGrid(false);
	table->horizontalHeader()->setFixedHeight(25);
	table->verticalHeader()->setDefaultSectionSize(20);
	table->horizontalHeader()->setHighlightSections(false);

	saveBtn = new QPushButton("Lưu và đóng");
	cancelBtn = new QPushButton("Hủy");

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addWidget(nameSnippetCombo, 1);
	topLayout->addWidget(renameSnippetBtn);
	topLayout->addWidget(addSnippetBtn);
	topLayout->addWidget(removeSnippetBtn);

	QHBoxLayout* bottomLayout = new QHBoxLayout();
	bottomLayout->addWidget(saveBtn);
	bottomLayout->addWidget(cancelBtn);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(topLayout);
	layout->addWidget(table);
	layout->addLayout(bottomLayout);

	setStyleSheet(R"(
        QDialog {
            background-color: #DCDCDC;
            border-radius: 10px;
        }

        QTableWidget {
			background-color: #DADADA;
			color: #000000;
            border: 1px solid #AAAAAA;
            border-radius: 10px;
            font-size: 12px;
            font-weight: bold;
            gridline-color: #E0E0E0;
            selection-background-color: #E0E0E0;
            selection-color: #000000;
			outline: none;
        }

        QTableWidget::item {
            padding: 0px 10px;
            border: none;
			color: #000000;
        }

        QTableWidget::item:selected {
            background-color: #DADADA;
            color: #000000;
        }

        QHeaderView::section {
            background-color: #CCCCCC;
            color: #222222;
            font-weight: bold;
            font-size: 13px;
            padding: 5px;
            border: none;
            border-bottom: 1px solid #CCCCCC;
        }

        QLineEdit {
            background-color: #EEEEEE;
            color: #000000;
            border: 1px solid #BBBBBB;
            font-weight: bold;
            border-radius: 5px;
            padding: 0px 0px;
            min-height: 20px;
        }

        QLineEdit:hover {
            border: 1px solid #FFFFFF;
        }

        QLineEdit:focus {
            background-color: #E8E8E8;
            border: 1px solid #FFFFFF;
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
    )");

	connect(nameSnippetCombo, &QComboBox::currentTextChanged, this, &SnippetEditor::onNameSnippetComboChanged);
	connect(renameSnippetBtn, &QPushButton::clicked, this, &SnippetEditor::onRenameSnippetButtonClicked);
	connect(addSnippetBtn, &QPushButton::clicked, this, &SnippetEditor::onAddSnippetButtonClicked);
	connect(removeSnippetBtn, &QPushButton::clicked, this, &SnippetEditor::onRemoveSnippetButtonClicked);
	connect(table, &QTableWidget::itemChanged, this, &SnippetEditor::onTableChanged);
	connect(saveBtn, &QPushButton::clicked, this, &SnippetEditor::onsaveButtonClicked);
	connect(cancelBtn, &QPushButton::clicked, this, &SnippetEditor::onCancelButtonClicked);

	loadSnippetFromFile();
}

SnippetEditor::~SnippetEditor() {}

void SnippetEditor::loadSnippetFromFile() {
	std::map<std::wstring, std::wstring> snippetTemp;
	std::map<QString, std::map<std::wstring, std::wstring>> dataSnippetTemp;
	QString nameSnippetTemp;
	QString nameSnippetUpperTemp;
	listNameSnippetUpper.clear();

	QFile file(getExeDirectory() + "/Snippet.ini");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&file);

		while (!in.atEnd()) {
			QString line = in.readLine().trimmed();
			if (line.isEmpty() || !line.contains('='))
				continue;

			if (line.startsWith("=")) {
				QString stringTemp = line.mid(1).trimmed();
				if (!stringTemp.isEmpty()) {
					if (!nameSnippetTemp.isEmpty() &&
						(nameSnippetUpperTemp != "Không sử dụng") &&
						!listNameSnippetUpper.contains(nameSnippetUpperTemp) &&
						!snippetTemp.empty())
					{
						dataSnippetTemp[nameSnippetTemp] = snippetTemp;
						listNameSnippetUpper.insert(nameSnippetUpperTemp);
					}
				}
				snippetTemp.clear();
				nameSnippetTemp = stringTemp;
				nameSnippetUpperTemp = nameSnippetTemp.toUpper();
				continue;
			}

			QStringList parts = line.split('=', Qt::KeepEmptyParts);
			if (parts.size() < 2) {
				continue;
			}

			QString key = parts[0].trimmed().toLower();
			QString val = parts.mid(1).join("=").trimmed();

			if (!key.isEmpty() &&
				!key.contains(' ') &&
				(snippetTemp.find(key.toStdWString()) == snippetTemp.end()) &&
				!val.isEmpty())
			{
				snippetTemp[key.toStdWString()] = val.toStdWString();
				continue;
			}
		}
		file.close();
	}

	if (!nameSnippetTemp.isEmpty() &&
		(nameSnippetUpperTemp != "Không sử dụng") &&
		!listNameSnippetUpper.contains(nameSnippetUpperTemp) &&
		!snippetTemp.empty())
	{
		dataSnippetTemp[nameSnippetTemp] = snippetTemp;
		listNameSnippetUpper.insert(nameSnippetUpperTemp);
	}

	if (nameSnippetTemp.isEmpty()) {
		snippetTemp.clear();
		nameSnippetTemp = "Danh sách gõ tắt 01";
		nameSnippetUpperTemp = nameSnippetTemp.toUpper();
		snippetTemp[L"_phi"] = L"Φ";
		snippetTemp[L"_pi"] = L"π";
		snippetTemp[L"_sig"] = L"∑";
		dataSnippetTemp[nameSnippetTemp] = snippetTemp;
		listNameSnippetUpper.insert(nameSnippetUpperTemp);

		snippetTemp.clear();
		nameSnippetTemp = "Danh sách gõ tắt 02";
		nameSnippetUpperTemp = nameSnippetTemp.toUpper();
		snippetTemp[L"chxh"] = L"Cộng Hòa Xã Hội Chủ Nghĩa Việt Nam";
		snippetTemp[L"dltd"] = L"Độc lập - Tự Do - Hạnh Phúc";
		snippetTemp[L"tphcm"] = L"thành phố Hồ Chí minh";
		dataSnippetTemp[nameSnippetTemp] = snippetTemp;
		listNameSnippetUpper.insert(nameSnippetUpperTemp);
	}
	dataSnippet = std::move(dataSnippetTemp);

	dataSnippetTable.clear();
	for (const auto& [nameSnippetTemp, snippetTemp] : dataSnippet) {
		QList<QPair<QString, QString>> snippetTable;

		for (const auto& [keyW, valueW] : snippetTemp) {
			QString key = QString::fromStdWString(keyW);
			QString value = QString::fromStdWString(valueW);
			snippetTable.append(qMakePair(key, value));
		}

		dataSnippetTable[nameSnippetTemp] = snippetTable;
	}
	updateTotal();

	nameSnippet = dataSnippet.begin()->first;
}

void SnippetEditor::updateTotal()
{
	listNameSnippet.clear();
	mapCountSnippet.clear();
	dataSnippetTotal = dataSnippet;

	QString keyTemp;
	QString valTemp;
	std::wstring key;
	std::wstring val;
	std::set<int> setCount;
	int count;
	for (const auto& [nameSnippetTemp, snippetTemp] : dataSnippet) {
		setCount.clear();
		for (const auto& [keyW, valW] : snippetTemp) {
			keyTemp = QString::fromStdWString(keyW);
			valTemp = QString::fromStdWString(valW);
			count = keyTemp.length();
			setCount.insert(count);
			if (count >= 1) {
				keyTemp[0] = keyTemp[0].toUpper();
				key = keyTemp.toStdWString();
				val = toTitle(valTemp).toStdWString();
				dataSnippetTotal[nameSnippetTemp].insert({ key, val });
			}
			if (count >= 2) {
				keyTemp[0] = keyTemp[0].toLower();
				keyTemp[1] = keyTemp[1].toUpper();
				key = keyTemp.toStdWString();
				val = valTemp.toUpper().toStdWString();
				dataSnippetTotal[nameSnippetTemp].insert({ key, val });
			}
		}
		mapCountSnippet[nameSnippetTemp] = setCount;
		listNameSnippet.insert(nameSnippetTemp);
	}
}

void SnippetEditor::loadNameSnippetCombo() {
	isLoading = true;

	QStringList listNameSnippetTemp = dataSnippetTable.keys();
	std::sort(listNameSnippetTemp.begin(), listNameSnippetTemp.end(), [](const QString& a, const QString& b) {
		return a.compare(b, Qt::CaseInsensitive) < 0;
		});

	nameSnippetCombo->clear();
	for (const auto& nameSnippetTemp : listNameSnippetTemp) {
		nameSnippetCombo->addItem(nameSnippetTemp);
	}

	int index = nameSnippetCombo->findText(nameSnippet, Qt::MatchFixedString);
	if (index >= 0) {
		nameSnippetCombo->setCurrentIndex(index);
	}

	isLoading = false;
}

void SnippetEditor::loadSnippetForApp(QString nameSnippetString, QString nameSnippetWords)
{
	maxCount = 0;

	if (listNameSnippet.contains(nameSnippetString)) {
		mapSnippetString = dataSnippetTotal[nameSnippetString];
		setCountString = mapCountSnippet[nameSnippetString];
		maxCount = valueMax(maxCount, *setCountString.rbegin());
	}
	else {
		mapSnippetString.clear();
		setCountString.clear();
	}

	if (listNameSnippet.contains(nameSnippetWords)) {
		mapSnippetWords = dataSnippetTotal[nameSnippetWords];
		setCountWords = mapCountSnippet[nameSnippetWords];
		maxCount = valueMax(maxCount, *setCountWords.rbegin());
	}
	else {
		mapSnippetWords.clear();
		setCountWords.clear();
	}
}

void SnippetEditor::onNameSnippetComboChanged()
{
	if (isLoading) return;
	nameSnippet = nameSnippetCombo->currentText();
	loadTable();
}

void SnippetEditor::onRenameSnippetButtonClicked()
{
	QString oldName = nameSnippetCombo->currentText();
	if (oldName.isEmpty()) return;

	if (!renameDialog) {
		renameDialog = new RenameSnippetDialog(this);

		connect(renameDialog, &QDialog::accepted, this, [this]() {
			QString oldName = renameDialog->oldName;
			QString newName = renameDialog->getNewName();

			QList<QPair<QString, QString>> snippetTable = dataSnippetTable[oldName];
			dataSnippetTable.remove(oldName);
			dataSnippetTable[newName] = snippetTable;
			listNameSnippetUpper.insert(newName.toUpper());
			renameDialog.clear();

			nameSnippet = newName;
			loadNameSnippetCombo();
			});

		connect(renameDialog, &QDialog::rejected, this, [this]() {
			renameDialog.clear();
			});
	}

	renameDialog->setOldName(oldName);
	renameDialog->showWindow();
}

void SnippetEditor::onAddSnippetButtonClicked()
{
	QString baseName = "Danh sách gõ tắt";
	int index = 1;
	QString newName;
	QString upperName;

	do {
		newName = QString("%1 %2").arg(baseName).arg(index, 2, 10, QChar('0'));
		upperName = newName.toUpper();
		++index;
	} while (listNameSnippetUpper.contains(upperName));

	QList<QPair<QString, QString>> snippetTable;
	if (!nameSnippet.isEmpty()) {
		snippetTable = dataSnippetTable[nameSnippet];
	}

	nameSnippet = newName;
	listNameSnippetUpper.insert(upperName);
	dataSnippetTable[nameSnippet] = snippetTable;

	loadNameSnippetCombo();
	loadTable();
}

void SnippetEditor::onRemoveSnippetButtonClicked()
{
	QString nameSnippetUpper = nameSnippet.toUpper();
	if (!listNameSnippetUpper.contains(nameSnippetUpper)) return;

	listNameSnippetUpper.remove(nameSnippetUpper);
	dataSnippetTable.remove(nameSnippet);

	if (!dataSnippetTable.isEmpty()) {
		nameSnippet = dataSnippetTable.firstKey();
	}
	else {
		nameSnippet.clear();
	}

	loadNameSnippetCombo();
	loadTable();
}

void SnippetEditor::loadTable()
{
	isLoading = true;

	const QList<QPair<QString, QString>>& snippetTable = dataSnippetTable[nameSnippet];
	table->clearContents();
	table->setRowCount(0);

	for (const auto& pair : snippetTable) {
		int numRow = table->rowCount();
		table->insertRow(numRow);
		table->setItem(numRow, 0, new QTableWidgetItem(pair.first));
		table->setItem(numRow, 1, new QTableWidgetItem(pair.second));
	}

	int numRow = table->rowCount();
	table->insertRow(numRow);
	table->setItem(numRow, 0, new QTableWidgetItem(""));
	table->setItem(numRow, 1, new QTableWidgetItem(""));

	isLoading = false;
}

void SnippetEditor::onTableChanged(QTableWidgetItem* item) {
	if (isLoading) return;
	isLoading = true;

	int numRow = item->row();
	QString key = table->item(numRow, 0) ? table->item(numRow, 0)->text().trimmed().remove(' ').toLower(): "";
	QString value = table->item(numRow, 1) ? table->item(numRow, 1)->text().trimmed() : "";

	if (table->item(numRow, 0)->text() != key) {
		table->item(numRow, 0)->setText(key);
	}

	if (key.isEmpty() && value.isEmpty() && numRow != table->rowCount() - 1) {
		table->removeRow(numRow);
		if (numRow < dataSnippetTable[nameSnippet].size()) {
			dataSnippetTable[nameSnippet].removeAt(numRow);
		}
		isLoading = false;
		return;
	}

	if ((!key.isEmpty() || !value.isEmpty()) && numRow == table->rowCount() - 1) {
		table->insertRow(table->rowCount());
		table->setItem(table->rowCount() - 1, 0, new QTableWidgetItem(""));
		table->setItem(table->rowCount() - 1, 1, new QTableWidgetItem(""));
		dataSnippetTable[nameSnippet].append(qMakePair(key, value));
		isLoading = false;
		return;
	}

	if (numRow < dataSnippetTable[nameSnippet].size()) {
		dataSnippetTable[nameSnippet][numRow].first = key;
		dataSnippetTable[nameSnippet][numRow].second = value;
	}

	isLoading = false;
}

void SnippetEditor::onsaveButtonClicked()
{
	std::map<std::wstring, std::wstring> snippetTemp;
	std::map<QString, std::map<std::wstring, std::wstring>> dataSnippetTemp;
	QString nameSnippetUpperTemp;
	listNameSnippetUpper.clear();

	for (const auto& nameSnippetTemp : dataSnippetTable.keys()) {
		const QList<QPair<QString, QString>>& snippetTable = dataSnippetTable[nameSnippetTemp];
		nameSnippetUpperTemp = nameSnippetTemp.toUpper();
		snippetTemp.clear();
		for (const auto& pair : snippetTable) {
			QString key = pair.first.trimmed().toLower();
			QString val = pair.second.trimmed();
			if (!key.isEmpty() &&
				!key.contains(' ') &&
				(snippetTemp.find(key.toStdWString()) == snippetTemp.end()) &&
				!val.isEmpty())
			{
				snippetTemp[key.toStdWString()] = val.toStdWString();
			}
		}
		if (!nameSnippetTemp.isEmpty() &&
			(nameSnippetUpperTemp != "Không sử dụng") &&
			!listNameSnippetUpper.contains(nameSnippetUpperTemp) &&
			!snippetTemp.empty())
		{
			dataSnippetTemp[nameSnippetTemp] = snippetTemp;
			listNameSnippetUpper.insert(nameSnippetUpperTemp);
		}
	}
	dataSnippet = std::move(dataSnippetTemp);

	QFile file(getExeDirectory() + "/Snippet.ini");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);
	for (const auto& [nameSnippetTemp, snippetTemp] : dataSnippet) {
		out << "=" << nameSnippetTemp << "\n";
		for (const auto& [keyW, valueW] : snippetTemp) {
			out << QString::fromStdWString(keyW) << "=" << QString::fromStdWString(valueW) << "\n";
		}
	}
	file.close();

	updateTotal();
	hideWindow();
}

void SnippetEditor::onCancelButtonClicked() {
	hideWindow();
}

void SnippetEditor::fadeIn() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(1000);
	anim->setStartValue(0.0);
	anim->setEndValue(0.95);
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void SnippetEditor::fadeOut() {
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