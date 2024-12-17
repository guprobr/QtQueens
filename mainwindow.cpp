#include "mainwindow.h"
#include <QInputDialog>
#include <QPushButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), chessBoard(new ChessBoard(this)) {

    // Set ChessBoard as the central widget
    setCentralWidget(chessBoard);

    // Create Menu Bar
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // Create "Options" Menu
    menuOptions = new QMenu("Options", this);
    menuBar->addMenu(menuOptions);

    // Add "Set Board Size" Action
    actionSetBoardSize = new QAction("Set Board Size", this);
    QAction *solutionAction = new QAction("SOLUTION", this);

    menuOptions->addAction(actionSetBoardSize);
    menuOptions->addAction(solutionAction);

    // Connect "Set Board Size" Action
    connect(actionSetBoardSize, &QAction::triggered, this, [this]() {
        bool ok;
        int size = QInputDialog::getInt(this, "Set Board Size",
                                        "Enter board size (e.g., 8 for 8x8):",
                                        8, 4, 25, 1, &ok); // Min: 4, Max: 25
        if (ok) {
            chessBoard->setBoardSize(size); // Update board size
            addQueens(size);               // Dynamically add queens
        }
    });

    connect(solutionAction, &QAction::triggered, this, &MainWindow::onSolvePuzzle);

    // Connect the chessBoard's signals
    connect(chessBoard, &ChessBoard::queenMoved, this, [this]() {
        bool solved = chessBoard->checkConflicts();
    });

    // Initial setup: Add queens for default board size
    addQueens(8);
}

void MainWindow::addQueens(int boardSize) {
    chessBoard->clearQueens(); // Remove any existing queens
    for (int i = 0; i < boardSize; ++i) {
        chessBoard->addQueen(i, i); // Place queens initially along the diagonal
    }
}

void MainWindow::onSolvePuzzle() {
    chessBoard->solvePuzzle();  // Call the solvePuzzle function of ChessBoard
}
