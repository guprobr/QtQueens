#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setMaximumSize(QSize(740, 740));
    window.show();

    return app.exec();
}
