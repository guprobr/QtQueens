#include "chessboard.h"
#include "queen.h"

#include <QPainter>
#include <QCursor>

Queen::Queen(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), m_row(0), m_col(0), beingDragged(false) {
    setPixmap(QPixmap(":/resources/queen.png").scaled(SQUARE_SIZE, SQUARE_SIZE));
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
}

void Queen::setPosition(int row, int col) {
    m_row = row;
    m_col = col;
    setPos(col * SQUARE_SIZE, row * SQUARE_SIZE); // Adjust for the square size
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

    // Calculate the board size dynamically based on SQUARE_SIZE
    int boardSize = 800 / SQUARE_SIZE;

    // Round to the nearest square, based on the grid size
    int newRow = std::round(y / static_cast<float>(SQUARE_SIZE));
    int newCol = std::round(x / static_cast<float>(SQUARE_SIZE));

    // Ensure the newRow and newCol are within the bounds of the board (based on boardSize)
    newRow = std::clamp(newRow, 0, boardSize - 1);  // Dynamic board size
    newCol = std::clamp(newCol, 0, boardSize - 1);  // Dynamic board size

    setPosition(newRow, newCol);  // Update the position of the queen

    // Notify ChessBoard to recheck conflicts
    auto board = dynamic_cast<ChessBoard *>(scene()->parent());
    if (board) {
        board->checkConflicts();  // Check for conflicts after the move
    }

    QGraphicsPixmapItem::mouseReleaseEvent(event);
}



