#include "TabRightDelegate.h"

#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QTextOption>
#include <QFontMetrics>

void TabRightDelegate::paint(QPainter* painter,
    const QStyleOptionViewItem& itemOption,
    const QModelIndex& modelIndex) const
{
    QStyleOptionViewItem opt = itemOption;
    initStyleOption(&opt, modelIndex);

    const QWidget* owningWidget = opt.widget;
    QStyle* style = owningWidget ? owningWidget->style()
        : QApplication::style();


    const QString savedText = opt.text;
    opt.text.clear();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, owningWidget);

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, owningWidget);
    const int hpad = style->pixelMetric(QStyle::PM_FocusFrameHMargin, nullptr, owningWidget) + 2;
    textRect.adjust(hpad, 0, -hpad, 0);

    QString leftText = modelIndex.data(Qt::DisplayRole).toString();
    const QString rightText = modelIndex.data(Qt::UserRole).toString();

    QFontMetrics fm(opt.font);
    const int minGap = fm.horizontalAdvance(QStringLiteral("   "));
    const int rightW = fm.horizontalAdvance(rightText);
    const int maxLeft = qMax(0, textRect.width() - minGap - rightW);
    leftText = fm.elidedText(leftText, Qt::ElideRight, maxLeft);

    const QString line = leftText + QChar('\t') + rightText;
    QTextOption to;
    to.setWrapMode(QTextOption::NoWrap);
    to.setTabs({ QTextOption::Tab{ double(textRect.width()), QTextOption::RightTab } });

    painter->save();
    painter->setFont(opt.font);
    const QColor penColor = (opt.state & QStyle::State_Selected)
        ? QColor("#000000")
        : QColor("#888888");
    painter->setPen(penColor);

    painter->translate(textRect.topLeft());
    painter->drawText(QRectF(0, 0, textRect.width(), textRect.height()), line, to);
    painter->restore();
}