#include "RenameDialog.h"
#include "TaskAIEditor.h"
#include "TaskAIDatabase.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPalette>

RenameDialog::RenameDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle("Đổi tên tác vụ AI");
    setWindowIcon(QIcon(":/icon.png"));
    setAttribute(Qt::WA_DeleteOnClose, false);
    setWindowOpacity(0.0);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor("#E0E0E0"));
    palette.setColor(QPalette::WindowText, Qt::black);
    setPalette(palette);

    setStyleSheet(R"(
        QDialog {
            background-color: #E0E0E0;
            border-radius: 10px;
        }

        QLabel {
            font-weight: bold;
            color: #222;
        }

        QLineEdit {
            background-color: #D0D0D0;
            font-weight: bold;
            border: 1px solid #bbb;
            border-radius: 10px;
            padding: 5px; 
            color: #000;
        }

        QPushButton:hover {
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

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    auto* inputLayout = new QHBoxLayout();
    QLabel* label = new QLabel("Tên mới:");
    label->setFixedWidth(60);

    lineEdit = new QLineEdit(oldName);

    inputLayout->addWidget(label);
    inputLayout->addWidget(lineEdit);

    auto* okBtn = new QPushButton("Đồng ý");
    auto* cancelBtn = new QPushButton("Hủy");

    auto* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(5);
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, &RenameDialog::onAcceptClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &RenameDialog::reject);

    resize(340, 110);
}

void RenameDialog::showWindow() {
    fadeIn();
    show();
    raise();
    activateWindow();
}

void RenameDialog::closeWindow() {
    hideWindow();
}

void RenameDialog::hideWindow() {
    if (popup) {
        popup->closeWindow();
        popup.clear();
    }

    if (this->isVisible()) {
        QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
        anim->setDuration(300);
        anim->setStartValue(this->windowOpacity());
        anim->setEndValue(0.0);
        connect(anim, &QPropertyAnimation::finished, this, [this]() {
            this->hide();
            this->setWindowOpacity(1.0);
            });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

QString RenameDialog::getNewName() const {
    return lineEdit->text().trimmed();
}

void RenameDialog::setOldName(QString name) {
    if (popup) {
        popup->closeWindow();
        popup.clear();
    }
    oldName = name;
    lineEdit->setText(name);
}

void RenameDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    fadeIn();
}

void RenameDialog::reject() {
    hideWindow();
}

void RenameDialog::fadeIn() {
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setDuration(1000);
    anim->setStartValue(0.0);
    anim->setEndValue(0.95);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void RenameDialog::onAcceptClicked() {
    TaskAIEditor* taskAIEditor = TaskAIEditor::getInstance();
    TaskAIDatabase& taskAIDatabase = TaskAIDatabase::getInstance();
    QString newName = getNewName();

    if (newName.isEmpty()) {
        if (!popup) {
            popup = new CustomMessageBox("Lỗi", this);
        }
        popup->setMessage("Tên mới không được để trống!");
        popup->showWindow();
        return;
    }

    if (!taskAIDatabase.isValidTaskName(newName, taskAIEditor->dataTaskAITemp, oldName)) {
        if (!popup) {
            popup = new CustomMessageBox("Lỗi", this);
        }
        popup->setMessage("Tên tác vụ đã tồn tại hoặc không hợp lệ!");
        popup->showWindow();
        return;
    }

    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setDuration(300);
    anim->setStartValue(this->windowOpacity());
    anim->setEndValue(0.0);

    connect(anim, &QPropertyAnimation::finished, this, [this]() {
        this->hide();
        this->setWindowOpacity(1.0);
        this->accept();
        });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
