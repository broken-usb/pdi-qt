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
    // Calcula o valor de cinza perceptual de uma cor usando a fórmula ITU-R BT.601.
    // Mais preciso que a média simples (r+g+b)/3 pois pondera cada canal
    // de acordo com a sensibilidade do olho humano.
    static int grayValue(const QColor &cor);

    // Desenha o histograma de luminância de uma imagem no lblHistograma.
    // Elimina a duplicação entre on_btnHistograma e on_btnHistogramaProcessada.
    void drawHistogram(const QImage &img);

    // Salva a imagem em m_imagemProcessada e exibe no label.
    // Centraliza toda atualização do painel direito.
    void setProcessedImage(const QImage &img);

    // Retorna o tamanho N do kernel selecionado no combo (ex: "3x3" → 3).
    int getKernelSize() const;

    Ui::MainWindow *ui;

    // Imagens armazenadas como member variables em vez de ficarem presas
    // dentro do QLabel (pixmap). Isso torna o acesso mais direto,
    // evita reconversões desnecessárias e facilita futuras extensões.
    QImage m_imagemOriginal;
    QImage m_imagemProcessada;
};

#endif // MAINWINDOW_H