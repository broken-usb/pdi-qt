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

    void on_btnBrilho_clicked();

    void on_btnQuantizacao_clicked();

    void on_btnMediana_clicked();

    void on_btnGaussiano_clicked();

    void on_btnOrdemK_clicked();

    void on_btnEspelhar_clicked();

    void on_btnRotacionar90_clicked();

    void on_btnHistogramaProcessada_clicked();

    void on_btnAplicarMascara_clicked();

    void on_btnModa_clicked();

    void on_btnSobel_clicked();

    void on_btnLaplaciano_clicked();

    void on_btnPrewitt_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
