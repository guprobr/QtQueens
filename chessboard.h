#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "queen.h"
#include "utils.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVector>

class ChessBoard : public QGraphicsView {
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);

    bool checkConflicts(); // Method to check for conflicts

    // Method to draw chessboard
    void drawBoard();
    // Method to add Queen
    void addQueen(int row, int col);

private:
    QGraphicsScene *scene;
    QVector<Queen *> queens; // The list of gorgeous QUEENs
};

#endif // CHESSBOARD_H
