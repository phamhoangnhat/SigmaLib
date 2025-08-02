#include "ShortcutKeyEditor.h"
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
#include <QPair>
#include <QList>
#include <QSet>


QPointer<ShortcutKeyEditor> ShortcutKeyEditor::m_instance = nullptr;

ShortcutKeyEditor* ShortcutKeyEditor::getInstance() {
    if (!m_instance)
        m_instance = new ShortcutKeyEditor();
    return m_instance;
}

void ShortcutKeyEditor::showWindow() {
    ShortcutKeyEditor* ui = getInstance();
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

void ShortcutKeyEditor::hideWindow() {
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

void ShortcutKeyEditor::closeWindow() {
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

QString ShortcutKeyEditor::getShortcutKey(const QString& key)
{
    QString shortcutKey = dataShortcutKey.value(key, "");
    if (!shortcutKey.isEmpty()) {
        shortcutKey = "Shift → " + shortcutKey;
    }
    return shortcutKey;
}

QString ShortcutKeyEditor::getAction(const int& vkCode)
{
    QString shortcutKey = mapVKCodeToShortcut.value(vkCode, QString());
    if (shortcutKey.isEmpty()) {
        return QString();
    }

    return dataShortcutKeyReverse.value(shortcutKey, QString());
}

ShortcutKeyEditor::ShortcutKeyEditor(QWidget* parent)
    : QDialog(parent) {

    listShortcutKey = {
        "Chuyển đổi bộ mã",
        "Chuyển đổi kiểu gõ",
        "Chuyển đổi dạng chữ viết hoa thường",
        "Gọi bảng cấu hình ứng dụng",
        "Gọi bảng cấu hình chung",
        "Gọi bảng tùy chỉnh kiểu gõ tích hợp",
        "Gọi bảng trình chỉnh sửa tác vụ AI",
        "Gọi bảng trình quản lý phím tắt",
        "Gọi bảng trình quản lý gõ tắt",
        "Gọi bảng xóa các tiết lập đã lưu cho từng ứng dụng",
        "Gọi bảng đóng góp ý kiến",
        "Gọi bảng hướng dẫn sử dụng",
        "Đóng ứng dụng Sigma",
        "Thực hiện tác vụ AI mặc định",
        "Bật | tắt sử dụng chế độ tiếng Việt chủ động",
        "Bật | tắt sử dụng clipboard khi gửi phím",
        "Bật | tắt tương thích với ứng dụng có gợi ý từ",
        "Bật | tắt tự động viết hoa thông minh",
        "Bật | tắt cho phép dùng phụ âm đầu \"f\" \"j\" \"w\" \"z\"",
        "Bật | tắt dùng phím ← → để điều hướng từng từ",
        "Bật | tắt khởi động cùng Windows",
        "Bật | tắt khôi phục từ gốc khi gõ sai chính tả",
        "Bật | tắt tự động chuyển từ tiếng Anh đã ghi nhớ",
        "Bật | tắt xóa toàn bộ dấu khi nhấn phím bỏ dấu",
        "Bật | tắt cho phép bỏ dấu xoay vòng",
        "Bật | tắt cho phép chèn ký tự bị thiếu",
    };

    dataShortcutKeyDefault = {
        {"Chuyển đổi bộ mã", "M"},
        {"Chuyển đổi dạng chữ viết hoa thường", "H"},
        {"Gọi bảng cấu hình ứng dụng", "U"},
        {"Gọi bảng cấu hình chung", "C"},
        {"Thực hiện tác vụ AI mặc định", "`"},
    };

    mapVKCodeToShortcut = {
        {0x30, "0"}, {0x31, "1"}, {0x32, "2"}, {0x33, "3"},
        {0x34, "4"}, {0x35, "5"}, {0x36, "6"}, {0x37, "7"},
        {0x38, "8"}, {0x39, "9"},

        {0x41, "A"}, {0x42, "B"}, {0x43, "C"}, {0x44, "D"},
        {0x45, "E"}, {0x46, "F"}, {0x47, "G"}, {0x48, "H"},
        {0x49, "I"}, {0x4A, "J"}, {0x4B, "K"}, {0x4C, "L"},
        {0x4D, "M"}, {0x4E, "N"}, {0x4F, "O"}, {0x50, "P"},
        {0x51, "Q"}, {0x52, "R"}, {0x53, "S"}, {0x54, "T"},
        {0x55, "U"}, {0x56, "V"}, {0x57, "W"}, {0x58, "X"},
        {0x59, "Y"}, {0x5A, "Z"},

        {0xC0, "`"}, {0xBD, "-"}, {0xBB, "="}, {0xDB, "["},
        {0xDD, "]"}, {0xDC, "\\"}, {0xBA, ";"}, {0xDE, "'"},
        {0xBC, ","}, {0xBE, "."}, {0xBF, "/"},

        {0x60, "0"}, {0x61, "1"}, {0x62, "2"},
        {0x63, "3"}, {0x64, "4"}, {0x65, "5"},
        {0x66, "6"}, {0x67, "7"}, {0x68, "8"},
        {0x69, "9"},
        {0x6A, "*"}, {0x6B, "+"}, {0x6D, "-"},
        {0x6E, "."}, {0x6F, "/"}
    };

    setWindowTitle("Trình quản lý phím tắt");
    setWindowIcon(QIcon(":/icon.png"));
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_DeleteOnClose, false);

    setStyleSheet(R"(
        QDialog {
            background-color: #DCDCDC;
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
            background-color: #DCDCDC;
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

    auto* layout = new QVBoxLayout(this);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(5);
    gridLayout->setContentsMargins(10, 10, 10, 10);

    int total = listShortcutKey.size();
    int columnCount = 2;
    int rowCount = (total + columnCount - 1) / columnCount;

    int index = 0;
    for (const QString& labelText : listShortcutKey) {
        int col = index / rowCount;
        int row = index % rowCount;

        int baseCol = col * 3;

        QLabel* label = new QLabel(labelText);
        gridLayout->addWidget(label, row, baseCol);

        QLineEdit* edit = new QLineEdit();
        edit->setMaxLength(1);
        edit->setFixedWidth(40);
        edit->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(edit, row, baseCol + 1);

        if (col < columnCount - 1) {
            gridLayout->addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Minimum), row, baseCol + 2);
        }

        shortcutEdits.append(edit);
        connect(edit, &QLineEdit::textChanged, this, [=]() { changeShortcutKey(index); });

        ++index;
    }
    layout->addLayout(gridLayout);

    auto* separator = new QFrame(this);
    separator->setObjectName("separatorLine");
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setFixedHeight(1);
    layout->addWidget(separator);
    layout->addSpacing(5);

    QLabel* noteLabel = new QLabel("Ghi chú: Tất cả các phím tắt trên đều thực hiện bằng cách nhấn lần lượt phím Shift rồi phím chính", this);
    noteLabel->setStyleSheet("color: #666666; font-style: italic;");
    layout->addWidget(noteLabel);

    saveBtn = new QPushButton("Lưu");
    cancelBtn = new QPushButton("Đóng");
    defaultBtn = new QPushButton("Mặc định");

    connect(saveBtn, &QPushButton::clicked, this, &ShortcutKeyEditor::saveChanges);
    connect(cancelBtn, &QPushButton::clicked, this, &ShortcutKeyEditor::hideWindow);
    connect(defaultBtn, &QPushButton::clicked, this, &ShortcutKeyEditor::loadDefault);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(defaultBtn);
    btnLayout->addWidget(cancelBtn);
    layout->addLayout(btnLayout);

    loadFromSettings();
}

ShortcutKeyEditor::~ShortcutKeyEditor() {}

void ShortcutKeyEditor::closeEvent(QCloseEvent* event) {
    event->ignore();
    hideWindow();
}

void ShortcutKeyEditor::reject() {
    hideWindow();
}

void ShortcutKeyEditor::changeShortcutKey(int index) {
    if (flagUpdating || index < 0 || index >= shortcutEdits.size())
        return;

    flagUpdating = true;
    QLineEdit* shortcutEdit = shortcutEdits[index];
    QString key = listShortcutKey[index];
    QString shortcutKeyTemp = shortcutEdit->text().toUpper();
    QString shortcutKey = dataShortcutKeyCurrent.value(key, QString());
    bool isValid;
    if (shortcutKeyTemp == shortcutKey) {
        isValid = true;
    }
    else {
        shortcutKey = shortcutKeyTemp;
        isValid = validateShortcut(shortcutKey, dataShortcutKeyCurrent);
    }

    if (!isValid) {
        shortcutKey = dataShortcutKeyCurrent.value(key,QString());
    }

    if (shortcutKey.isEmpty()) {
        dataShortcutKeyCurrent.remove(key);
        shortcutEdit->setText("");
    }
    else {
        dataShortcutKeyCurrent[key] = shortcutKey;
        shortcutEdit->setText(shortcutKey);
    }

    flagUpdating = false;
}

void ShortcutKeyEditor::saveChanges() {
    Variable& variable = Variable::getInstance();
    dataShortcutKeyReverse.clear();
    dataShortcutKey = dataShortcutKeyCurrent;

    QSettings settings(variable.appName, "ShortcutKeys");
    for (int i = 0; i < listShortcutKey.size(); ++i) {
        const QString& key = listShortcutKey[i];
        QString shortcutKey = dataShortcutKey.value(key, QString());
        if (shortcutKey.isEmpty()) {
            settings.remove(key);
        }
        else {
            settings.setValue(key, shortcutKey);
            dataShortcutKeyReverse[shortcutKey] = key;
        }
    }

    hideWindow();
}

void ShortcutKeyEditor::loadFromSettings() {
    Variable& variable = Variable::getInstance();
    flagUpdating = true;
    QSettings settings(variable.appName, "ShortcutKeys");

    dataShortcutKey.clear();
    dataShortcutKeyReverse.clear();
    int i = 0;
    QLineEdit* shortcutEdit;
    for (const QString& key : listShortcutKey) {
        shortcutEdit = shortcutEdits[i];
        QString shortcutKeyDefault = dataShortcutKeyDefault.value(key, QString());
        QString shortcutKey = settings.value(key, shortcutKeyDefault).toString().toUpper();
        bool isValid = validateShortcut(shortcutKey, dataShortcutKey);

        if (!isValid) {
            shortcutKey = QString();
        }

        if (shortcutKey.isEmpty()) {
            dataShortcutKey.remove(key);
            shortcutEdit->setText("");
        }
        else {
            dataShortcutKey[key] = shortcutKey;
            dataShortcutKeyReverse[shortcutKey] = key;
            shortcutEdit->setText(shortcutKey);
        }

        ++i;
    }

    dataShortcutKeyCurrent = dataShortcutKey;
    flagUpdating = false;
}

void ShortcutKeyEditor::loadDefault() {
    flagUpdating = true;

    dataShortcutKeyCurrent.clear();
    for (int i = 0; i < listShortcutKey.size(); ++i) {
        const QString& key = listShortcutKey[i];
        QString shortcutKey = dataShortcutKeyDefault.value(key, QString());

        if (shortcutKey.isEmpty()) {
            dataShortcutKeyCurrent.remove(key);
            shortcutEdits[i]->setText("");
        }
        else {
            dataShortcutKeyCurrent[key] = shortcutKey;
            shortcutEdits[i]->setText(shortcutKey);
        }        
    }

    flagUpdating = false;
}

bool ShortcutKeyEditor::validateShortcut(const QString& shortcutKey, const QMap<QString, QString>& dataShortcutKeyTemp) {
    if (shortcutKey.isEmpty())
        return true;

    if (dataShortcutKeyTemp.values().contains(shortcutKey))
        return false;

    return mapVKCodeToShortcut.values().contains(shortcutKey);
}
