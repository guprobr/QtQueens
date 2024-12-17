#include "chessboard.h"

#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QTimer>
#include <QPushButton>
#include <QEventLoop>
#include <QRandomGenerator>
#include <cmath>

int SQUARE_SIZE = 100;

ChessBoard::ChessBoard(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)), boardSize(8), elapsedSeconds(0) {
    setScene(scene);
    setFixedSize(740, 740);
    scene->setSceneRect(0, 0, 580, 580);

    // Add a "Hint" button
    QPushButton *hintButton = new QPushButton("Hint", this);
    hintButton->setGeometry(370, 700, 100, 40);
    connect(hintButton, &QPushButton::clicked, this, &ChessBoard::showHint);

    // Init the chronometer
    chronometerLabel = new QLabel(this);
    chronometerLabel->setText("Elapsed Time: 00:00");
    chronometerLabel->setAlignment(Qt::AlignCenter);
    chronometerLabel->setStyleSheet("font: bold 21px; color: black; background-color: gray;");
    chronometerLabel->setGeometry(0, 0, 740, 25); // Position at the top of the UI
    chronometerTimer = new QTimer(this);
    connect(chronometerTimer, &QTimer::timeout, this, &ChessBoard::updateChronometer);

    // Init chessboard
    setBoardSize(8); // default 8x8

}

void ChessBoard::setBoardSize(int newSize) {

    // THIS RESETs THE GAME
    clearQueens();       // Clear any existing queens
    boardSize = newSize; // Update internal board size
    scene->clear();      // Clear the scene
    boardNeedsRedraw = true;
    drawBoard();         // Redraw the board

    // START New game, so Reset the chronometer
    resetChronometer();
    chronometerTimer->start(1000);

    // enable the UI
    setInteractive(true);
    for (QWidget *child : findChildren<QWidget *>()) {
        child->setEnabled(true);
    }
}

void ChessBoard::resetChronometer() {
    elapsedSeconds = 0;
    chronometerTimer->stop();
    chronometerLabel->setText("Elapsed Time: 00:00");
}

void ChessBoard::updateChronometer() {
    elapsedSeconds++;
    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;
    chronometerLabel->setText(QString("Elapsed Time: %1:%2")
                              .arg(minutes, 2, 10, QChar('0'))
                              .arg(seconds, 2, 10, QChar('0')));
}

void ChessBoard::clearQueens() {
    for (Queen *queen : queens) {
        scene->removeItem(queen);
        delete queen;
    }
    queens.clear(); // kill the objects also
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
    queen->setZValue(2);  // Ensure queens stay above highlights and board
    queens.append(queen);

    emit queenMoved();  // Notify listeners
}

void ChessBoard::removeQueen(int row, int col) {
    for (int i = 0; i < queens.size(); ++i) {
        if (queens[i]->row() == row && queens[i]->col() == col) {
            scene->removeItem(queens[i]);
            delete queens[i];
            queens.removeAt(i);
            return;
        }
    }
}

void ChessBoard::onQueenDropped(int row, int col) {
    // Handle drag-and-drop for queens
    for (Queen *queen : queens) {
        if (queen->row() == row && queen->col() == col) {
            if ( !isQueenAt(row, col) ) {
                queen->setPosition(row, col);
                emit queenMoved();  // Notify listeners
            }
            return;
        }
    }
}

bool ChessBoard::checkConflicts() {
    bool conflictsFound = false;

    // Reset tinting for all queens
    for (Queen *queen : queens) {
        QPixmap queenPixmap(":/resources/queen.png");
        queen->setPixmap(queenPixmap.scaled(SQUARE_SIZE, SQUARE_SIZE));
    }

    // Check for conflicts between queens
    for (int i = 0; i < queens.size(); ++i) {
        for (int j = i + 1; j < queens.size(); ++j) {
            Queen *q1 = queens[i];
            Queen *q2 = queens[j];

            if (q1->row() == q2->row() || q1->col() == q2->col() ||
                std::abs(q1->row() - q2->row()) == std::abs(q1->col() - q2->col())) {
                QPixmap queenPixmap(":/resources/queen.png");
                q1->setPixmap(tintPixmap(queenPixmap.scaled(SQUARE_SIZE, SQUARE_SIZE), Qt::red));
                q2->setPixmap(tintPixmap(queenPixmap.scaled(SQUARE_SIZE, SQUARE_SIZE), Qt::red));

                conflictsFound = true;
            }
        }
    }

    // Check if the puzzle is solved
    if (!conflictsFound) {
        if (queens.size() == boardSize) {
            chronometerTimer->stop(); // Stop the chronometer timer
            QMessageBox::information(nullptr, "Success",
                                     QString("Congratulations! You've solved the puzzle in %1:%2!")
                                     .arg(elapsedSeconds / 60, 2, 10, QChar('0'))
                                     .arg(elapsedSeconds % 60, 2, 10, QChar('0')));

            // Disable the UI
            setInteractive(false);
            for (QWidget *child : findChildren<QWidget *>()) {
                child->setEnabled(false);
            }
            return true;
        }
    }

    return false;
}

