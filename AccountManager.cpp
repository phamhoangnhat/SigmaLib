#include "AccountManager.h"
#include "Variable.h"
#include "TaskAIDatabase.h"
#include "KeyAPIManage.h"
#include "Listener.h"
#include "GeneralConfig.h"
#include "SnippetEditor.h"
#include "ShortcutKeyEditor.h"

#include <QSettings>
#include <QFileDialog>
#include <QProcess>

QPointer<AccountManager> AccountManager::m_instance = nullptr;

AccountManager* AccountManager::getInstance() {
	if (!m_instance) {
		m_instance = new AccountManager();
	}
	return m_instance;
}

void AccountManager::showWindow() {
	AccountManager* ui = getInstance();
	if (!ui->isVisible()) {
		ui->fadeIn();
		ui->show();
		ui->raise();
		ui->activateWindow();
		ui->loadWindow();
	}
}

void AccountManager::hideWindow() {
	if (m_instance && m_instance->isVisible()) {
		if (m_instance->renameAccountDialog) {
			m_instance->renameAccountDialog->closeWindow();
			m_instance->renameAccountDialog.clear();
		}
		if (m_instance->popup) {
			m_instance->popup->closeWindow();
			m_instance->popup.clear();
		}
		if (m_instance->confirmRemoveAccount) {
			m_instance->confirmRemoveAccount->closeWindow();
			m_instance->confirmRemoveAccount.clear();
		}
		m_instance->fadeOut();
	}
}

