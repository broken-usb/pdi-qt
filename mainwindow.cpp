#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

// ─────────────────────────────────────────────────────────────
//  Construtor / Destrutor
// ─────────────────────────────────────────────────────────────

// Monta a janela principal e bloqueia o botão de maximizar.
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1000, 600);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
}

// Libera a interface gerada pelo Qt Designer.
MainWindow::~MainWindow()
{
    delete ui;
}

// ─────────────────────────────────────────────────────────────
//  Helpers privados
// ─────────────────────────────────────────────────────────────

// Fórmula perceptual ITU-R BT.601.
// O olho humano é mais sensível ao verde (~59%) do que ao vermelho (~30%)
// e muito menos ao azul (~11%). Usar (r+g+b)/3 ignora isso e produz
// resultados visivelmente mais escuros/claros do que o esperado.
int MainWindow::grayValue(const QColor &cor)
{
    return qRound(0.299 * cor.red() + 0.587 * cor.green() + 0.114 * cor.blue());
}

// Desenha o histograma de luminância de 'img' no label de histograma.
// Evita duplicar o mesmo bloco de ~25 linhas em dois slots diferentes.
void MainWindow::drawHistogram(const QImage &img)
{
    int largura = img.width();
    int altura  = img.height();

    int hist[256] = {0};
    int maxVal = 0;

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            int cinza = grayValue(img.pixelColor(x, y));
            hist[cinza]++;
            if (hist[cinza] > maxVal) maxVal = hist[cinza];
        }
    }

    if (maxVal == 0) return;

    QPixmap grafico(256, 200);
    grafico.fill(Qt::white);

    QPainter pintor(&grafico);
    pintor.setPen(Qt::black);

    for (int i = 0; i < 256; i++) {
        int alturaBarra = (hist[i] * 200) / maxVal;
        pintor.drawLine(i, 200, i, 200 - alturaBarra);
    }

    ui->lblHistograma->setPixmap(grafico);
}

// Salva 'img' em m_imagemProcessada e exibe no painel direito.
// Todos os slots de processamento passam por aqui, garantindo
// que a variável e o label estejam sempre sincronizados.
void MainWindow::setProcessedImage(const QImage &img)
{
    m_imagemProcessada = img;
    ui->lblProcessada->setPixmap(QPixmap::fromImage(img));
}

// Lê o tamanho N do kernel a partir do combo (ex: "3x3" → 3).
int MainWindow::getKernelSize() const
{
    return ui->cbTamanhoKernel->currentText().split("x")[0].toInt();
}

// ─────────────────────────────────────────────────────────────
//  Carregar / Salvar
// ─────────────────────────────────────────────────────────────

// Abre um arquivo de imagem, redimensiona se necessário e exibe.
// Também guarda a imagem em m_imagemOriginal para acesso posterior.
void MainWindow::on_btnCarregar_clicked()
{
    QString nomeArquivo = QFileDialog::getOpenFileName(this,
        "Abrir Imagem", "", "Imagens (*.png *.jpg *.jpeg *.bmp)");

    if (nomeArquivo.isEmpty()) return;

    QPixmap imagem(nomeArquivo);
    if (imagem.isNull()) {
        QMessageBox::warning(this, "Erro", "Não foi possível carregar a imagem.");
        return;
    }

    if (imagem.width() > 1280 || imagem.height() > 720) {
        // Qt::KeepAspectRatio: mantém a proporção para a imagem não ficar esticada.
        // Qt::SmoothTransformation: aplica suavização bilinear para não serrilhar as bordas.
        imagem = imagem.scaled(1280, 720, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    m_imagemOriginal   = imagem.toImage();
    m_imagemProcessada = QImage();

    ui->lblOriginal->setPixmap(imagem);
    ui->lblProcessada->clear();
    ui->lblHistograma->clear();
}

// Salva a imagem processada em disco.
void MainWindow::on_btnSalvar_clicked()
{
    if (m_imagemProcessada.isNull()) {
        QMessageBox::warning(this, "Aviso", "Não há nenhuma imagem processada para salvar!");
        return;
    }

    QString nomeArquivo = QFileDialog::getSaveFileName(this,
        "Salvar Imagem Processada", "", "Imagens PNG (*.png);;Imagens JPEG (*.jpg)");

    if (nomeArquivo.isEmpty()) return;

    if (QPixmap::fromImage(m_imagemProcessada).save(nomeArquivo)) {
        QMessageBox::information(this, "Sucesso", "Imagem salva com sucesso!");
    } else {
        QMessageBox::critical(this, "Erro", "Ocorreu um problema ao salvar a imagem.");
    }
}

// ─────────────────────────────────────────────────────────────
//  Histogramas
// ─────────────────────────────────────────────────────────────

// Gera o histograma da imagem original.
void MainWindow::on_btnHistograma_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }
    drawHistogram(m_imagemOriginal);
}