void ChessBoard::showHint() {
    drawBoard();
    checkConflicts();

    QList<ChessBoard::Hint> hints;

    for (Queen *queen : queens) {
        if (isBlocking(queen)) {
            QPair<int, int> safeMove = findSafeMoveForQueen(queen);
            if (safeMove.first != -1 && safeMove.second != -1) {
                hints.append({queen->row(), queen->col(), safeMove.first, safeMove.second, Qt::green, "Move this queen to a safe square."});
                break; // Found a hint, no need to check other blocking queens for immediate moves.
            }
        }
    }

    if (hints.isEmpty()) {
        if (  suggestLeastConflictMove(hints) || suggestFutureSafeMove(hints) || suggestConflictBreaker(hints) || suggestRandomMove(hints)) {
            // A hint was found by one of the fallback methods
        }
    }

    if (hints.isEmpty() && !checkConflicts()) {
        QMessageBox::information(this, "No Conflicts", "All queens are SAFE!");
        return;
    }

    if (hints.isEmpty()) {
        QMessageBox::information(this, "No Hint available", "No immediate or future safe moves found.");
        return;
    }

    if (hints.isEmpty()) {
        return;
    }

    for (const auto& hint : hints) {
        highlightSquare(hint.fromRow, hint.fromCol, Qt::red);
        highlightSquare(hint.toRow, hint.toCol, hint.color);

        QMessageBox msgBox;
        msgBox.setText(hint.description);
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Ok) {
            QMetaObject::invokeMethod(this, "performHintMove", Qt::QueuedConnection,
                                      Q_ARG(int, hint.fromRow), Q_ARG(int, hint.fromCol),
                                      Q_ARG(int, hint.toRow), Q_ARG(int, hint.toCol));
        } else {
            drawBoard(); // Clear highlights if cancelled
        }
        return; // Show only one hint
    }
}

void ChessBoard::performHintMove(int fromRow, int fromCol, int toRow, int toCol) {
    for (Queen *queen : queens) {
        if (queen->row() == fromRow && queen->col() == fromCol) {
            queen->setPosition(toRow, toCol); // execute Hint suggestion
            drawBoard();
            emit queenMoved();
            return;
        }
    }
}

void ChessBoard::drawBoard() {
    const int squareSize = 580 / boardSize;
    SQUARE_SIZE = squareSize;

    // Remove only highlights
    QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem* item : items) {
        if (item->zValue() == 1) {
            scene->removeItem(item);
            delete item;
        }
    }

    if (boardNeedsRedraw) {
        scene->clear();
        for (int row = 0; row < boardSize; ++row) {
            for (int col = 0; col < boardSize; ++col) {
                QGraphicsRectItem* square = scene->addRect(
                    col * squareSize, row * squareSize, squareSize, squareSize);

                if ((row + col) % 2 == 0) {
                    square->setBrush(QBrush(Qt::gray));
                } else {
                    square->setBrush(QBrush(Qt::darkGray));
                }
            }
        }
        boardNeedsRedraw = false;
    }

    for (Queen* queen : queens) {
        queen->setPixmap(QPixmap(":/resources/queen.png").scaled(SQUARE_SIZE, SQUARE_SIZE));
        queen->setZValue(2);
        queen->setPosition(queen->row(), queen->col()); // Very important: position the queen
    }
}


