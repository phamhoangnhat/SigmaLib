#pragma once
#include <QStyledItemDelegate>

class TabRightDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter,
        const QStyleOptionViewItem& itemOption,
        const QModelIndex& modelIndex) const override;

    QSize sizeHint(const QStyleOptionViewItem& option,
        const QModelIndex& index) const override {
        return QStyledItemDelegate::sizeHint(option, index);
    }
};