// Gera o histograma da imagem processada.
void MainWindow::on_btnHistogramaProcessada_clicked()
{
    if (m_imagemProcessada.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, aplique algum filtro na imagem primeiro!");
        return;
    }
    drawHistogram(m_imagemProcessada);
}

// ─────────────────────────────────────────────────────────────
//  Manipulação básica
// ─────────────────────────────────────────────────────────────

// Converte a imagem original para escala de cinza.
void MainWindow::on_btnCinza_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgProcessada = m_imagemOriginal.copy();
    int largura = imgProcessada.width();
    int altura  = imgProcessada.height();

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            int cinza = grayValue(imgProcessada.pixelColor(x, y));
            imgProcessada.setPixelColor(x, y, QColor(cinza, cinza, cinza));
        }
    }

    setProcessedImage(imgProcessada);
}

// Binariza a imagem com o limiar informado pelo usuário.
void MainWindow::on_btnBinarizar_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int limiar = ui->edtLimiar->text().toInt();
    QImage imgProcessada = m_imagemOriginal.copy();
    int largura = imgProcessada.width();
    int altura  = imgProcessada.height();

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            int cinza    = grayValue(imgProcessada.pixelColor(x, y));
            QColor novaCor = (cinza >= limiar) ? QColor(255, 255, 255) : QColor(0, 0, 0);
            imgProcessada.setPixelColor(x, y, novaCor);
        }
    }

    setProcessedImage(imgProcessada);
}

// Ajusta o brilho somando uma constante a cada canal RGB.
void MainWindow::on_btnBrilho_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int constante = ui->edtBrilho->text().toInt();
    QImage imgProcessada = m_imagemOriginal.copy();
    int largura = imgProcessada.width();
    int altura  = imgProcessada.height();

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            QColor cor = imgProcessada.pixelColor(x, y);
            int r = qBound(0, cor.red()   + constante, 255);
            int g = qBound(0, cor.green() + constante, 255);
            int b = qBound(0, cor.blue()  + constante, 255);
            imgProcessada.setPixelColor(x, y, QColor(r, g, b));
        }
    }

    setProcessedImage(imgProcessada);
}

// Reduz a paleta de cores ao número de níveis indicado.
void MainWindow::on_btnQuantizacao_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int niveis = ui->edtQuantizacao->text().toInt();
    if (niveis < 2) {
        QMessageBox::warning(this, "Aviso", "O número de níveis deve ser no mínimo 2.");
        return;
    }

    QImage imgProcessada = m_imagemOriginal.copy();
    int largura = imgProcessada.width();
    int altura  = imgProcessada.height();

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            QColor cor = imgProcessada.pixelColor(x, y);

            int nivelR = (cor.red()   * niveis) / 256;
            int nivelG = (cor.green() * niveis) / 256;
            int nivelB = (cor.blue()  * niveis) / 256;

            int novoR = (nivelR * 255) / (niveis - 1);
            int novoG = (nivelG * 255) / (niveis - 1);
            int novoB = (nivelB * 255) / (niveis - 1);

            imgProcessada.setPixelColor(x, y, QColor(novoR, novoG, novoB));
        }
    }

    setProcessedImage(imgProcessada);
}

