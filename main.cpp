#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec(); // ✅ Corrigido: usar a instância local em vez de QCoreApplication::exec()
}