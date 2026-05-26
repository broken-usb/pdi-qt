#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

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
    void on_btnHighBoost_clicked();
    void on_btnEqualizar_clicked();
    void on_btnAscii_clicked();

private:
    // Converte a cor em valor de cinza perceptual.
    static int grayValue(const QColor &cor);

    // Desenha o histograma de luminância.
    void drawHistogram(const QImage &img);

    // Atualiza a imagem processada e o label correspondente.
    void setProcessedImage(const QImage &img);

    // Retorna o tamanho do kernel selecionado.
    int getKernelSize() const;

    Ui::MainWindow *ui;

    // Imagens originais e processadas.
    QImage m_imagemOriginal;
    QImage m_imagemProcessada;
};

#endif // MAINWINDOW_H