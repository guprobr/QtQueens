#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMenuBar>
#include <QLabel>
#include "utils.h"
#include "queen.h"

class ChessBoard : public QGraphicsView {
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);

    void setBoardSize(int size);
    void drawBoard();
    void addQueen(int row, int col);
    bool checkConflicts();
    void solvePuzzle();
    void resetGame();
    void clearQueens();

signals:
    void queenMoved();  // Emitted whenever a queen is moved

private slots:
    void performHintMove(int fromRow, int fromCol, int toRow, int toCol);


private:
    QGraphicsScene *scene;
    int boardSize; // Dynamic board size
    bool boardNeedsRedraw; // flag to only redraw squares if necessary

    QList<Queen *> queens;
    void onQueenDropped(int row, int col);
    bool isQueenAt(int row, int col);
    void removeQueen(int row, int col);

    struct Hint {
        int fromRow;
        int fromCol;
        int toRow;
        int toCol;
        QColor color;
        QString description;
    };


    void showHint();  // Show a hint "sound system" ye ! !!
    int calculateConflicts();
    int calculateConflictsAt(int row, int col);
    bool suggestLeastConflictMove(QList<ChessBoard::Hint>& hints);
    bool suggestConflictBreaker(QList<ChessBoard::Hint>& hints);
    Queen* findQueenWithMostConflicts();
    int calculateConflictsForQueen(Queen* targetQueen);
    bool suggestRandomMove(QList<ChessBoard::Hint>& hints);
    bool canCreateFutureSafeMove(Queen *movedQueen);
    bool suggestFutureSafeMove(QList<ChessBoard::Hint>& hints);
    QPair<int, int> findSafeMoveForQueen(Queen *queen);
    bool isBlocking(Queen *queen);
    bool isSquareSafe(Queen *ignoreQueen, int row, int col);
        
    std::vector<std::pair<int, int>> findSolution();
    bool solveBacktrack(std::vector<int>& board, int row, std::vector<std::pair<int, int>>& solution);
    bool isSafe(const std::vector<int>& board, int row, int col);

    void highlightSquare(int row, int col, QColor color);  // Helper function to highlight a square

    void resetChronometer();
    void updateChronometer();
    QTimer *chronometerTimer;  // Timer for the chronometer
    QLabel *chronometerLabel;  // Label to display the chronometer
    int elapsedSeconds;        // Keeps track of elapsed time

};

#endif // CHESSBOARD_H