// ─────────────────────────────────────────────────────────────
//  Filtros Passa-Baixas
// ─────────────────────────────────────────────────────────────

// Filtro de média: substitui cada pixel pela média dos vizinhos no kernel NxN.
void MainWindow::on_btnPassaBaixas_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int N = getKernelSize();
    int offset      = N / 2;
    int totalPixels = N * N;

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    for (int y = offset; y < altura - offset; y++) {
        for (int x = offset; x < largura - offset; x++) {
            int somaR = 0, somaG = 0, somaB = 0;
            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);
                    somaR += cor.red();
                    somaG += cor.green();
                    somaB += cor.blue();
                }
            }
            imgProcessada.setPixelColor(x, y, QColor(somaR / totalPixels,
                                                      somaG / totalPixels,
                                                      somaB / totalPixels));
        }
    }

    setProcessedImage(imgProcessada);
}

// Filtro de mediana: substitui cada pixel pela mediana dos vizinhos no kernel NxN.
void MainWindow::on_btnMediana_clicked()
{
    if (m_imagemOriginal.isNull()) return;

    int N = getKernelSize();
    int offset      = N / 2;
    int totalPixels = N * N;

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    for (int y = offset; y < altura - offset; y++) {
        for (int x = offset; x < largura - offset; x++) {
            std::vector<int> vR, vG, vB;
            vR.reserve(totalPixels);
            vG.reserve(totalPixels);
            vB.reserve(totalPixels);

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);
                    vR.push_back(cor.red());
                    vG.push_back(cor.green());
                    vB.push_back(cor.blue());
                }
            }

            std::sort(vR.begin(), vR.end());
            std::sort(vG.begin(), vG.end());
            std::sort(vB.begin(), vB.end());

            imgProcessada.setPixelColor(x, y, QColor(vR[totalPixels / 2],
                                                      vG[totalPixels / 2],
                                                      vB[totalPixels / 2]));
        }
    }

    setProcessedImage(imgProcessada);
}

// Filtro Gaussiano com kernel NxN gerado dinamicamente.
// Melhoria: o código original usava um kernel 3x3 hardcoded e ignorava
// o combo de tamanho. Agora o kernel é calculado para qualquer N usando
// sigma = N/6.0 (regra prática para conter 99.7% da distribuição no kernel).
void MainWindow::on_btnGaussiano_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int N      = getKernelSize();
    int offset = N / 2;
    double sigma = N / 6.0;

    // Gera e normaliza o kernel Gaussiano
    std::vector<std::vector<double>> kernel(N, std::vector<double>(N));
    double somaKernel = 0.0;

    for (int ky = -offset; ky <= offset; ky++) {
        for (int kx = -offset; kx <= offset; kx++) {
            double val = std::exp(-(kx * kx + ky * ky) / (2.0 * sigma * sigma));
            kernel[ky + offset][kx + offset] = val;
            somaKernel += val;
        }
    }
    for (int ky = 0; ky < N; ky++)
        for (int kx = 0; kx < N; kx++)
            kernel[ky][kx] /= somaKernel;

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    for (int y = offset; y < altura - offset; y++) {
        for (int x = offset; x < largura - offset; x++) {
            double somaR = 0.0, somaG = 0.0, somaB = 0.0;

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);
                    double peso = kernel[ky + offset][kx + offset];
                    somaR += cor.red()   * peso;
                    somaG += cor.green() * peso;
                    somaB += cor.blue()  * peso;
                }
            }

            imgProcessada.setPixelColor(x, y, QColor(qBound(0, (int)somaR, 255),
                                                      qBound(0, (int)somaG, 255),
                                                      qBound(0, (int)somaB, 255)));
        }
    }

    setProcessedImage(imgProcessada);
}

