#include "CustomConfirmBox.h"
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

CustomConfirmBox::CustomConfirmBox(const QString& title, const QString& message, QWidget* reference)
    : QWidget(nullptr), referenceWidget(reference)
{
    setWindowTitle(title);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(400, 150);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    label = new QLabel(message, this);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);

    yesButton = new QPushButton("Có", this);
    noButton = new QPushButton("Không", this);
    yesButton->setFixedHeight(30);
    noButton->setFixedHeight(30);

    yesButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    noButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(yesButton, &QPushButton::clicked, this, &CustomConfirmBox::onYesButton);
    connect(noButton, &QPushButton::clicked, this, &CustomConfirmBox::onNoButton);

    setStyleSheet(R"(
        QLabel {
            background: transparent;
            color: #222;
            font-weight: bold;
            font-size: 14px;
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

    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(20);
    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);

    layout->addLayout(buttonLayout);

    auto* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    effect->setOpacity(0.0);
}

void CustomConfirmBox::showWindow()
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

    eventLoop.exec();
}

void CustomConfirmBox::closeWindow()
{
    m_forceClose = true;
    fadeOutAndClose();
}

void CustomConfirmBox::fadeIn()
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

void CustomConfirmBox::fadeOutAndClose()
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

void CustomConfirmBox::fadeOutAndHide()
{
    if (auto* effect = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect())) {
        auto* anim = new QPropertyAnimation(effect, "opacity", this);
        anim->setDuration(300);
        anim->setStartValue(effect->opacity());
        anim->setEndValue(0.0);
        anim->setEasingCurve(QEasingCurve::OutQuad);

        connect(anim, &QPropertyAnimation::finished, this, [this]() {
            hide();
            if (auto* eff = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect())) {
                eff->setOpacity(0.95);
            }
            });

        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else {
        hide();
    }
}

void CustomConfirmBox::onYesButton()
{
    confirm = 1;
    fadeOutAndHide();
    eventLoop.quit();
}

void CustomConfirmBox::onNoButton()
{
    confirm = 0;
    fadeOutAndHide();
    eventLoop.quit();
}

void CustomConfirmBox::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor bgColor(240, 240, 240, 255);
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 12, 12);
}

void CustomConfirmBox::closeEvent(QCloseEvent* event)
{
    if (eventLoop.isRunning()) {
        eventLoop.quit();
    }

    if (m_forceClose) {
        event->accept();
        deleteLater();
    }
    else {
        event->ignore();
        fadeOutAndHide();
    }
}

void CustomConfirmBox::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        fadeOutAndHide();
        eventLoop.quit();
    }
    else {
        QWidget::keyPressEvent(event);
    }
}
