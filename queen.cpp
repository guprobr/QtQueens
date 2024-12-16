#include "chessboard.h"
#include "queen.h"

#include <QPainter>
#include <QCursor>

Queen::Queen(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), m_row(0), m_col(0), beingDragged(false) {
    setPixmap(QPixmap(":/resources/queen.png").scaled(32, 32));
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
}

void Queen::setPosition(int row, int col) {
    m_row = row;
    m_col = col;
    setPos(col * 50 + 10, row * 50 + 10); // Adjust for the square size
}

int Queen::row() const {
    return m_row;
}

int Queen::col() const {
    return m_col;
}

void Queen::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    beingDragged = true;
    setCursor(QCursor(Qt::ClosedHandCursor));
    QGraphicsPixmapItem::mousePressEvent(event);
    
}

void Queen::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    beingDragged = false;
    setCursor(QCursor(Qt::ArrowCursor));

    // Snap the queen to the nearest square
    int x = static_cast<int>(pos().x());
    int y = static_cast<int>(pos().y());
    int newRow = y / 50; // Calculate the row based on grid size
    int newCol = x / 50; // Calculate the column based on grid size

    setPosition(newRow, newCol);

    // Notify ChessBoard to recheck conflicts
    auto board = dynamic_cast<ChessBoard *>(scene()->parent());
    if (board) {
        board->checkConflicts();  // Check for conflicts after the move
    }

    QGraphicsPixmapItem::mouseReleaseEvent(event);
}