void ChessBoard::highlightSquare(int row, int col, QColor color) {
    int squareSize = SQUARE_SIZE;

    QList<QGraphicsItem *> items = scene->items(QRectF(col * squareSize, row * squareSize, squareSize, squareSize));
    for (QGraphicsItem *item : items) {
        if (item->zValue() == 1 && scene->items().contains(item)) {
            scene->removeItem(item);
            delete item;
        }
    }

    QGraphicsRectItem *highlight = scene->addRect(
        col * squareSize, row * squareSize, squareSize, squareSize,
        QPen(Qt::NoPen), QBrush(color.lighter(150)));
    highlight->setZValue(1);
}

bool ChessBoard::suggestLeastConflictMove(QList<ChessBoard::Hint>& hints) {
    int minConflicts = boardSize * boardSize;  // Initialize with a large value
    QPair<int, int> bestMove = {-1, -1};
    Queen* queenToMove = nullptr;

   // Shuffle the queens list before iterating
    QList<Queen*> shuffledQueens = queens;
    std::random_device rd;   // Seed for randomness
    std::mt19937 g(rd());    // Mersenne Twister PRNG
    std::shuffle(shuffledQueens.begin(), shuffledQueens.end(), g);

    for (Queen* queen : shuffledQueens) {
        int originalRow = queen->row();
        int originalCol = queen->col();

        for (int targetRow = 0; targetRow < boardSize; ++targetRow) {
            for (int targetCol = 0; targetCol < boardSize; ++targetCol) {
                if (originalRow == targetRow && originalCol == targetCol) continue;

                if ( !isQueenAt(targetRow, targetCol) ) {
                    queen->setPosition(targetRow, targetCol);
                } else continue;

                int conflicts = calculateConflicts();
                if (conflicts < minConflicts) {
                    minConflicts = conflicts;
                    bestMove = {targetRow, targetCol};
                    queenToMove = queen;
                }
                queen->setPosition(originalRow, originalCol);
            }
        }
    }

    if (queenToMove && bestMove.first != -1) {
        hints.append({queenToMove->row(), queenToMove->col(), bestMove.first, bestMove.second, Qt::yellow, "Move to reduce conflicts."});
        return true;
    }
    return false;
}

int ChessBoard::calculateConflicts() {
    int conflictCount = 0;
    for (int i = 0; i < queens.size(); ++i) {
        for (int j = i + 1; j < queens.size(); ++j) {
            Queen* q1 = queens[i];
            Queen* q2 = queens[j];
            if (q1->row() == q2->row() || q1->col() == q2->col() ||
                std::abs(q1->row() - q2->row()) == std::abs(q1->col() - q2->col())) {
                ++conflictCount;
            }
        }
    }
    return conflictCount;
}

int ChessBoard::calculateConflictsAt(int row, int col) {
    int conflictCount = 0;

    for (Queen* queen : queens) {
        // Skip the position itself (optional check for queens already there)
        if (queen->row() == row && queen->col() == col) continue;

        // Check for conflicts:
        if (queen->row() == row || queen->col() == col || 
            std::abs(queen->row() - row) == std::abs(queen->col() - col)) {
            ++conflictCount;
        }
    }

    return conflictCount;
}


bool ChessBoard::suggestConflictBreaker(QList<ChessBoard::Hint>& hints) {
    Queen* mostConflictingQueen = findQueenWithMostConflicts();
    if (!mostConflictingQueen) return false;

    int originalRow = mostConflictingQueen->row();
    int originalCol = mostConflictingQueen->col();

    for (int targetRow = 0; targetRow < boardSize; ++targetRow) {
        for (int targetCol = 0; targetCol < boardSize; ++targetCol) {
            if (originalRow == targetRow && originalCol == targetCol) continue;

            if ( !isQueenAt(targetRow, targetCol) ) {
                mostConflictingQueen->setPosition(targetRow, targetCol);
            } else continue;
            
            if (calculateConflicts() < calculateConflictsAt(originalRow, originalCol)) {
                hints.append({originalRow, originalCol, targetRow, targetCol, Qt::magenta, "Move to reduce major conflicts."});
                mostConflictingQueen->setPosition(originalRow, originalCol);
                return true;
            }
        }
    }
    mostConflictingQueen->setPosition(originalRow, originalCol);
    return false;
}