// Filtro de Ordem K: seleciona o k-ésimo menor valor do kernel (ordenado).
void MainWindow::on_btnOrdemK_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int N           = getKernelSize();
    int totalPixels = N * N;
    int k           = ui->edtK->text().toInt();
    int offset      = N / 2;

    if (k < 0 || k >= totalPixels) {
        QMessageBox::warning(this, "Aviso",
            "Para este tamanho de Kernel, K deve estar entre 0 e "
            + QString::number(totalPixels - 1));
        return;
    }

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    for (int y = offset; y < altura - offset; y++) {
        for (int x = offset; x < largura - offset; x++) {
            std::vector<int> vR, vG, vB;
            vR.reserve(totalPixels);
            vG.reserve(totalPixels);
            vB.reserve(totalPixels);

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);
                    vR.push_back(cor.red());
                    vG.push_back(cor.green());
                    vB.push_back(cor.blue());
                }
            }

            std::sort(vR.begin(), vR.end());
            std::sort(vG.begin(), vG.end());
            std::sort(vB.begin(), vB.end());

            imgProcessada.setPixelColor(x, y, QColor(vR[k], vG[k], vB[k]));
        }
    }

    setProcessedImage(imgProcessada);
}

// Filtro de moda: substitui cada pixel pelo valor mais frequente no kernel.
void MainWindow::on_btnModa_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int N      = getKernelSize();
    int offset = N / 2;

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    for (int y = offset; y < altura - offset; y++) {
        for (int x = offset; x < largura - offset; x++) {
            std::map<int, int> freqR, freqG, freqB;
            QColor corCentro = imgOriginal.pixelColor(x, y);

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    QColor v = imgOriginal.pixelColor(x + kx, y + ky);
                    freqR[v.red()]++;
                    freqG[v.green()]++;
                    freqB[v.blue()]++;
                }
            }

            auto calcularModa = [](const std::map<int, int> &freq, int vCentro) {
                int maxRep = 0, valorModa = vCentro;
                for (auto const &[cor, cont] : freq) {
                    if (cont > maxRep) {
                        maxRep = cont;
                        valorModa = cor;
                    } else if (cont == maxRep) {
                        if (std::abs(cor - vCentro) < std::abs(valorModa - vCentro))
                            valorModa = cor;
                    }
                }
                return valorModa;
            };

            imgProcessada.setPixelColor(x, y, QColor(
                calcularModa(freqR, corCentro.red()),
                calcularModa(freqG, corCentro.green()),
                calcularModa(freqB, corCentro.blue())));
        }
    }

    setProcessedImage(imgProcessada);
}

// ─────────────────────────────────────────────────────────────
//  Filtros de Detecção de Bordas (Passa-Altas)
// ─────────────────────────────────────────────────────────────

// Detector de bordas de Sobel (magnitude do gradiente).
void MainWindow::on_btnSobel_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    // Matriz de Sobel para detectar bordas Verticais (Eixo X)
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    // Matriz de Sobel para detectar bordas Horizontais (Eixo Y)
    int Gy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    for (int y = 1; y < altura - 1; y++) {
        for (int x = 1; x < largura - 1; x++) {
            int somaRx = 0, somaGx = 0, somaBx = 0;
            int somaRy = 0, somaGy = 0, somaBy = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    QColor cor  = imgOriginal.pixelColor(x + kx, y + ky);
                    int pesoX   = Gx[ky + 1][kx + 1];
                    int pesoY   = Gy[ky + 1][kx + 1];

                    somaRx += cor.red()   * pesoX;
                    somaGx += cor.green() * pesoX;
                    somaBx += cor.blue()  * pesoX;

                    somaRy += cor.red()   * pesoY;
                    somaGy += cor.green() * pesoY;
                    somaBy += cor.blue()  * pesoY;
                }
            }

            int magR = static_cast<int>(std::sqrt(somaRx * somaRx + somaRy * somaRy));
            int magG = static_cast<int>(std::sqrt(somaGx * somaGx + somaGy * somaGy));
            int magB = static_cast<int>(std::sqrt(somaBx * somaBx + somaBy * somaBy));

            imgProcessada.setPixelColor(x, y, QColor(qBound(0, magR, 255),
                                                      qBound(0, magG, 255),
                                                      qBound(0, magB, 255)));
        }
    }

    setProcessedImage(imgProcessada);
}

