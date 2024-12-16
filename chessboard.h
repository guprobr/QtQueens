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
    void resetGame();

    void clearQueens();

signals:
    void queenMoved();  // Emitted whenever a queen is moved

private:
    QGraphicsScene *scene;
    int boardSize; // Dynamic board size
    QList<Queen *> queens;

    void onQueenDropped(int row, int col);
};

#endif // CHESSBOARD_H