void AccountManager::closeWindow() {
	if (m_instance) {
		if (m_instance->renameAccountDialog) {
			m_instance->renameAccountDialog->closeWindow();
			m_instance->renameAccountDialog.clear();
		}
		if (m_instance->popup) {
			m_instance->popup->closeWindow();
			m_instance->popup.clear();
		}
		if (m_instance->confirmRemoveAccount) {
			m_instance->confirmRemoveAccount->closeWindow();
			m_instance->confirmRemoveAccount.clear();
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

void AccountManager::closeEvent(QCloseEvent* event) {
	event->ignore();
	hideWindow();
}

void AccountManager::reject() {
	hideWindow();
}

AccountManager::AccountManager(QWidget* parent)
	: QDialog(parent)
{
	getDataAccounts();
	setWindowTitle("Trình quản lý người dùng");
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
	setWindowIcon(QIcon(":/icon.png"));
	setFixedWidth(420);

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

        QFrame {
            background-color: #CCCCCC;
        }
    )");

	accountCombo = new QComboBox();
	renameAccountBtn = new QPushButton("");
	copyAccountBtn = new QPushButton("");
	removeAccountBtn = new QPushButton("");
	newAccountBtn = new QPushButton("Thêm người dùng");
	exportAccountBtn = new QPushButton("Xuất cấu hình");
	closeAccountBtn = new QPushButton("Đóng");

	renameAccountBtn->setIcon(QIcon(":/iconRenameItem.png"));
	copyAccountBtn->setIcon(QIcon(":/iconAddItem.png"));
	removeAccountBtn->setIcon(QIcon(":/iconDeleteItem.png"));

	auto* separator = new QFrame(this);
	separator->setFrameShape(QFrame::NoFrame);
	separator->setFrameShadow(QFrame::Plain);
	separator->setFixedHeight(1);
	
	separator->setStyleSheet("background-color: #CCCCCC;");

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->addWidget(accountCombo, 1);
	topLayout->addWidget(renameAccountBtn);
	topLayout->addWidget(copyAccountBtn);
	topLayout->addWidget(removeAccountBtn);

	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->addWidget(newAccountBtn);
	btnLayout->addWidget(exportAccountBtn);
	btnLayout->addWidget(closeAccountBtn);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(topLayout);
	layout->addSpacing(5);
	layout->addWidget(separator);
	layout->addSpacing(5);
	layout->addLayout(btnLayout);

	connect(accountCombo, &QComboBox::currentTextChanged, this, &AccountManager::onAccountComboChanged);
	connect(newAccountBtn, &QPushButton::clicked, this, &AccountManager::onNewAccountClicked);
	connect(copyAccountBtn, &QPushButton::clicked, this, &AccountManager::onCopyAccountClicked);
	connect(renameAccountBtn, &QPushButton::clicked, this, &AccountManager::onRenameAccountClicked);
	connect(removeAccountBtn, &QPushButton::clicked, this, &AccountManager::onRemoveAccountClicked);
	connect(exportAccountBtn, &QPushButton::clicked, this, [this]() {exportCurrentAccountToReg(this);});
	connect(closeAccountBtn, &QPushButton::clicked, this, &AccountManager::onCloseButtonClicked);
}

AccountManager::~AccountManager() {}

void AccountManager::loadWindow() {
	isInternalChange = true;

	Variable& variable = Variable::getInstance();
	ShortcutKeyEditor* shortcutKeyEditor = ShortcutKeyEditor::getInstance();
	SnippetEditor* snippetEditor = SnippetEditor::getInstance();
	TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
	
	QList<QString> listAccounts = dataAccounts.values();
	accountCombo->clear();
	for (const QString& account : listAccounts) {
		accountCombo->addItem(account);
	}
	int index = accountCombo->findText(currentAccount);
	accountCombo->setCurrentIndex(index);

	snippetEditor->loadSnippetFromRegistry();
	shortcutKeyEditor->loadFromSettings();
	taskAIDatabase.loadDataTaskAI();
	variable.init();

	isInternalChange = false;
}

void AccountManager::fadeIn() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(1000);
	anim->setStartValue(0.0);
	anim->setEndValue(0.95);
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void AccountManager::fadeOut() {
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


void AccountManager::onNewAccountClicked()
{
	QString newAccount = addAccounts();
	setCurrentAccount(newAccount);
	loadWindow();
}

void AccountManager::onCopyAccountClicked() {
	QString newAccount = addAccounts();
	copyAccounts(currentAccount, newAccount, false);
	setCurrentAccount(newAccount);
	loadWindow();
}

void AccountManager::onRenameAccountClicked() {
	QString oldAccount = currentAccount;
	if (oldAccount.isEmpty()) return;

	if (!renameAccountDialog) {
		renameAccountDialog = new RenameAccountDialog(this);

		connect(renameAccountDialog, &QDialog::accepted, this, [this]() {
			QString oldAccount = renameAccountDialog->oldName;
			QString newAccount = renameAccountDialog->getNewName();
			copyAccounts(oldAccount, newAccount, true);
			setCurrentAccount(newAccount);
			loadWindow();
		});
	}
	renameAccountDialog->setOldName(oldAccount);
	renameAccountDialog->showWindow();
}

void AccountManager::onRemoveAccountClicked() {
	QString oldAccount = currentAccount;

	if (dataAccounts.size() <= 1) {
		if (!popup) {
			popup = new CustomMessageBox("Lỗi", this);
		}
		popup->setMessage("Phải có ít nhất một tài khoản người dùng!");
		popup->showWindow();
		return;
	}

	if (!confirmRemoveAccount) {
		confirmRemoveAccount = new CustomConfirmBox("Xác nhận", "Bạn có chắc muốn xóa tài khoản người dùng?", nullptr);
	}
	confirmRemoveAccount->showWindow();
	if (confirmRemoveAccount) {
		removeAccounts(oldAccount);
		loadWindow();
	}
}

void AccountManager::onAccountComboChanged(const QString& account) {
	if (isInternalChange) return;

	setCurrentAccount(account);
	loadWindow();
}

void AccountManager::onCloseButtonClicked()
{
	hideWindow();
}

void AccountManager::getDataAccounts()
{
	dataAccounts.clear();

	QSettings settingsAcount(APP_NAME, "AccountManager");
	QStringList listGroupSub = settingsAcount.childGroups();

	for (const QString& nameGroupSub : listGroupSub) {
		QString account = nameGroupSub.trimmed();
		QString upperAccount = account.toUpper();
		if (!account.isEmpty() && !dataAccounts.contains(upperAccount)) {
			dataAccounts[account.toUpper()] = account;
		}
		else {
			settingsAcount.remove(nameGroupSub);
		}
	}

	QString newAccount;
	newAccount = addAccounts();
	bool isAccount = copyAccounts("", newAccount, true);
	if (!isAccount) {
		removeAccounts(newAccount);
	}

	if (dataAccounts.isEmpty()) {
		newAccount = addAccounts();
	}

	QSettings settingsConfig(APP_NAME, "Config");
	QString account = settingsConfig.value("currentAccount", "").toString().trimmed();
	if(account.isEmpty() || !dataAccounts.contains(account.toUpper())) {
		account = dataAccounts.first();
	}
	setCurrentAccount(account);
}

bool AccountManager::copyAccounts(const QString& accountSrc,
	const QString& accountDes,
	bool isMove)
{
	bool result = false;

	QStringList listKeyConfig{
		"inputMethod", "modeRestore", "modeAutoChangeLang", "modeRemoveDiacTone", "modeLoopDiacTone", "modeInsertChar"
	};

	if (!accountSrc.isEmpty()) {
		listKeyConfig.append("modeAutoUpdate");
	}

	result |= copyKeys(accountSrc, accountDes, "Config",	listKeyConfig, false,	true, isMove);
	result |= copyKeys(accountSrc, accountDes, "ConfigUi",				{}, true,	true, isMove);
	result |= copyKeys(accountSrc, accountDes, "DataAutoChangeLang",	{}, false,	true, isMove);
	result |= copyKeys(accountSrc, accountDes, "InputMethodCustom",		{}, false,	true, isMove);
	result |= copyKeys(accountSrc, accountDes, "ShortcutKeys",			{}, false,	true, isMove);
	result |= copyKeys(accountSrc, accountDes, "Snippet",				{}, true,	true, isMove);
	result |= copyKeys(accountSrc, accountDes, "TaskAIDatabase",		{}, false,	true, isMove);

	if (result) {
		dataAccounts[accountDes.toUpper()] = accountDes;
	}

	if (isMove) {
		if (accountSrc.isEmpty()) {
			const QStringList listKeyOld = {
				"ConfigUi",
				"DataAutoChangeLang",
				"InputMethodCustom",
				"ShortcutKeys",
				"Snippet",
				"TaskAIDatabase"
			};

			auto deleteRegTree = [&](const QString& keyOld) {
				const QString fullPath = QStringLiteral("HKCU\\Software\\%1\\%2")
					.arg(APP_NAME, keyOld);
				runTaskHidden("reg.exe", { "delete", fullPath, "/f", "/reg:64" });
				runTaskHidden("reg.exe", { "delete", fullPath, "/f", "/reg:32" });
				};

			for (const QString& keyOld : listKeyOld) {
				deleteRegTree(keyOld);
			}
		}
		else {
			removeAccounts(accountSrc);
		}
	}
	return result;
}

bool AccountManager::copyKeys(const QString& accountSrc, const QString& accountDes,
	const QString& groupParent, const QStringList& listKey,
	bool hasGroupSub, bool clearDestFirst, bool isMove)
{
	bool result = false;
	QString groupSrcTemp;
	QString groupDesTemp;

	groupSrcTemp = accountSrc.isEmpty() ? groupParent : "AccountManager";
	QSettings settingSrc(APP_NAME, groupSrcTemp);

	groupDesTemp = accountDes.isEmpty() ? groupParent : "AccountManager";
	QSettings settingDes(APP_NAME, groupDesTemp);


	groupSrcTemp = accountSrc.isEmpty() ? QString() : accountSrc + "/" + groupParent;
	if (!groupSrcTemp.isEmpty()) {
		settingSrc.beginGroup(groupSrcTemp);
	}
	QStringList listGroupSub = hasGroupSub ? settingSrc.childGroups() : QStringList{ QString() };
	if (!groupSrcTemp.isEmpty()) {
		settingSrc.endGroup();
	}

	for (const QString& groupSub : listGroupSub) {
		groupSrcTemp = accountSrc.isEmpty() ? QString() : accountSrc + "/" + groupParent;
		groupDesTemp = accountDes.isEmpty() ? QString() : accountDes + "/" + groupParent;
		if (hasGroupSub) {
			groupSrcTemp = groupSrcTemp.isEmpty() ? groupSub : groupSrcTemp + "/" + groupSub;
			groupDesTemp = groupDesTemp.isEmpty() ? groupSub : groupDesTemp + "/" + groupSub;
		}

		if (!groupSrcTemp.isEmpty()) {
			settingSrc.beginGroup(groupSrcTemp);
		}
		if (!groupDesTemp.isEmpty()) {
			settingDes.beginGroup(groupDesTemp);
		}

		if (clearDestFirst) {
			settingDes.remove(QString());
		}

		const QStringList keysLocal = listKey.isEmpty()
			? settingSrc.childKeys()
			: listKey;

		for (const QString& key : keysLocal) {
			if (settingSrc.contains(key)) {
				const QVariant value = settingSrc.value(key);
				settingDes.setValue(key, value);
				result = true;
				if (isMove) {
					settingSrc.remove(key);
				}
			}
		}
		if (!groupSrcTemp.isEmpty()) {
			settingSrc.endGroup();
		}
		if (!groupDesTemp.isEmpty()) {
			settingDes.endGroup();
		}
	}

	settingDes.sync();
	settingSrc.sync();
	return result;
}

void AccountManager::setCurrentAccount(const QString& account)
{
	if (dataAccounts.contains(account.toUpper())) {
		currentAccount = account;
		QSettings settingsConfig(APP_NAME, "Config");
		settingsConfig.setValue("currentAccount", currentAccount);
		settingsConfig.sync();
	}
}

QString AccountManager::addAccounts()
{
	QString baseName = "Tài khoản";
	int index = 1;
	QString newAccount;
	QString upperAccount;

	do {
		newAccount = QString("%1 %2").arg(baseName).arg(index, 2, 10, QChar('0'));
		upperAccount = newAccount.toUpper();
		++index;
	} while (dataAccounts.contains(upperAccount));

	dataAccounts[upperAccount] = newAccount;

	QSettings settings(APP_NAME, "AccountManager");
	settings.beginGroup(newAccount);
	settings.setValue(QStringLiteral("created"), true);
	settings.endGroup();
	settings.sync();
	return newAccount;
}

bool AccountManager::removeAccounts(const QString& account)
{
	dataAccounts.remove(account.toUpper());
	QSettings settings(APP_NAME, "AccountManager");
	settings.remove(account);
	settings.sync();
	if (currentAccount == account) {
		setCurrentAccount(dataAccounts.values().first());
	}
	return true;
}

bool AccountManager::exportCurrentAccountToReg(QWidget* parent)
{
	const QString suggested = QString("%1.reg").arg(currentAccount);

	QString outPath = QFileDialog::getSaveFileName(
		parent,
		tr("Xuất cấu hình tài khoản"),
		QDir::homePath() + "/" + suggested,
		tr("Registry File (*.reg)")
	);
	if (outPath.isEmpty()) return false;
	if (!outPath.endsWith(".reg", Qt::CaseInsensitive)) outPath += ".reg";
	QDir().mkpath(QFileInfo(outPath).absolutePath());

	const QString regKey = QStringLiteral("HKCU\\Software\\%1\\AccountManager\\%2").arg(APP_NAME, currentAccount);

	QProcess process;
	process.start("reg.exe", QStringList() << "export" << regKey << outPath << "/y");
	if (!process.waitForStarted(5000)) return false;
	process.waitForFinished(-1);

	return (process.error() == QProcess::UnknownError && process.exitCode() == 0);
}
