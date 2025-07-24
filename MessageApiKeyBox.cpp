#include "MessageApiKeyBox.h"
#include "KeyAPIManage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScreen>
#include <QGuiApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QUrl>

MessageApiKeyBox::MessageApiKeyBox(QWidget* reference)
    : QWidget(nullptr), referenceWidget(reference)
{
    setWindowTitle("Thông báo");
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(360, 180);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    label = new QLabel("Không thể kết nối tới máy chủ. Nguyên nhân có thể do mất mạng, đường truyền yếu hoặc khóa API hiện tại đang bị quá tải. Vui lòng tạo thêm khóa API Gemini khác để đảm bảo tác vụ AI hoạt động ổn định.", this);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);

    closeButton = new QPushButton("Đóng", this);
    addKeyButton = new QPushButton("Thêm khóa", this);

    closeButton->setFixedHeight(32);
    addKeyButton->setFixedHeight(32);

    connect(closeButton, &QPushButton::clicked, this, &MessageApiKeyBox::onCloseButton);
    connect(addKeyButton, &QPushButton::clicked, this, &MessageApiKeyBox::onAddKeyButton);

    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();

    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(10);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(addKeyButton);
    layout->addLayout(buttonLayout);

    setStyleSheet(R"(
        QLabel {
            background: transparent;
            color: #000000;
            font-weight: bold;
            border-radius: 10px;
            padding-left: 0px;
            padding-right: 0px;
            min-height: 25px;
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

    auto* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    effect->setOpacity(0.0);
}

void MessageApiKeyBox::showWindow()
{
    QRect targetRect;
    if (referenceWidget && referenceWidget->isVisible()) {
        targetRect = referenceWidget->geometry();
    }
    else {
        targetRect = QGuiApplication::primaryScreen()->geometry();
    }

    QPoint center = targetRect.center();
    QPoint topLeft = center - QPoint(width() / 2, height() / 2);
    move(topLeft);

    show();
    raise();
    activateWindow();

    QTimer::singleShot(0, this, [this]() { fadeIn(); });
}

void MessageApiKeyBox::closeWindow()
{
    m_forceClose = true;
    fadeOutAndClose();
}

void MessageApiKeyBox::fadeIn()
{
    if (auto* effect = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect())) {
        auto* anim = new QPropertyAnimation(effect, "opacity", this);
        anim->setDuration(1000);
        anim->setStartValue(0.0);
        anim->setEndValue(0.95);
        anim->setEasingCurve(QEasingCurve::OutQuad);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void MessageApiKeyBox::fadeOutAndClose()
{
    if (auto* effect = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect())) {
        auto* anim = new QPropertyAnimation(effect, "opacity", this);
        anim->setDuration(300);
        anim->setStartValue(effect->opacity());
        anim->setEndValue(0.0);
        anim->setEasingCurve(QEasingCurve::OutQuad);

        connect(anim, &QPropertyAnimation::finished, this, [this]() {
            close();
            });

        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else {
        close();
    }
}

void MessageApiKeyBox::fadeOutAndHide()
{
    if (auto* effect = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect())) {
        auto* anim = new QPropertyAnimation(effect, "opacity", this);
        anim->setDuration(300);
        anim->setStartValue(effect->opacity());
        anim->setEndValue(0.0);
        anim->setEasingCurve(QEasingCurve::OutQuad);

        connect(anim, &QPropertyAnimation::finished, this, [this]() {
            hide();
            if (auto* eff = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect()))
                eff->setOpacity(0.95);
            });

        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else {
        hide();
    }
}

void MessageApiKeyBox::onCloseButton()
{
    fadeOutAndHide();
}

void MessageApiKeyBox::onAddKeyButton()
{
    fadeOutAndHide();
    KeyAPIManage::showWindow();
}

void MessageApiKeyBox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor bgColor(240, 240, 240, 255);
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 12, 12);
}

void MessageApiKeyBox::closeEvent(QCloseEvent* event)
{
    if (m_forceClose) {
        event->accept();
        deleteLater();
    }
    else {
        event->ignore();
        fadeOutAndHide();
    }
}

void MessageApiKeyBox::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        fadeOutAndHide();
    }
    else {
        QWidget::keyPressEvent(event);
    }
}
