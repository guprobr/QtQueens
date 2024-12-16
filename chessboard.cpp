#include "chessboard.h"

#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QBrush>

#include <cmath>

ChessBoard::ChessBoard(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)) {
    // Cfg scene and view
    setScene(scene);
    setFixedSize(400, 400);
    scene->setSceneRect(0, 0, 400, 400);

    // Draw the board
    drawBoard();
}

void ChessBoard::drawBoard() {
    const int squareSize = 50; // SQUARE SIZE ( 50 pixels)

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // Create a SQUARE
            QGraphicsRectItem *square = scene->addRect(
                col * squareSize, row * squareSize, squareSize, squareSize);

            // Alterna as cores
            if ((row + col) % 2 == 0) {
                square->setBrush(QBrush(Qt::white)); // WHITEs
            } else {
                square->setBrush(QBrush(Qt::darkYellow)); // BLACKs
            }
        }
    }
}

void ChessBoard::addQueen(int row, int col) {
    // Create new Queen
    Queen *queen = new Queen();
    queen->setPosition(row, col);

    // Add to board and vector
    scene->addItem(queen);
    queens.append(queen);
}

bool ChessBoard::checkConflicts() {
    bool conflictsFound = false;

    // Reset tinting for all queens (remove any previous tints)
    for (Queen *queen : queens) {
        QPixmap queenPixmap(":/resources/queen.png");
        queen->setPixmap(queenPixmap.scaled(32, 32));  // scale to 32px
    }

    // Check for conflicts between queens
    for (int i = 0; i < queens.size(); ++i) {
        for (int j = i + 1; j < queens.size(); ++j) {
            Queen *q1 = queens[i];
            Queen *q2 = queens[j];

            // Check if they're in the same row, column, or diagonal
            if (q1->row() == q2->row() || q1->col() == q2->col() ||
                std::abs(q1->row() - q2->row()) == std::abs(q1->col() - q2->col())) {
                // Highlight conflicting queens in red
                QPixmap queenPixmap(":/resources/queen.png");
                q1->setPixmap(tintPixmap(queenPixmap.scaled(32, 32), Qt::red));
                q2->setPixmap(tintPixmap(queenPixmap.scaled(32, 32), Qt::red));

                conflictsFound = true;
            }
        }
    }

    // Check if the puzzle is solved (i.e., no conflicts and all queens placed)
    if (!conflictsFound) {
        if (queens.size() == 8) {  // Assuming we're solving the 8-Queens problem
            qDebug() << "Congratulations! You've solved the puzzle!";
            // Optionally, show a message box or play a sound here??

            QMessageBox::information(nullptr, "Success", "Congratulations! You've solved the puzzle!");

            return true;  // The game has been successfully solved
        }
    }

    return false;  // Conflicts found or incomplete placement
}
