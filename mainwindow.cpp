#include "mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), chessBoard(new ChessBoard(this)) {
    setCentralWidget(chessBoard);

    // TEST> Lets add eight Queens
    chessBoard->addQueen(0, 0);  
    chessBoard->addQueen(1, 2); 
    chessBoard->addQueen(3, 4);
    chessBoard->addQueen(4, 1);  
    
    chessBoard->addQueen(1, 7); 
    chessBoard->addQueen(3, 2);
    chessBoard->addQueen(0, 4);  
    chessBoard->addQueen(1, 6); 
    
    //QPushButton *resetButton = new QPushButton("Reset Game", this);
    //resetButton->setGeometry(10, 10, 100, 40);  // Position the button
}

MainWindow::~MainWindow() {}
