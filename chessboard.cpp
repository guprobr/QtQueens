#include "chessboard.h"

#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QTimer>
#include <QEventLoop>
#include <cmath>

int SQUARE_SIZE = 100;

ChessBoard::ChessBoard(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)), boardSize(8) { // Default to 8x8
    setScene(scene);
    setFixedSize(900, 900);
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

    emit queenMoved();  // Notify listeners
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
        queen->setPixmap(queenPixmap.scaled(SQUARE_SIZE, SQUARE_SIZE));  // scale to SQUARE_SIZE pretzels
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
        if (queens.size() == boardSize) {
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

void ChessBoard::solvePuzzle() {
    // Clear previous queens if any; by now we show a solution from scratch, not from the current game played :(
    clearQueens();

    // Find a solution
    std::vector<std::pair<int, int>> solution = findSolution();  // this function returns the solution

    // Place all queens with a delay
    for (const auto& pos : solution) {
        int row = pos.first;
        int col = pos.second;

        addQueen(row, col);  // Add queen at the solution's position

        // Create a blocking delay of 250 ms for animation purporses
        QEventLoop loop;
        QTimer::singleShot(250, &loop, &QEventLoop::quit);  // Quit the loop after 250 ms
        loop.exec();  // Start the event loop and block!! here :O
    }
}


std::vector<std::pair<int, int>> ChessBoard::findSolution() {
    std::vector<std::pair<int, int>> solution;
    std::vector<int> board(boardSize, -1);  // -1 means no queen is placed in that column
    
    if (solveBacktrack(board, 0, solution)) {
        return solution;
    }
    
    return solution;  // Empty (if no solution found)
}

bool ChessBoard::solveBacktrack(std::vector<int>& board, int row, std::vector<std::pair<int, int>>& solution) {
    if (row == boardSize) {
        // If we reach the last row, (solution found)
        for (int i = 0; i < boardSize; ++i) {
            solution.push_back({i, board[i]});
        }
        return true;
    }
    
    for (int col = 0; col < boardSize; ++col) {
        if (isSafe(board, row, col)) {
            board[row] = col;  // Place queen at (row, col)
            if (solveBacktrack(board, row + 1, solution)) {
                return true;
            }
            board[row] = -1;  // Backtrackz
        }
    }
    
    return false;  // No safe position for the queen :(
}

bool ChessBoard::isSafe(const std::vector<int>& board, int row, int col) {
    for (int i = 0; i < row; ++i) {
        int c = board[i];
        if (c == col || std::abs(c - col) == std::abs(i - row)) {
            return false;  // Conflict with another queen :(
        }
    }
    return true;
}


