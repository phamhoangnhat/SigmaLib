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
		ui->fadeIn();
		ui->show();
		ui->raise();
		ui->activateWindow();
	}
}

void SnippetEditor::hideWindow() {
	if (m_instance && m_instance->isVisible()) {
		if (m_instance->popup) {
			m_instance->popup->closeWindow();
			m_instance->popup.clear();
		}
		m_instance->fadeOut();
	}
}

void SnippetEditor::closeWindow() {
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

	saveBtn = new QPushButton("Lưu và đóng", this);
	cancelBtn = new QPushButton("Hủy", this);
	changeTypeSnippet = new QComboBox(this);

	changeTypeSnippet->addItem("Bảng gõ tắt theo cụm từ");
	changeTypeSnippet->addItem("Bảng gõ tắt tức thì theo chuỗi con");

	saveBtn->setFixedWidth(100);
	cancelBtn->setFixedWidth(100);

	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->addWidget(changeTypeSnippet);
	btnLayout->addWidget(saveBtn);
	btnLayout->addWidget(cancelBtn);


	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(table);
	layout->addLayout(btnLayout);

	setStyleSheet(R"(
        QDialog {
            background-color: #DCDCDC;
            border-radius: 10px;
        }

        QTableWidget {
			background-color: #DADADA;
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

        QLineEdit:focus {
            background-color: #E8E8E8;
            border: 1px solid #FFFFFF;
        }

        QPushButton {
            background-color: #D0D0D0;
            color: black;
            border-radius: 10px;
            font-weight: bold;
            width: 40px;
            min-height: 32px;
			outline: none;
        }

        QPushButton:hover {
            background-color: #C0C0C0;
            border: 1px solid #FFFFFF;
        }

        QPushButton:pressed {
            background-color: #B0B0B0;
        }

		QComboBox {
			background-color: #D0D0D0;
			color: #000;
			font-weight: bold;
			border: 1px solid #aaa;
			border-radius: 10px;
			padding-left: 10px;
            min-width: 80px;
            min-height: 32px;
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

        QScrollBar:vertical, QScrollBar:horizontal {
            background: #E0E0E0;
            border-radius: 5px;
        }

        QScrollBar::handle {
            background: #A0A0A0;
            border-radius: 5px;
        }

        QScrollBar::handle:hover {
            background: #888888;
        }

        QScrollBar::add-line, QScrollBar::sub-line {
            width: 0px;
            height: 0px;
        }

        QScrollBar::add-page, QScrollBar::sub-page {
            background: none;
        }
    )");

	connect(saveBtn, &QPushButton::clicked, this, &SnippetEditor::onSaveButtonClicked);
	connect(cancelBtn, &QPushButton::clicked, this, &SnippetEditor::onCancelButtonClicked);
	connect(changeTypeSnippet, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
		if (isLoading) return;
		saveWindow(index == 1 ? 0 : 1);
		loadWindow(index);
		});
	connect(table, &QTableWidget::itemChanged, this, [=](QTableWidgetItem* item) {
		if (isLoading) return;
		isLoading = true;
		int row = item->row();
		QString col0 = table->item(row, 0) ? table->item(row, 0)->text().trimmed().toLower() : "";
		if (table->item(row, 0)->text() != col0) {
			table->item(row, 0)->setText(col0);
		}

		QString col1 = table->item(row, 1) ? table->item(row, 1)->text().trimmed() : "";

		if (col0.isEmpty() && col1.isEmpty() && row != table->rowCount() - 1) {
			table->removeRow(row);
			isLoading = false;
			return;
		}

		if (row == table->rowCount() - 1 && (!col0.isEmpty() || !col1.isEmpty())) {
			table->insertRow(table->rowCount());
			table->setItem(table->rowCount() - 1, 0, new QTableWidgetItem(""));
			table->setItem(table->rowCount() - 1, 1, new QTableWidgetItem(""));
		}
		isLoading = false;
		});

	loadSnippetFromFile();
	loadWindow(0);
}

SnippetEditor::~SnippetEditor() {}

void SnippetEditor::loadWindow(int index) {
	isLoading = true;

	changeTypeSnippet->setCurrentIndex(index);

	std::vector<std::pair<std::wstring, std::wstring>> listSnippetTemp;
	if (index == 0) {
		listSnippetTemp = listSnippetWords;
	}
	else {
		listSnippetTemp = listSnippetString;
	}

	table->clearContents();
	table->setRowCount(0);

	for (const auto& [key, val] : listSnippetTemp) {
		int row = table->rowCount();
		table->insertRow(row);
		table->setItem(row, 0, new QTableWidgetItem(QString::fromStdWString(key)));
		table->setItem(row, 1, new QTableWidgetItem(QString::fromStdWString(val)));
	}

	int newRow = table->rowCount();
	table->insertRow(newRow);
	table->setItem(newRow, 0, new QTableWidgetItem(""));
	table->setItem(newRow, 1, new QTableWidgetItem(""));

	isLoading = false;
}

void SnippetEditor::saveWindow(int index)
{
	std::vector<std::pair<std::wstring, std::wstring>> listSnippetTemp;

	for (int row = 0; row < table->rowCount(); ++row) {
		QString key = table->item(row, 0) ? table->item(row, 0)->text().trimmed() : "";
		QString val = table->item(row, 1) ? table->item(row, 1)->text().trimmed() : "";

		if (key.isEmpty() && val.isEmpty() && row == table->rowCount() - 1)
			continue;

		std::wstring wKey = key.toStdWString();
		std::wstring wVal = val.toStdWString();
		listSnippetTemp.emplace_back(wKey, wVal);
	}

	if (index == 0) {
		listSnippetWords = std::move(listSnippetTemp);
	}
	else {
		listSnippetString = std::move(listSnippetTemp);
	}
}

void SnippetEditor::findSetCount()
{
	int tempCount;
	maxCount = 0;

	setCountString.clear();
	for (const auto& pair : mapSnippetString) {
		const std::wstring& key = pair.first;
		tempCount = static_cast<int>(key.size());
		setCountString.insert(tempCount);
		maxCount = valueMax(maxCount, tempCount);
	}

	setCountWords.clear();
	for (const auto& pair : mapSnippetWords) {
		const std::wstring& key = pair.first;
		tempCount = static_cast<int>(key.size());
		setCountWords.insert(tempCount);
		maxCount = valueMax(maxCount, tempCount);
	}
}

void SnippetEditor::loadSnippetFromFile() {
	Variable& variable = Variable::getInstance();

	std::map<std::wstring, std::wstring> mapSnippetTemp;
	std::vector<std::pair<std::wstring, std::wstring>> listSnippetTemp;

	QVector<QString> listNameFile = { "SnippetWords.ini", "SnippetString.ini" };
	for (const QString& nameFile : listNameFile) {
		mapSnippetTemp.clear();

		QFile file(getExeDirectory() + "/" + nameFile);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return;

		QTextStream in(&file);

		std::unordered_set<wchar_t> setCharInvalid;
		if (nameFile == "SnippetWords.ini") {
			setCharInvalid = variable.setCharSpaceSnippet;
		}
		else {
			setCharInvalid = { L'=' };
		}

		while (!in.atEnd()) {
			QString line = in.readLine().trimmed();
			if (line.isEmpty() || line.startsWith('[') || !line.contains('='))
				continue;

			QStringList parts = line.split('=', Qt::KeepEmptyParts);
			if (parts.size() < 2) continue;

			QString key = parts[0].trimmed().toLower();
			QString val = parts.mid(1).join("=").trimmed();

			bool flagCharValid = true;
			for (wchar_t wChar : setCharInvalid) {
				if (key.contains(QChar(wChar))) {
					flagCharValid = false;
					continue;
				}
			}
			if (not flagCharValid) {
				continue;
			}

			if (key.isEmpty() || val.isEmpty())
				continue;

			std::wstring wKey = key.toStdWString();
			std::wstring wVal = val.toStdWString();

			if (mapSnippetTemp.find(wKey) != mapSnippetTemp.end()) {
				continue;
			}

			mapSnippetTemp.insert({ wKey, wVal });
			listSnippetTemp.emplace_back(wKey, wVal);

			QString keyTemp;
			QString valTemp;
			if (key.length() >= 2) {
				keyTemp = key;
				valTemp = val;
				keyTemp[0] = keyTemp[0].toUpper();
				keyTemp[1] = keyTemp[1].toUpper();
				wKey = keyTemp.toStdWString();
				wVal = valTemp.toUpper().toStdWString();
				mapSnippetTemp.insert({ wKey, wVal });
			}
			if (key.length() >= 1) {
				keyTemp = key;
				valTemp = val;
				keyTemp[0] = keyTemp[0].toUpper();
				wKey = keyTemp.toStdWString();
				wVal = toTitle(valTemp).toStdWString();
				mapSnippetTemp.insert({ wKey, wVal });
			}
		}

		file.close();

		if (!mapSnippetTemp.empty()) {
			if (nameFile == "SnippetWords.ini") {
				mapSnippetWords = std::move(mapSnippetTemp);
				listSnippetWords = std::move(listSnippetTemp);
			}
			else {
				mapSnippetString = std::move(mapSnippetTemp);
				listSnippetString = std::move(listSnippetTemp);
			}
		}
	}
	findSetCount();
}

void SnippetEditor::onSaveButtonClicked() {
	Variable& variable = Variable::getInstance();

	int index = changeTypeSnippet->currentIndex();
	saveWindow(index);

	std::vector<int> listIndex = { index ,index == 1 ? 0 : 1 };

	for (int index : listIndex) {
		loadWindow(index);

		std::map<std::wstring, std::wstring> mapSnippetTemp;
		std::vector<std::pair<std::wstring, std::wstring>> listSnippetTemp;

		for (int row = 0; row < table->rowCount(); ++row) {
			QString key = table->item(row, 0) ? table->item(row, 0)->text().trimmed() : "";
			QString val = table->item(row, 1) ? table->item(row, 1)->text().trimmed() : "";

			if (key.isEmpty() && val.isEmpty() && row == table->rowCount() - 1)
				continue;

			if (key.isEmpty() || val.isEmpty()) {
				if (!popup) {
					popup = new CustomMessageBox("Lỗi", this);
				}
				popup->setMessage(QString("Dòng %1 không hợp lệ.\nBạn cần nhập cả viết tắt và kết quả.").arg(row + 1));
				popup->showWindow();
				return;
			}

			std::unordered_set<wchar_t> setCharInvalid;
			if (index == 0) {
				setCharInvalid = variable.setCharSpaceSnippet;
			}
			else {
				setCharInvalid = { L'='};
			}
			for (wchar_t wChar : setCharInvalid) {
				if (key.contains(QChar(wChar))) {
					if (!popup) {
						popup = new CustomMessageBox("Lỗi", this);
					}
					popup->setMessage(
						QString("Dòng %1 không hợp lệ.\nPhần viết tắt không được chứa ký tự không hợp lệ: '%2'.")
						.arg(row + 1)
						.arg(QChar(wChar))
					);
					popup->showWindow();
					return;
				}
			}

			std::wstring wKey = key.toStdWString();
			std::wstring wVal = val.toStdWString();

			if (mapSnippetTemp.count(wKey)) {
				if (!popup) {
					popup = new CustomMessageBox("Lỗi", this);
				}
				popup->setMessage(QString("Dòng %1 có viết tắt trùng lặp: \"%2\".").arg(row + 1).arg(key));
				popup->showWindow();
				return;
			}

			mapSnippetTemp.insert({ wKey, wVal });
			listSnippetTemp.emplace_back(wKey, wVal);

			QString keyTemp;
			QString valTemp;
			if (key.length() >= 2) {
				keyTemp = key;
				valTemp = val;
				keyTemp[0] = keyTemp[0].toUpper();
				keyTemp[1] = keyTemp[1].toUpper();
				wKey = keyTemp.toStdWString();
				wVal = valTemp.toUpper().toStdWString();
				mapSnippetTemp.insert({ wKey, wVal });
			}
			if (key.length() >= 1) {
				keyTemp = key;
				valTemp = val;
				keyTemp[0] = keyTemp[0].toUpper();
				wKey = keyTemp.toStdWString();
				wVal = toTitle(valTemp).toStdWString();
				mapSnippetTemp.insert({ wKey, wVal });
			}
		}

		QString nameFile;
		if (index == 0) {
			nameFile = "SnippetWords.ini";
		}
		else {
			nameFile = "SnippetString.ini";
		}

		QFile file(getExeDirectory() + "/" + nameFile);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream out(&file);
		out << "[General]\n";

		for (const auto& [key, val] : listSnippetTemp) {
			out << QString::fromStdWString(key) << "=" << QString::fromStdWString(val) << "\n";
		}

		file.close();


		if (index == 0) {
			mapSnippetWords = std::move(mapSnippetTemp);
			listSnippetWords = std::move(listSnippetTemp);
		}
		else {
			mapSnippetString = std::move(mapSnippetTemp);
			listSnippetString = std::move(listSnippetTemp);
		}
	}

	findSetCount();
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