// Detector de bordas Laplaciano (segunda derivada).
void MainWindow::on_btnLaplaciano_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    // Máscara Laplaciana com centro 8 (detecta bordas em todas as direções)
    int mascara[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    for (int y = 1; y < altura - 1; y++) {
        for (int x = 1; x < largura - 1; x++) {
            int somaR = 0, somaG = 0, somaB = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);
                    int peso   = mascara[ky + 1][kx + 1];
                    somaR += cor.red()   * peso;
                    somaG += cor.green() * peso;
                    somaB += cor.blue()  * peso;
                }
            }

            // O Laplaciano produz valores negativos nas bordas.
            // std::abs converte os negativos e qBound limita a [0, 255].
            imgProcessada.setPixelColor(x, y, QColor(qBound(0, std::abs(somaR), 255),
                                                      qBound(0, std::abs(somaG), 255),
                                                      qBound(0, std::abs(somaB), 255)));
        }
    }

    setProcessedImage(imgProcessada);
}

// Detector de bordas de Prewitt (semelhante ao Sobel, pesos uniformes).
void MainWindow::on_btnPrewitt_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    // Matriz de Prewitt para Eixo X
    int Gx[3][3] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };
    // Matriz de Prewitt para Eixo Y
    int Gy[3][3] = {
        {-1, -1, -1},
        { 0,  0,  0},
        { 1,  1,  1}
    };

    for (int y = 1; y < altura - 1; y++) {
        for (int x = 1; x < largura - 1; x++) {
            int somaRx = 0, somaGx = 0, somaBx = 0;
            int somaRy = 0, somaGy = 0, somaBy = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    QColor cor  = imgOriginal.pixelColor(x + kx, y + ky);
                    int pesoX   = Gx[ky + 1][kx + 1];
                    int pesoY   = Gy[ky + 1][kx + 1];

                    somaRx += cor.red()   * pesoX;
                    somaGx += cor.green() * pesoX;
                    somaBx += cor.blue()  * pesoX;

                    somaRy += cor.red()   * pesoY;
                    somaGy += cor.green() * pesoY;
                    somaBy += cor.blue()  * pesoY;
                }
            }

            // Correção: o código original atribuía std::sqrt (double) a int sem
            // static_cast, o que é uma conversão implícita potencialmente imprecisa.
            int magR = static_cast<int>(std::sqrt(somaRx * somaRx + somaRy * somaRy));
            int magG = static_cast<int>(std::sqrt(somaGx * somaGx + somaGy * somaGy));
            int magB = static_cast<int>(std::sqrt(somaBx * somaBx + somaBy * somaBy));

            imgProcessada.setPixelColor(x, y, QColor(qBound(0, magR, 255),
                                                      qBound(0, magG, 255),
                                                      qBound(0, magB, 255)));
        }
    }

    setProcessedImage(imgProcessada);
}

// ─────────────────────────────────────────────────────────────
//  Realce e Análise
// ─────────────────────────────────────────────────────────────

