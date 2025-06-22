#include "TaskAIResult.h"
#include "Clipboard.h"
#include "CustomMessageBox.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QPropertyAnimation>


QPointer<TaskAIResult> TaskAIResult::m_instance = nullptr;

TaskAIResult* TaskAIResult::getInstance() {
	if (!m_instance) {
		m_instance = new TaskAIResult();
	}
	return m_instance;
}

void TaskAIResult::showWindow() {
	TaskAIResult* ui = getInstance();

	if (!ui->isVisible()) {
		ui->fadeIn();
	}
	ui->show();
	ui->raise();
	ui->activateWindow();
}

void TaskAIResult::hideWindow() {
	if (m_instance && m_instance->isVisible()) {
		m_instance->fadeOutAndHide();
	}
}

void TaskAIResult::closeWindow() {
	if (m_instance) {
		m_instance->fadeOutAndClose();
	}
}

void TaskAIResult::setResultText(const QString& text) {
	taskAIResultEditor->setPlainText(text);
}

void TaskAIResult::closeEvent(QCloseEvent* event) {
	event->ignore();
	fadeOutAndHide();
}

void TaskAIResult::reject() {
	fadeOutAndHide();
}

TaskAIResult::TaskAIResult(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("Xử lý tác vụ AI");
	setWindowIcon(QIcon(":/icon.png"));
	setWindowFlags(Qt::Window | Qt::Dialog);
	setMinimumSize(400, 300);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowOpacity(0.0);

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor("#E0E0E0"));
	palette.setColor(QPalette::WindowText, Qt::black);
	setPalette(palette);

	taskAIResultEditor = new QTextEdit(this);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(taskAIResultEditor);
	setLayout(mainLayout);

	setStyleSheet(R"(
        QDialog { background-color: #E0E0E0; }

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
}

TaskAIResult::~TaskAIResult() {}

void TaskAIResult::fadeIn() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(1000);
	anim->setStartValue(0.0);
	anim->setEndValue(0.95);
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void TaskAIResult::fadeOutAndHide() {
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

void TaskAIResult::fadeOutAndClose() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(300);
	anim->setStartValue(0.95);
	anim->setEndValue(0.0);
	connect(anim, &QPropertyAnimation::finished, this, [=]() {
		this->hide();
		this->deleteLater();
		m_instance = nullptr;
		});
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}