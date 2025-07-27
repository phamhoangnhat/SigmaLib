#include "NoticeUi.h"
#include "Util.h"
#include "Variable.h"

#include <QPainter>
#include <QScreen>
#include <QGuiApplication>
#include <QString>
#include <QSettings>

#include <windows.h>
#include <iostream>
#include <algorithm>

NoticeUi& NoticeUi::getInstance() {
	static NoticeUi instance;
	return instance;
}

void NoticeUi::showWindow() {
	getInstance().doShow();
}

void NoticeUi::hideWindow() {
	getInstance().doHide();
}

void NoticeUi::closeWindow() {
	getInstance().doClose();
}

void NoticeUi::updateWindow(bool flagReset) {
	Variable& variable = Variable::getInstance();
	NoticeUi& noticeUi = getInstance();

	if (flagReset) {
		noticeUi.backgroundColor = QColor(34, 103, 107);
		noticeUi.displayText = variable.appNameFull;
		noticeUi.currentFontSize = 12;
		noticeUi.currentOpacity = 0;
		noticeUi.targetOpacity = 255;
		noticeUi.iconWidth = 110;
	}

	noticeUi.setFixedSize(noticeUi.iconWidth, 40);
	noticeUi.moveToActiveWindow();
	noticeUi.doShow();
	noticeUi.startFadeEffect();
}

NoticeUi::NoticeUi(QWidget* parent)
	: QWidget(parent) {

	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);
	setFocusPolicy(Qt::NoFocus);
	setFixedSize(iconWidth, 40);

	connect(&fadeTimer, &QTimer::timeout, this, [this]() {
		if (fadeState == FadeState::FadingIn) {
			currentOpacity += 6;
			if (currentOpacity >= targetOpacity) {
				currentOpacity = targetOpacity;
				fadeState = FadeState::FadingOut;
			}
		}
		else if (fadeState == FadeState::FadingOut) {
			currentOpacity -= 1;
			if (currentOpacity <= 0) {
				currentOpacity = 0;
				fadeTimer.stop();
				fadeState = FadeState::Idle;
				doHide();
			}
		}

		setWindowOpacity(currentOpacity / 255.0);
		update();
		});
}

NoticeUi::~NoticeUi() {
	doClose();
}

void NoticeUi::doShow() {
	if (!isOpen) {
		isOpen = true;
		raise();
		show();

		HWND hwnd = (HWND)winId();
		LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
		SetWindowLongW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED);
	}
}

void NoticeUi::doHide() {
	if (!isOpen) return;
	isOpen = false;
	hide();
}

void NoticeUi::doClose() {
	if (!isOpen) return;
	isOpen = false;
	hide();
}

void NoticeUi::moveToActiveWindow() {
	HWND hwndFocused = GetForegroundWindow();
	if (!hwndFocused) {
		moveToCenterScreen();
		return;
	}

	HWND hwndTop = GetAncestor(hwndFocused, GA_ROOT);

	if (!hwndTop) {
		moveToCenterScreen();
		return;
	}

	RECT rect;
	if (!GetWindowRect(hwndTop, &rect)) {
		moveToCenterScreen();
		return;
	}

	HMONITOR hMonitor = MonitorFromWindow(hwndTop, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo = {};
	monitorInfo.cbSize = sizeof(MONITORINFO);
	if (!GetMonitorInfo(hMonitor, &monitorInfo)) {
		moveToCenterScreen();
		return;
	}

	RECT visibleRect;
	visibleRect.left = valueMax(rect.left, monitorInfo.rcMonitor.left);
	visibleRect.top = valueMax(rect.top, monitorInfo.rcMonitor.top);
	visibleRect.right = valueMin(rect.right, monitorInfo.rcMonitor.right);
	visibleRect.bottom = valueMin(rect.bottom, monitorInfo.rcMonitor.bottom);

	if (visibleRect.left >= visibleRect.right || visibleRect.top >= visibleRect.bottom) {
		moveToCenterScreen();
		return;
	}

	QRect monitorRect(
		monitorInfo.rcMonitor.left,
		monitorInfo.rcMonitor.top,
		monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
		monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top
	);

	QScreen* screen = nullptr;
	for (QScreen* scr : QGuiApplication::screens()) {
		if (scr->geometry().intersects(monitorRect)) {
			screen = scr;
			break;
		}
	}

	if (!screen) {
		moveToCenterScreen();
		return;
	}

	qreal scale = screen->devicePixelRatio();

	int vx = visibleRect.left + ((visibleRect.right - visibleRect.left) * 1 / 2);
	int vy = visibleRect.top + ((visibleRect.bottom - visibleRect.top) * 1 / 2);


	QPoint logicalPos(int(vx / scale), int(vy / scale));

	QScreen* confirmedScreen = QGuiApplication::screenAt(logicalPos);
	if (!confirmedScreen) {
		confirmedScreen = screen;
	}

	QRect screenGeom = confirmedScreen->geometry();

	int winW = width();
	int winH = height();

	int x = logicalPos.x() - winW / 2;
	int y = logicalPos.y() - winH / 2;

	x = std::clamp(x, screenGeom.left(), screenGeom.right() - winW);
	y = std::clamp(y, screenGeom.top(), screenGeom.bottom() - winH);

	move(x, y);
}

void NoticeUi::moveToCenterScreen() {
	QScreen* screen = QGuiApplication::primaryScreen();
	if (!screen) return;

	QRect screenGeom = screen->geometry();
	int x = screenGeom.left() + (screenGeom.width() - width()) / 2;
	int y = screenGeom.top() + (screenGeom.height() - height()) / 2;
	move(x, y);
}

void NoticeUi::startFadeEffect() {
	currentOpacity = 0;
	fadeState = FadeState::FadingIn;
	setWindowOpacity(0);
	fadeTimer.start(10);
}

void NoticeUi::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(backgroundColor);
	painter.setPen(Qt::NoPen);
	painter.drawRoundedRect(rect(), 10, 10);

	QFont font("Arial", currentFontSize, QFont::Bold);
	painter.setFont(font);
	painter.setPen(Qt::white);
	painter.drawText(rect(), Qt::AlignCenter, displayText);
}

void NoticeUi::focusOutEvent(QFocusEvent*) {}

void NoticeUi::closeEvent(QCloseEvent* event) {
	doHide();
	event->ignore();
}
