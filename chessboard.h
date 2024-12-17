#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "utils.h"
#include "queen.h"

class ChessBoard : public QGraphicsView {
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);

    void setBoardSize(int size); // New method to set board size
    void drawBoard();
    void addQueen(int row, int col);
    bool checkConflicts();
    void solvePuzzle();
    void resetGame();
    void clearQueens();

signals:
    void queenMoved();  // Emitted whenever a queen is moved

private:
    QGraphicsScene *scene;
    int boardSize; // Dynamic board size
    QList<Queen *> queens;

    void onQueenDropped(int row, int col);
    
    std::vector<std::pair<int, int>> findSolution();
    bool solveBacktrack(std::vector<int>& board, int row, std::vector<std::pair<int, int>>& solution);
    bool isSafe(const std::vector<int>& board, int row, int col);


};

#endif // CHESSBOARD_H
