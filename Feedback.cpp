#include "Feedback.h"
#include "CustomMessageBox.h"
#include <QApplication>
#include <QNetworkRequest>
#include <QUrl>

QPointer<Feedback> Feedback::m_instance = nullptr;

Feedback* Feedback::getInstance() {
	if (!m_instance) {
		m_instance = new Feedback();
	}
	return m_instance;
}

void Feedback::showWindow() {
	Feedback* ui = getInstance();
	if (!ui->isVisible()) {
		ui->loadFeedbackFromSettings();
		ui->fadeIn();
		ui->show();
		ui->raise();
		ui->activateWindow();
	}
}

void Feedback::hideWindow() {
	if (m_instance && m_instance->isVisible()) {
		if (m_instance->popup) {
			m_instance->popup->hide();
		}
		m_instance->fadeOutAndHide();
	}
}

void Feedback::closeWindow() {
	if (m_instance) {
		if (m_instance->popup) {
			m_instance->popup->closeWindow();
			m_instance->popup.clear();
		}
		m_instance->fadeOutAndClose();
	}
}

void Feedback::closeEvent(QCloseEvent* event) {
	if (!flagSending) {
		saveFeedbackToSettings();
		event->ignore();
		fadeOutAndHide();
	}
}

void Feedback::reject() {
	if (!flagSending) {
		saveFeedbackToSettings();
		fadeOutAndHide();
	}
}

Feedback::Feedback(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle("Đóng góp ý kiến");
	setWindowIcon(QIcon(":/icon.png"));
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setMinimumSize(400, 300);
	setAttribute(Qt::WA_DeleteOnClose, false);
	setWindowOpacity(0.0);

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor("#E0E0E0"));
	palette.setColor(QPalette::WindowText, Qt::black);
	setPalette(palette);

	feedbackEditor = new QTextEdit(this);
	sendButton = new QPushButton("Gửi", this);
	cancelButton = new QPushButton("Đóng", this);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(sendButton);
	buttonLayout->addWidget(cancelButton);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(feedbackEditor);
	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);

	feedbackEditor->setPlaceholderText("Chúng tôi rất mong nhận được phản hồi từ bạn ở đây. Bạn cũng có thể để lại thông tin liên hệ để chúng tôi hỗ trợ tốt hơn.");
	connect(sendButton, &QPushButton::clicked, this, &Feedback::onSendClicked);
	connect(cancelButton, &QPushButton::clicked, this, &Feedback::onCancelClicked);

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

Feedback::~Feedback() {}

void Feedback::fadeIn() {
	QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
	anim->setDuration(1000);
	anim->setStartValue(0.0);
	anim->setEndValue(0.95);
	anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void Feedback::fadeOutAndHide() {
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

void Feedback::fadeOutAndClose() {
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

void Feedback::onSendClicked() {
	QString userFeedback = feedbackEditor->toPlainText().trimmed();

	if (userFeedback.isEmpty()) {
		if (!popup) {
			popup = new CustomMessageBox("Thông báo", this);
		}
		popup->setMessage("Vui lòng nhập nội dung phản hồi trước khi gửi.");
		popup->showWindow();
		return;
	}

	saveFeedbackToSettings();
	feedbackEditor->setEnabled(false);
	sendButton->setEnabled(false);
	cancelButton->setEnabled(false);

	flagSending = true;
	sendFeedback(userFeedback);
}

void Feedback::onCancelClicked() {
	saveFeedbackToSettings();
	hideWindow();
}

void Feedback::sendFeedback(const QString& userFeedback) {
	QString baseUrl = "https://script.google.com/macros/s/AKfycbz1n3v780vBayl68zGEGeEc0swT9HgO1Sg5c_bz4xK-tO9oUARczwj59oH4UCPcgify/exec";
	QUrl url(baseUrl);
	QUrlQuery query;
	query.addQueryItem("NameSoftware", "Sigma");
	query.addQueryItem("Feedback", userFeedback);
	url.setQuery(query);

	QNetworkRequest request(url);
	QNetworkReply* reply = networkManager.get(request);

	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			if (!popup) {
				popup = new CustomMessageBox("Thông báo", this);
			}
			popup->setMessage("Phản hồi đã được gửi thành công.");
			popup->showWindow();
			feedbackEditor->clear();
			saveFeedbackToSettings();
		}
		else {
			if (!popup) {
				popup = new CustomMessageBox("Thông báo", this);
			}
			popup->setMessage("Không thể gửi phản hồi. Vui lòng kiểm tra kết nối Internet.");
			popup->showWindow();
		}
		feedbackEditor->setEnabled(true);
		sendButton->setEnabled(true);
		cancelButton->setEnabled(true);
		flagSending = false;
		reply->deleteLater();
		});
}

void Feedback::loadFeedbackFromSettings() {
	QSettings settings("Sigma", "Feedback");
	QString savedFeedback = settings.value("Content", "").toString();
	feedbackEditor->setPlainText(savedFeedback);
}

void Feedback::saveFeedbackToSettings() {
	QString userFeedback = feedbackEditor->toPlainText().trimmed();
	QSettings settings("Sigma", "Feedback");
	settings.setValue("Content", userFeedback);
}
