#include "mainwindow.h"
#include <QApplication>
#include <QIcon> // ícone do aplicativo

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Define o ícone do aplicativo
    a.setWindowIcon(QIcon(":/icone.png"));

    MainWindow w;
    w.show();
    return a.exec();
}
