#ifndef UTILS_H
#define UTILS_H

#include <QPixmap>
#include <QColor>

extern int SQUARE_SIZE;

// Function to tint a QPixmap with a specific color
QPixmap tintPixmap(const QPixmap &original, const QColor &color);

#endif // UTILS_H
