#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btnCarregar_clicked();

    void on_btnCinza_clicked();

    void on_btnBinarizar_clicked();

    void on_btnSalvar_clicked();

    void on_btnPassaBaixas_clicked();

    void on_btnHistograma_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
