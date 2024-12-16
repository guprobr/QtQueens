#include "chessboard.h"

#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QBrush>

#include <cmath>

int SQUARE_SIZE = 75;

ChessBoard::ChessBoard(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)), boardSize(8) { // Default to 8x8
    setScene(scene);
    setFixedSize(800, 800);
    scene->setSceneRect(0, 0, 800, 800);

    drawBoard();
}


void ChessBoard::setBoardSize(int newSize) {
    clearQueens();       // Clear any existing queens
    boardSize = newSize; // Update internal board size
    scene->clear();      // Clear the scene
    drawBoard();         // Redraw the board
    
}


void ChessBoard::clearQueens() {
    for (Queen *queen : queens) {
        scene->removeItem(queen);
        delete queen;
    }
    queens.clear();
}

void ChessBoard::drawBoard() {
    const int squareSize = 800 / boardSize; // Dynamically adjust square size
    
    SQUARE_SIZE = squareSize;

    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            QGraphicsRectItem *square = scene->addRect(
                col * squareSize, row * squareSize, squareSize, squareSize);

            // Alternate colors
            if ((row + col) % 2 == 0) {
                square->setBrush(QBrush(Qt::gray));
            } else {
                square->setBrush(QBrush(Qt::darkYellow));
            }
        }
    }
}


void ChessBoard::resetGame() {
    queens.clear();
    scene->clear();
    drawBoard();
}


void ChessBoard::addQueen(int row, int col) {
    Queen *queen = new Queen();
    queen->setPosition(row, col);

    scene->addItem(queen);
    queens.append(queen);

    emit queenMoved();  // Notify listeners about the move
}

void ChessBoard::onQueenDropped(int row, int col) {
    // Handle drag-and-drop for queens
    for (Queen *queen : queens) {
        if (queen->row() == row && queen->col() == col) {
            queen->setPosition(row, col);
            emit queenMoved();  // Notify listeners
            return;
        }
    }
}

bool ChessBoard::checkConflicts() {
    bool conflictsFound = false;

    // Reset tinting for all queens (remove any previous tints)
    for (Queen *queen : queens) {
        QPixmap queenPixmap(":/resources/queen.png");
        queen->setPixmap(queenPixmap.scaled(SQUARE_SIZE, SQUARE_SIZE));  // scale to 64 pretzels
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
                q1->setPixmap(tintPixmap(queenPixmap.scaled(SQUARE_SIZE, SQUARE_SIZE), Qt::red));
                q2->setPixmap(tintPixmap(queenPixmap.scaled(SQUARE_SIZE, SQUARE_SIZE), Qt::red));

                conflictsFound = true;
            }
        }
    }

    // Check if the puzzle is solved (i.e., no conflicts and all queens placed)
    if (!conflictsFound) {
        if (queens.size() == boardSize) {  // Assuming we're solving the 8-Queens problem
            qDebug() << "Congratulations! You've solved the puzzle of the " << boardSize << " Queens!";
            // Optionally, show a message box or play a sound here??

        QMessageBox::information(nullptr, "Success", 
                                QString("Congratulations! You've solved the puzzle of the %1 Queens!")
                                .arg(boardSize));
                                
            return true;  // The game has been successfully solved
        }
    }

    return false;  // Conflicts found or incomplete placement
}
