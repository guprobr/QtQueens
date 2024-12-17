#include "mainwindow.h"
#include <QInputDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QApplication>
#include <QVBoxLayout>

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
    // Inside your MainWindow constructor or initialization function:

    // 1. Create "About" Menu
    QMenu *menuAbout = new QMenu("About", this);
    menuBar->addMenu(menuAbout);

    // 2. Add "About Qt" Action
    QAction *actionAboutQt = new QAction("About Qt", this);
    menuAbout->addAction(actionAboutQt);
    connect(actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // 3. Add "About QtQueens" Action
    QAction *actionAboutQtQueens = new QAction("About QtQueens", this);
    menuAbout->addAction(actionAboutQtQueens);

    connect(actionAboutQtQueens, &QAction::triggered, this, &MainWindow::showAboutQtQueensDialog);

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

// 4. Define the About QtQueens Dialog
void MainWindow::showAboutQtQueensDialog() {
    // Create dialog
    QDialog aboutDialog(this);
    aboutDialog.setWindowTitle("About QtQueens");
    aboutDialog.setFixedSize(800, 600); // Fixed dialog size

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(&aboutDialog);

    // Add Logo (Queens PNG)
    QLabel *logoLabel = new QLabel(&aboutDialog);
    QPixmap logoPixmap(":/resources/queen.png"); // Load the existing logo
    logoLabel->setPixmap(logoPixmap.scaled(100, 100, Qt::KeepAspectRatio));
    logoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(logoLabel);

    // Add Text
    QLabel *textLabel = new QLabel(&aboutDialog);
    textLabel->setText(
        "<h2>QtQueens Puzzle App</h2>"
        "<p>A graphical and interactive implementation of the classic N-Queens Puzzle using the Qt6 framework.</p>"
        "<p>Made by <b>Gustavo L Conte</b> - Visit my website: <a href='https://gu.pro.br'>https://gu.pro.br</a></p>"
        "<p>The code is available on GitHub: <a href='https://github.com/guprobr/QtQueens'>https://github.com/guprobr/QtQueens</a></p>"
    );
    textLabel->setTextFormat(Qt::RichText);
    textLabel->setOpenExternalLinks(true); // Enable clickable links
    textLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(textLabel);

    // Add OK Button
    QPushButton *okButton = new QPushButton("OK", &aboutDialog);
    layout->addWidget(okButton);
    connect(okButton, &QPushButton::clicked, &aboutDialog, &QDialog::accept);

    // Show dialog
    aboutDialog.exec();
}