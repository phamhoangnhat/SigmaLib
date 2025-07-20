#include "CustomInputMethod.h"
#include "Variable.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>
#include <QSet>
#include <QSettings>
#include <QCloseEvent>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QHBoxLayout>

QPointer<CustomInputMethod> CustomInputMethod::m_instance = nullptr;

CustomInputMethod* CustomInputMethod::getInstance() {
    if (!m_instance)
        m_instance = new CustomInputMethod();
    return m_instance;
}

void CustomInputMethod::showWindow() {
    CustomInputMethod* ui = getInstance();
    if (!ui->isVisible()) {
        ui->loadFromSettings();

        ui->setWindowOpacity(0.0);
        ui->show();
        ui->raise();
        ui->activateWindow();

        QPropertyAnimation* fadeIn = new QPropertyAnimation(ui, "windowOpacity");
        fadeIn->setDuration(1000);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(0.95);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void CustomInputMethod::hideWindow() {
    if (m_instance && m_instance->isVisible()) {
        if (m_instance->popup) {
            m_instance->popup->closeWindow();
            m_instance->popup.clear();
        }
        QPropertyAnimation* anim = new QPropertyAnimation(m_instance, "windowOpacity");
        anim->setDuration(300);
        anim->setStartValue(0.95);
        anim->setEndValue(0.0);
        QObject::connect(anim, &QPropertyAnimation::finished, m_instance, []() {
            m_instance->hide();
            m_instance->setWindowOpacity(1.0);
            });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void CustomInputMethod::closeWindow() {
    if (m_instance) {
        if (m_instance->popup) {
            m_instance->popup->closeWindow();
            m_instance->popup.clear();
        }

        if (m_instance->isVisible()) {
            QPropertyAnimation* anim = new QPropertyAnimation(m_instance, "windowOpacity");
            anim->setDuration(300);
            anim->setStartValue(0.95);
            anim->setEndValue(0.0);
            QObject::connect(anim, &QPropertyAnimation::finished, m_instance, []() {
                m_instance->hide();
                m_instance->setWindowOpacity(1.0);
                });
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
}

CustomInputMethod::CustomInputMethod(QWidget* parent)
    : QDialog(parent) {
    Variable& variable = Variable::getInstance();

    setWindowTitle("Tùy chỉnh kiểu gõ tích hợp");
    setWindowIcon(QIcon(":/icon.png"));
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setStyleSheet(R"(
        QDialog {
            background-color: #DCDCDC; /* Xám nhạt */
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

        QLineEdit {
            background-color: #DCDCDC; /* Nền sáng hơn QDialog */
            color: #000000;
            border: 1px solid #bbbbbb;
            font-weight: bold;
            border-radius: 10px;
            padding-left: 10px;
            padding-right: 10px;
            min-height: 25px;
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
            min-width: 70px;
            min-height: 32px;
            outline: none;
        }

        QPushButton:hover {
            background-color: #c0c0c0;
            border: 1px solid #FFFFFF;
        }

        QPushButton:pressed {
            background-color: #B0B0B0;
        }

        QFrame#separatorLine {
            background-color: #CCCCCC;
            border: none;
        }
    )");

    auto* layout = new QGridLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 16, 16, 16);

    const std::vector<QString> labelListLeft = {
       "Xóa xấu", "Sắc", "Huyền", "Hỏi", "Ngã", "Nặng"
    };
    const std::vector<QString> labelListRight = {
        "a → ă", "a → â", "e → ê", "o → ô", "o → ơ", "u → ư", "d → đ"
    };

    indexListChar = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12, 14 };
    inputFields.resize(indexListChar.size());
    
    for (int i = 0; i < labelListLeft.size(); ++i) {
        int globalIndex = i;

        QLabel* label = new QLabel(labelListLeft[i]);
        label->setFixedWidth(70);

        QLineEdit* input = new QLineEdit();
        input->setMaxLength(5);
        input->setFixedWidth(80);
        input->setAlignment(Qt::AlignCenter);
        input->setPlaceholderText("Gõ phím");

        connect(input, &QLineEdit::textChanged, this, [=](const QString&) {
            changeInput(globalIndex);
            });

        inputFields[globalIndex] = input;

        layout->addWidget(label, i, 0);
        layout->addWidget(input, i, 1);
    }

    for (int i = 0; i < labelListRight.size(); ++i) {
        int globalIndex = static_cast<int>(i + static_cast<int>(labelListLeft.size()));

        QLabel* label = new QLabel(labelListRight[i]);
        label->setFixedWidth(70);

        QLineEdit* input = new QLineEdit();
        input->setMaxLength(5);
        input->setFixedWidth(80);
        input->setAlignment(Qt::AlignCenter);
        input->setPlaceholderText("Gõ phím");

        connect(input, &QLineEdit::textChanged, this, [=](const QString&) {
            changeInput(globalIndex);
            });

        inputFields[globalIndex] = input;

        layout->addWidget(label, i, 2);
        layout->addWidget(input, i, 3);
    }

    auto* separator = new QFrame(this);
    separator->setObjectName("separatorLine");
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setFixedHeight(1);
    layout->addWidget(separator, 7, 0, 1, 4);

    saveBtn = new QPushButton("Lưu");
    cancelBtn = new QPushButton("Đóng");
    defaultBtn = new QPushButton("Mặc định");

    saveBtn->setFixedHeight(32);
    cancelBtn->setFixedHeight(32);
    defaultBtn->setFixedHeight(32);

    saveBtn->setFixedWidth(110);
    cancelBtn->setFixedWidth(110);
    defaultBtn->setFixedWidth(110);

    connect(saveBtn, &QPushButton::clicked, this, &CustomInputMethod::saveChanges);
    connect(cancelBtn, &QPushButton::clicked, this, &CustomInputMethod::hideWindow);
    connect(defaultBtn, &QPushButton::clicked, this, &CustomInputMethod::loadDefault);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(defaultBtn);
    btnLayout->addWidget(cancelBtn);

    layout->addLayout(btnLayout, 8, 0, 1, 4);
}

CustomInputMethod::~CustomInputMethod() {}

void CustomInputMethod::closeEvent(QCloseEvent* event) {
    event->ignore();
    hideWindow();
}

void CustomInputMethod::reject() {
    hideWindow();
}

void CustomInputMethod::changeInput(int index) {
    Variable& variable = Variable::getInstance();
    if (index < 0 || index >= inputFields.size()) {
        return;
    }
    if (flagUpdatingInput) {
        return;
    }

    flagUpdatingInput = true;
    int indexChar = indexListChar[index];

    QLineEdit* input = inputFields[index];
    QString stringRaw = input->text();

    variable.addKeyInputMethod(stringRaw, indexChar, inputMethodTemp);
    QString stringInput = QString::fromStdWString(inputMethodTemp[indexChar]);
    input->setText(stringInput);
    flagUpdatingInput = false;
}

void CustomInputMethod::saveChanges() {
    Variable& variable = Variable::getInstance();
    QStringList result;
    result.resize(15);

    for (int i = 0; i < inputFields.size(); ++i) {
        QString text = inputFields[i]->text();
        result[indexListChar[i]] = text;
    }

    QSettings settings(variable.appName, "InputMethodCustom");
    settings.setValue("data", result);
    
    variable.inputMethodBase = inputMethodTemp;
    variable.update();
    hideWindow();
}

void CustomInputMethod::loadFromSettings() {
    Variable& variable = Variable::getInstance();
	inputMethodTemp = variable.inputMethodBase;

    flagUpdatingInput = true;
    for (int index = 0; index < inputFields.size(); ++index) {
        int indexChar = indexListChar[index];
        QString stringInput = QString::fromStdWString(inputMethodTemp[indexChar]);
        QLineEdit* input = inputFields[index];
        input->setText(stringInput);
    }
    flagUpdatingInput = false;
}

void CustomInputMethod::loadDefault() {
    Variable& variable = Variable::getInstance();
    flagUpdatingInput = true;
    inputMethodTemp = variable.mapInputMethodBase[L"Tích hợp"];

    for (int index = 0; index < inputFields.size(); ++index) {
        int indexChar = indexListChar[index];
        QString stringInput = QString::fromStdWString(inputMethodTemp[indexChar]);
        QLineEdit* input = inputFields[index];
        input->setText(stringInput);
    }
    flagUpdatingInput = false;
}