// Filtro HighBoost: realça detalhes amplificando a diferença entre
// o original e uma versão borrada. A = 1 equivale ao Unsharpen Mask.
void MainWindow::on_btnHighBoost_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    double A = ui->edtHighBoost->text().toDouble();

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    for (int y = 1; y < altura - 1; y++) {
        for (int x = 1; x < largura - 1; x++) {
            int somaR = 0, somaG = 0, somaB = 0;

            // 1. Calcula o Passa-Baixas (média 3x3 = versão borrada)
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);
                    somaR += cor.red();
                    somaG += cor.green();
                    somaB += cor.blue();
                }
            }

            int mediaR = somaR / 9;
            int mediaG = somaG / 9;
            int mediaB = somaB / 9;

            QColor corOriginal = imgOriginal.pixelColor(x, y);

            // 2. Máscara: Detalhes = Original - Borrado
            int mascaraR = corOriginal.red()   - mediaR;
            int mascaraG = corOriginal.green() - mediaG;
            int mascaraB = corOriginal.blue()  - mediaB;

            // 3. HighBoost: Resultado = Original + (A × Máscara)
            int novoR = qBound(0, (int)(corOriginal.red()   + A * mascaraR), 255);
            int novoG = qBound(0, (int)(corOriginal.green() + A * mascaraG), 255);
            int novoB = qBound(0, (int)(corOriginal.blue()  + A * mascaraB), 255);

            imgProcessada.setPixelColor(x, y, QColor(novoR, novoG, novoB));
        }
    }

    setProcessedImage(imgProcessada);
}

// Equalização de histograma por canal (R, G, B independentes).
// Melhoria em relação ao código original, que convertia a imagem para
// escala de cinza antes de equalizar — perdendo toda a informação de cor.
// Equalizar cada canal de forma independente mantém as cores e melhora
// o contraste em imagens coloridas.
void MainWindow::on_btnEqualizar_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgOriginal   = m_imagemOriginal;
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();
    int totalPixels = largura * altura;

    // 1. Calcula o histograma de cada canal separadamente
    int histR[256] = {0}, histG[256] = {0}, histB[256] = {0};

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            QColor cor = imgOriginal.pixelColor(x, y);
            histR[cor.red()]++;
            histG[cor.green()]++;
            histB[cor.blue()]++;
        }
    }

    // 2. Calcula o mapa CDF (histograma acumulado normalizado) para cada canal
    int mapaR[256] = {0}, mapaG[256] = {0}, mapaB[256] = {0};
    int acR = 0, acG = 0, acB = 0;

    for (int i = 0; i < 256; i++) {
        acR += histR[i];  mapaR[i] = (acR * 255) / totalPixels;
        acG += histG[i];  mapaG[i] = (acG * 255) / totalPixels;
        acB += histB[i];  mapaB[i] = (acB * 255) / totalPixels;
    }

    // 3. Aplica o mapeamento a cada pixel
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            QColor cor = imgOriginal.pixelColor(x, y);
            imgProcessada.setPixelColor(x, y, QColor(mapaR[cor.red()],
                                                      mapaG[cor.green()],
                                                      mapaB[cor.blue()]));
        }
    }

    setProcessedImage(imgProcessada);
}

// ─────────────────────────────────────────────────────────────
//  Geometria
// ─────────────────────────────────────────────────────────────

// Espelha a imagem horizontalmente (eixo vertical).
void MainWindow::on_btnEspelhar_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgOriginal = m_imagemOriginal;
    int largura = imgOriginal.width();
    int altura  = imgOriginal.height();

    QImage imgProcessada(largura, altura, imgOriginal.format());

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            imgProcessada.setPixelColor(largura - 1 - x, y, imgOriginal.pixelColor(x, y));
        }
    }

    setProcessedImage(imgProcessada);
}

// Rotaciona a imagem 90° no sentido horário.
// Opera sobre a imagem processada se existir (permitindo rotações acumulativas),
// ou sobre a original caso contrário.
void MainWindow::on_btnRotacionar90_clicked()
{
    QImage imgAtual = m_imagemProcessada.isNull() ? m_imagemOriginal : m_imagemProcessada;

    if (imgAtual.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int largura = imgAtual.width();
    int altura  = imgAtual.height();

    // A imagem rotacionada tem largura e altura invertidas
    QImage imgProcessada(altura, largura, imgAtual.format());

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            int novoX = altura - 1 - y;
            int novoY = x;
            imgProcessada.setPixelColor(novoX, novoY, imgAtual.pixelColor(x, y));
        }
    }

    setProcessedImage(imgProcessada);
}

// ─────────────────────────────────────────────────────────────
//  Máscara Lógica (AND)
// ─────────────────────────────────────────────────────────────

