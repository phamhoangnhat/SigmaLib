#include "KeyAPIManage.h"
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
#include <QVBoxLayout>
#include <QDesktopServices>

QPointer<KeyAPIManage> KeyAPIManage::m_instance = nullptr;

KeyAPIManage* KeyAPIManage::getInstance() {
	if (!m_instance) {
		m_instance = new KeyAPIManage();
	}
	return m_instance;
}

void KeyAPIManage::showWindow() {
	KeyAPIManage* ui = getInstance();
	if (!ui->isVisible()) {
		ui->loadTable();
		ui->fadeIn();
		ui->show();
		ui->raise();
		ui->activateWindow();
	}
}

void KeyAPIManage::hideWindow() {
	if (m_instance && m_instance->isVisible()) {
		m_instance->fadeOut();
	}
}

void KeyAPIManage::closeWindow() {
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

void KeyAPIManage::closeEvent(QCloseEvent* event) {
	event->ignore();
	hideWindow();
}

void KeyAPIManage::reject() {
	hideWindow();
}

KeyAPIManage::KeyAPIManage(QWidget* parent)
	: QDialog(parent) {
	setWindowTitle("Trình quản lý khóa API Gemini");
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
	setWindowIcon(QIcon(":/icon.png"));
	resize(350, 300);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowOpacity(0.0);

	table = new QTableWidget(this);
	table->setColumnCount(1);
	table->setHorizontalHeaderLabels({ "Danh sách khóa API Gemini" });
	table->horizontalHeader()->setStretchLastSection(true);
	table->verticalHeader()->setVisible(false);
	table->setEditTriggers(QAbstractItemView::AllEditTriggers);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->setShowGrid(false);
	table->horizontalHeader()->setFixedHeight(25);
	table->verticalHeader()->setDefaultSectionSize(20);
	table->horizontalHeader()->setHighlightSections(false);

	createKeyBtn = new QPushButton("Tạo khóa");
	saveBtn = new QPushButton("Lưu và đóng");
	cancelBtn = new QPushButton("Hủy");

	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* bottomLayout = new QHBoxLayout();
	bottomLayout->addWidget(createKeyBtn);
	bottomLayout->addWidget(saveBtn);
	bottomLayout->addWidget(cancelBtn);

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

	connect(table, &QTableWidget::itemChanged, this, &KeyAPIManage::onTableChanged);
	connect(createKeyBtn, &QPushButton::clicked, this, &KeyAPIManage::onCreateKeyButtonClicked);
	connect(saveBtn, &QPushButton::clicked, this, &KeyAPIManage::onsaveButtonClicked);
	connect(cancelBtn, &QPushButton::clicked, this, &KeyAPIManage::onCancelButtonClicked);

	loadTable();
}

void KeyAPIManage::loadTable()
{
	isLoading = true;

	Variable& variable = Variable::getInstance();

	QSet<QString> setKeyAPITemp;
	QSettings settings(variable.appName, "KeyAPIManage");
	QStringList listKeyAPITemp = settings.value("listKeyAPIUser").toStringList();

	listKeyAPIUser.clear();
	for (const auto& keyAPI : listKeyAPITemp) {
		if (!keyAPI.isEmpty() &&
			!setKeyAPITemp.contains(keyAPI) &&
			(keyAPI.length() == 39) &&
			keyAPI.startsWith("AIza"))
		{
			listKeyAPIUser.append(keyAPI);
			setKeyAPITemp.insert(keyAPI);
		}
	}

	table->clearContents();
	table->setRowCount(0);
	for (const auto& keyAPI : listKeyAPIUser) {
		int numRow = table->rowCount();
		table->insertRow(numRow);
		table->setItem(numRow, 0, new QTableWidgetItem(keyAPI));
	}

	int numRow = table->rowCount();
	table->insertRow(numRow);
	table->setItem(numRow, 0, new QTableWidgetItem(""));

	if (listKeyAPIUser.size() > 0) {
		listKeyAPI = listKeyAPIUser;
	}
	else {
		listKeyAPI = listKeyAPIDefault;
	}

	isLoading = false;
}


void KeyAPIManage::onTableChanged(QTableWidgetItem* item) {
	if (isLoading) return;
	isLoading = true;

	int numRow = item->row();

	QString keyAPI = table->item(numRow, 0) ? table->item(numRow, 0)->text().trimmed() : "";

	if (table->item(numRow, 0)->text() != keyAPI) {
		table->item(numRow, 0)->setText(keyAPI);
	}

	if (keyAPI.isEmpty() && numRow != table->rowCount() - 1) {
		table->removeRow(numRow);
		if (numRow < listKeyAPIUser.size()) {
			listKeyAPIUser.removeAt(numRow);
		}
		isLoading = false;
		return;
	}

	if (!keyAPI.isEmpty() && (numRow == table->rowCount() - 1)) {
		table->insertRow(table->rowCount());
		table->setItem(table->rowCount() - 1, 0, new QTableWidgetItem(""));
		listKeyAPIUser.append(keyAPI);
		isLoading = false;
		return;
	}

	if (numRow < listKeyAPIUser.size()) {
		listKeyAPIUser[numRow] = keyAPI;
	}

	isLoading = false;
}

void KeyAPIManage::onsaveButtonClicked()
{
	Variable& variable = Variable::getInstance();

	QSet<QString> setKeyAPITemp;
	QStringList listKeyAPITemp = listKeyAPIUser;

	listKeyAPIUser.clear();
	for (const auto& keyAPI : listKeyAPITemp) {
		if (!keyAPI.isEmpty() &&
			!setKeyAPITemp.contains(keyAPI) &&
			(keyAPI.length() == 39) &&
			keyAPI.startsWith("AIza"))
		{
			listKeyAPIUser.append(keyAPI);
			setKeyAPITemp.insert(keyAPI);
		}
	}

	QSettings settings(variable.appName, "KeyAPIManage");
	settings.setValue("listKeyAPIUser", listKeyAPIUser);

	if (listKeyAPIUser.size() > 0) {
		listKeyAPI = listKeyAPIUser;
	}
	else {
		listKeyAPI = listKeyAPIDefault;
	}

	hideWindow();
}

void KeyAPIManage::onCancelButtonClicked() {
	hideWindow();
}

void KeyAPIManage::onCreateKeyButtonClicked()
{
	QDesktopServices::openUrl(QUrl("https://aistudio.google.com/app/apikey"));
}

KeyAPIManage::~KeyAPIManage() {}

void KeyAPIManage::fadeIn() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(1000);
	anim->setStartValue(0.0);
	anim->setEndValue(0.95);
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void KeyAPIManage::fadeOut() {
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