Queen* ChessBoard::findQueenWithMostConflicts() {
    Queen* worstQueen = nullptr;
    int maxConflicts = 0;

    // Shuffle the queens list before iterating
    QList<Queen*> shuffledQueens = queens;
    std::random_device rd;   // Seed for randomness
    std::mt19937 g(rd());    // Mersenne Twister PRNG
    std::shuffle(shuffledQueens.begin(), shuffledQueens.end(), g);

    for (Queen* queen : shuffledQueens) {
        int conflicts = calculateConflictsForQueen(queen);
        if (conflicts > maxConflicts) {
            maxConflicts = conflicts;
            worstQueen = queen;
        }
    }
    return worstQueen;
}

int ChessBoard::calculateConflictsForQueen(Queen* targetQueen) {
    int conflicts = 0;
    for (Queen* otherQueen : queens) {
        if (targetQueen == otherQueen) continue;

        if (targetQueen->row() == otherQueen->row() || 
            targetQueen->col() == otherQueen->col() || 
            std::abs(targetQueen->row() - otherQueen->row()) == std::abs(targetQueen->col() - otherQueen->col())) {
            ++conflicts;
        }
    }
    return conflicts;
}

bool ChessBoard::suggestRandomMove(QList<ChessBoard::Hint>& hints) {
    // Shuffle the queens list before iterating
    QList<Queen*> shuffledQueens = queens;
    std::random_device rd;   // Seed for randomness
    std::mt19937 g(rd());    // Mersenne Twister PRNG
    std::shuffle(shuffledQueens.begin(), shuffledQueens.end(), g);

    for (Queen* queen : shuffledQueens) {
        int targetRow = QRandomGenerator::global()->bounded(boardSize);
        int targetCol = QRandomGenerator::global()->bounded(boardSize);

        if (targetRow != queen->row() || targetCol != queen->col()) {
            hints.append({queen->row(), queen->col(), targetRow, targetCol, Qt::blue, "Try this random move."});
            return true;
        }
    }
    return false;
}



bool ChessBoard::suggestFutureSafeMove(QList<ChessBoard::Hint>& hints) {
    // Shuffle the queens list before iterating
    QList<Queen*> shuffledQueens = queens;
    std::random_device rd;   // Seed for randomness
    std::mt19937 g(rd());    // Mersenne Twister PRNG
    std::shuffle(shuffledQueens.begin(), shuffledQueens.end(), g);

    for (Queen* queenToMove : shuffledQueens) {
        if (!isBlocking(queenToMove)) continue;

        int originalRow = queenToMove->row();
        int originalCol = queenToMove->col();

        for (int targetRow = 0; targetRow < boardSize; ++targetRow) {
            for (int targetCol = 0; targetCol < boardSize; ++targetCol) {
                if (originalRow == targetRow && originalCol == targetCol) continue;

                if ( !isQueenAt(targetRow, targetCol) ) {
                    queenToMove->setPosition(targetRow, targetCol);
                    emit queenMoved();
                    checkConflicts();
                } else continue;

                for (Queen* otherQueen : queens) {
                    if (otherQueen == queenToMove) continue;

                    QPair<int, int> safeMoveForOther = findSafeMoveForQueen(otherQueen);
                    if (safeMoveForOther.first != -1) { // A safe move exists for the other queen
                        hints.append({originalRow, originalCol, targetRow, targetCol, Qt::cyan, QString("Moving this queen might allow the queen at (%1,%2) to move to a safe square.").arg(otherQueen->row()).arg(otherQueen->col())});
                        queenToMove->setPosition(originalRow, originalCol);
                        emit queenMoved();
                        return true; // Found one future move
                    }
                }

                queenToMove->setPosition(originalRow, originalCol);
                emit queenMoved();

            }
        }
    }
    return false;
}

bool ChessBoard::canCreateFutureSafeMove(Queen *movedQueen) {
    for (Queen *otherQueen : queens) {
        if (movedQueen == otherQueen)
            continue;

        // Check if any position becomes safe for the other queen
        for (int row = 0; row < boardSize; ++row) {
            for (int col = 0; col < boardSize; ++col) {
                if (isSquareSafe(otherQueen, row, col))
                    return true;  // A future safe move is possible!!
            }
        }
    }
    return false;  // No future safe moves found
}

