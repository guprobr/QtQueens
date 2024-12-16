#include "utils.h"
#include <QPainter>

QPixmap tintPixmap(const QPixmap &original, const QColor &color) {
    QPixmap tinted = original;
    QPainter painter(&tinted);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(tinted.rect(), color);
    painter.end();
    return tinted;
}
