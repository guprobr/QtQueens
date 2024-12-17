#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "chessboard.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:
    void onSolvePuzzle();

private:
    QMenuBar *menuBar;       // Menu bar
    QMenu *menuOptions;      // Options menu
    QAction *actionSetBoardSize; // Action to set board size

    ChessBoard *chessBoard;  // The chessboard widget

    void addQueens(int boardSize);

    void showAboutQtQueensDialog();
};

#endif // MAINWINDOW_H