// Aplica uma máscara binária: pixels brancos da máscara mantêm a cor
// original; pixels escuros tornam-se preto.
void MainWindow::on_btnAplicarMascara_clicked()
{
    QImage imgBase = m_imagemProcessada.isNull() ? m_imagemOriginal : m_imagemProcessada;

    if (imgBase.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem base primeiro!");
        return;
    }

    QString nomeArquivo = QFileDialog::getOpenFileName(this,
        "Escolher Imagem de Máscara", "", "Imagens (*.png *.jpg *.bmp *.jpeg)");

    if (nomeArquivo.isEmpty()) return;

    QImage imgMascara(nomeArquivo);
    if (imgMascara.isNull()) {
        QMessageBox::warning(this, "Erro", "Não foi possível carregar o arquivo da máscara.");
        return;
    }

    imgMascara = imgMascara.scaled(imgBase.width(), imgBase.height(), Qt::IgnoreAspectRatio);

    QImage imgProcessada = imgBase.copy();

    for (int y = 0; y < imgBase.height(); y++) {
        for (int x = 0; x < imgBase.width(); x++) {
            QColor corBase    = imgBase.pixelColor(x, y);
            QColor corMascara = imgMascara.pixelColor(x, y);

            // Usa grayValue perceptual para decidir a luminosidade da máscara
            int luminosidade = grayValue(corMascara);
            imgProcessada.setPixelColor(x, y, (luminosidade < 128) ? Qt::black : corBase);
        }
    }

    setProcessedImage(imgProcessada);
}

// ─────────────────────────────────────────────────────────────
//  Arte ASCII
// ─────────────────────────────────────────────────────────────

// Converte a imagem em arte ASCII colorida: cada pixel vira um caractere
// cuja densidade visual corresponde ao brilho do pixel, desenhado com
// a cor original do pixel.
void MainWindow::on_btnAscii_clicked()
{
    if (m_imagemOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgOriginal = m_imagemOriginal;

    // 1. Pegamos a fonte primeiro para saber as proporções do caractere
    QFont fonte("Courier", 5);
    fonte.setStyleHint(QFont::Monospace);
    QFontMetrics fm(fonte);

    int charW = fm.horizontalAdvance('M');
    int charH = fm.height();

    // 2. Calcula o tamanho da grade compensando o "esticamento" da fonte
    int larguraAscii = 150;

    // A mágica acontece aqui: multiplicamos a altura pelo fator (charW / charH)
    // para compensar o fato de que a letra é mais alta do que larga.
    int alturaAscii = (imgOriginal.height() * larguraAscii * charW) / (imgOriginal.width() * charH);

    // Redimensiona ignorando a proporção original, pois já calculamos a proporção compensada
    QImage imgPequena = imgOriginal.scaled(larguraAscii, alturaAscii, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    int larg = imgPequena.width();
    int alt  = imgPequena.height();

    // 3. Mapa de caracteres: do mais denso (preto) ao mais leve (branco)
    const QString chars = "@%#*+=-:. ";
    int numChars = chars.length();

    // 4. Cria a imagem de saída com fundo preto (tamanho final exato da grade * letras)
    QImage imgAscii(larg * charW, alt * charH, QImage::Format_RGB32);
    imgAscii.fill(Qt::black);

    QPainter pintor(&imgAscii);
    pintor.setFont(fonte);

    for (int y = 0; y < alt; y++) {
        for (int x = 0; x < larg; x++) {
            QColor cor = imgPequena.pixelColor(x, y);
            int cinza  = grayValue(cor);

            // Mapeia o brilho para um índice no mapa de caracteres
            int   idx = (cinza * (numChars - 1)) / 255;
            QChar c   = chars[idx];

            // Desenha o caractere com a cor original do pixel
            pintor.setPen(cor);
            pintor.drawText(x * charW, y * charH + fm.ascent(), QString(c));
        }
    }

    setProcessedImage(imgAscii);
}