bool ChessBoard::isSquareSafe(Queen *ignoreQueen, int row, int col) {
    for (Queen *queen : queens) {
        // Skip the given queen
        if (queen == ignoreQueen)
            continue;

        // Check if any queen threatens the (row, col) position
        if (queen->row() == row ||                        // Same row
            queen->col() == col ||                        // Same column
            std::abs(queen->row() - row) == std::abs(queen->col() - col)) {  // Same diagonal
            return false;  // Conflict detected
        }
    }
    return true;  // The square is safe
}



bool ChessBoard::isBlocking(Queen *queen) {
    for (Queen *otherQueen : queens) {
        if (queen != otherQueen) {
            if (queen->row() == otherQueen->row() ||    // Same row
                queen->col() == otherQueen->col() ||    // Same column
                std::abs(queen->row() - otherQueen->row()) == std::abs(queen->col() - otherQueen->col())) {  // Same diagonal
                return true;  // There is a conflict :P
            }
        }
    }
    return false;  // No conflict! :D
}

QPair<int, int> ChessBoard::findSafeMoveForQueen(Queen *queen) {
    // Try to find a safe move for the given queen that will resolve its conflicts
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            // Check if the square is safe
            bool safe = true;
            for (Queen *otherQueen : queens) {
                if (queen != otherQueen && 
                    (otherQueen->row() == row || otherQueen->col() == col || 
                    std::abs(otherQueen->row() - row) == std::abs(otherQueen->col() - col))) {
                    safe = false;  // Conflict found
                    break;
                }
            }

            // If the square is safe and not the queen's current position, return it
            if (safe && !(row == queen->row() && col == queen->col())) {
                return QPair<int, int>(row, col);
            }
        }
    }
    // No safe move found
    return QPair<int, int>(-1, -1);
}

bool ChessBoard::isQueenAt(int row, int col) {
    for (const auto& queen : queens) {
        if (queen->row() == row && queen->col() == col) {
            return true;
        }
    }
    return false;
}

void ChessBoard::solvePuzzle() {
    // Disable the menu actions and UI
    QMenuBar *menuBar = parentWidget()->findChild<QMenuBar *>();
    if (menuBar) {
        for (QAction *action : menuBar->actions()) {
            action->setEnabled(false);
        }
    }

    setInteractive(false);
    for (QWidget *child : findChildren<QWidget *>()) {
        child->setEnabled(false);
    }

    // Find a solution
    std::vector<std::pair<int, int>> solution = findSolution();

    // Create a vector to store coordinates of already existing queens on the board
    std::vector<std::pair<int, int>> existingQueens;

    // Iterate through the board to find existing queens and store their positions
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if (isQueenAt(row, col)) {  // Check if there is a queen at this position
                existingQueens.push_back({row, col});
            }
        }
    }

    // Loop through the solution and "move" queens to their new positions
    for (size_t i = 0; i < solution.size(); ++i) {
        int row = solution[i].first;
        int col = solution[i].second;

        // If there are queens already on the board, remove the last one
        if (!existingQueens.empty()) {
            // Get the last queen's position
            std::pair<int, int> lastQueen = existingQueens.back();
            int prevRow = lastQueen.first;
            int prevCol = lastQueen.second;

            // Remove the last OLD queen from the board
            removeQueen(prevRow, prevCol);

            // Remove the last OLD queen from the list
            existingQueens.pop_back();
        }

        // Place the NEW queen in the current solution's position
        addQueen(row, col);

        // Create a blocking delay of 500 ms for animation purposes
        QEventLoop loop;
        QTimer::singleShot(500, &loop, &QEventLoop::quit); 
        loop.exec();
    }

    // Re-enable UI interactions after solving the puzzle
    setInteractive(true);
    for (QWidget *child : findChildren<QWidget *>()) {
        child->setEnabled(true);
    }

    // Re-enable the menu actions after solving the puzzle
    if (menuBar) {
        for (QAction *action : menuBar->actions()) {
            action->setEnabled(true);
        }
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
            if (solveBacktrack(board, row + 1, solution)) {  // Descent
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