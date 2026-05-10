#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

// monta a janela principal e bloqueia maximizar
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1000, 600);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
}

// libera a interface
MainWindow::~MainWindow()
{
    delete ui;
}

// pega uma imagem, ajusta para resolução HD (720p) e mostra na esquerda
void MainWindow::on_btnCarregar_clicked()
{
    QString nomeArquivo = QFileDialog::getOpenFileName(this,
                                                       "Abrir Imagem", "", "Imagens (*.png *.jpg *.jpeg *.bmp)");

    if (nomeArquivo.isEmpty()) {
        return;
    }

    QPixmap imagem(nomeArquivo);

    if (imagem.isNull()) {
        QMessageBox::warning(this, "Erro", "Não foi possível carregar a imagem.");
        return;
    }

    if (imagem.width() > 1280 || imagem.height() > 720) {

        // Redimensiona a imagem aplicando duas regras importantes:
        // Qt::KeepAspectRatio: Mantém a proporção para a imagem não ficar esticada.
        // Qt::SmoothTransformation: Aplica suavização bilinear para não "serrilhar" as bordas.
        imagem = imagem.scaled(1280, 720, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    ui->lblOriginal->setPixmap(imagem);

    ui->lblProcessada->clear();
    ui->lblHistograma->clear();
}

// transforma a imagem em cinza
void MainWindow::on_btnCinza_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();

    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imagemProcessada = pixmapOriginal.toImage();

    int largura = imagemProcessada.width();
    int altura = imagemProcessada.height();

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            QColor corOriginal = imagemProcessada.pixelColor(x, y);

            int r = corOriginal.red();
            int g = corOriginal.green();
            int b = corOriginal.blue();

            int cinza = (r + g + b) / 3;

            QColor novaCor(cinza, cinza, cinza);

            imagemProcessada.setPixelColor(x, y, novaCor);
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imagemProcessada));
}

// deixa imagem só preto ou branco
void MainWindow::on_btnBinarizar_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();

    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int limiar = ui->edtLimiar->text().toInt();

    QImage imagemProcessada = pixmapOriginal.toImage();

    int largura = imagemProcessada.width();
    int altura = imagemProcessada.height();

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            QColor corOriginal = imagemProcessada.pixelColor(x, y);

            int r = corOriginal.red();
            int g = corOriginal.green();
            int b = corOriginal.blue();
            int cinza = (r + g + b) / 3;

            if (cinza >= limiar) {
                imagemProcessada.setPixelColor(x, y, QColor(255, 255, 255));
            } else {
                imagemProcessada.setPixelColor(x, y, QColor(0, 0, 0));
            }
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imagemProcessada));
}

// salva a imagem processada
void MainWindow::on_btnSalvar_clicked()
{
    QPixmap pixmapProcessada = ui->lblProcessada->pixmap();

    if (pixmapProcessada.isNull()) {
        QMessageBox::warning(this, "Aviso", "Não há nenhuma imagem processada para salvar!");
        return;
    }

    QString nomeArquivo = QFileDialog::getSaveFileName(this,
                                                       "Salvar Imagem Processada", "", "Imagens PNG (*.png);;Imagens JPEG (*.jpg)");

    if (nomeArquivo.isEmpty()) {
        return;
    }

    if (pixmapProcessada.save(nomeArquivo)) {
        QMessageBox::information(this, "Sucesso", "Imagem salva com sucesso!");
    } else {
        QMessageBox::critical(this, "Erro", "Ocorreu um problema ao salvar a imagem.");
    }
}

// aplica um filtro de desfoque leve
void MainWindow::on_btnPassaBaixas_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QString textoCombo = ui->cbTamanhoKernel->currentText();
    int N = textoCombo.split("x")[0].toInt();

    int offset = N / 2;
    int totalPixels = N * N;

    QImage imgOriginal = pixmapOriginal.toImage();
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura = imgOriginal.height();

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

            int mediaR = somaR / totalPixels;
            int mediaG = somaG / totalPixels;
            int mediaB = somaB / totalPixels;

            imgProcessada.setPixelColor(x, y, QColor(mediaR, mediaG, mediaB));
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

// desenha o histograma da original
void MainWindow::on_btnHistograma_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage img = pixmapOriginal.toImage();
    int largura = img.width();
    int altura = img.height();

    int vetorHistograma[256] = {0};
    int valorMaximo = 0;

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            QColor cor = img.pixelColor(x, y);

            int cinza = (cor.red() + cor.green() + cor.blue()) / 3;

            vetorHistograma[cinza]++;

            if (vetorHistograma[cinza] > valorMaximo) {
                valorMaximo = vetorHistograma[cinza];
            }
        }
    }

    QPixmap grafico(256, 200);
    grafico.fill(Qt::white);

    if (valorMaximo == 0) return;

    QPainter pintor(&grafico);
    pintor.setPen(Qt::black);

    for (int i = 0; i < 256; i++) {
        int alturaBarra = (vetorHistograma[i] * 200) / valorMaximo;
        pintor.drawLine(i, 200, i, 200 - alturaBarra);
    }

    ui->lblHistograma->setPixmap(grafico);
}

// mexe no brilho da imagem
void MainWindow::on_btnBrilho_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int constante = ui->edtBrilho->text().toInt();

    QImage imgProcessada = pixmapOriginal.toImage();
    int largura = imgProcessada.width();
    int altura = imgProcessada.height();

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            QColor corOriginal = imgProcessada.pixelColor(x, y);

            int r = qBound(0, corOriginal.red() + constante, 255);
            int g = qBound(0, corOriginal.green() + constante, 255);
            int b = qBound(0, corOriginal.blue() + constante, 255);

            imgProcessada.setPixelColor(x, y, QColor(r, g, b));
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

// reduz a paleta de cores
void MainWindow::on_btnQuantizacao_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int niveis = ui->edtQuantizacao->text().toInt();

    if (niveis < 2) {
        QMessageBox::warning(this, "Aviso", "O número de níveis deve ser no mínimo 2.");
        return;
    }

    QImage imgProcessada = pixmapOriginal.toImage();
    int largura = imgProcessada.width();
    int altura = imgProcessada.height();

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            QColor cor = imgProcessada.pixelColor(x, y);

            int nivelR = (cor.red() * niveis) / 256;
            int nivelG = (cor.green() * niveis) / 256;
            int nivelB = (cor.blue() * niveis) / 256;

            int novoR = (nivelR * 255) / (niveis - 1);
            int novoG = (nivelG * 255) / (niveis - 1);
            int novoB = (nivelB * 255) / (niveis - 1);

            imgProcessada.setPixelColor(x, y, QColor(novoR, novoG, novoB));
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

// aplica filtro de mediana no kernel
void MainWindow::on_btnMediana_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) return;

    int N = ui->cbTamanhoKernel->currentText().split("x")[0].toInt();
    int offset = N / 2;
    int totalPixels = N * N;

    QImage imgOriginal = pixmapOriginal.toImage();
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura = imgOriginal.height();

    for (int y = offset; y < altura - offset; y++) {
        for (int x = offset; x < largura - offset; x++) {

            std::vector<int> valoresR, valoresG, valoresB;

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);
                    valoresR.push_back(cor.red());
                    valoresG.push_back(cor.green());
                    valoresB.push_back(cor.blue());
                }
            }

            std::sort(valoresR.begin(), valoresR.end());
            std::sort(valoresG.begin(), valoresG.end());
            std::sort(valoresB.begin(), valoresB.end());

            imgProcessada.setPixelColor(x, y, QColor(valoresR[totalPixels/2],
                                                     valoresG[totalPixels/2],
                                                     valoresB[totalPixels/2]));
        }
    }
    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

// suaviza com máscara gaussiana
void MainWindow::on_btnGaussiano_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgOriginal = pixmapOriginal.toImage();
    QImage imgProcessada = imgOriginal.copy();

    int largura = imgOriginal.width();
    int altura = imgOriginal.height();

    int mascara[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    for (int y = 1; y < altura - 1; y++) {
        for (int x = 1; x < largura - 1; x++) {

            int somaR = 0, somaG = 0, somaB = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {

                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);

                    int peso = mascara[ky + 1][kx + 1];

                    somaR += cor.red() * peso;
                    somaG += cor.green() * peso;
                    somaB += cor.blue() * peso;
                }
            }

            imgProcessada.setPixelColor(x, y, QColor(somaR / 16, somaG / 16, somaB / 16));
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

// aplica filtro de ordem K
void MainWindow::on_btnOrdemK_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int N = ui->cbTamanhoKernel->currentText().split("x")[0].toInt();
    int totalPixels = N * N;
    int k = ui->edtK->text().toInt();
    int offset = N / 2;

    if (k < 0 || k >= totalPixels) {
        QMessageBox::warning(this, "Aviso", "Para este tamanho de Kernel, K deve estar entre 0 e " + QString::number(totalPixels - 1));
        return;
    }

    QImage imgOriginal = pixmapOriginal.toImage();
    QImage imgProcessada = imgOriginal.copy();

    int largura = imgOriginal.width();
    int altura = imgOriginal.height();

    for (int y = offset; y < altura - offset; y++) {
        for (int x = offset; x < largura - offset; x++) {

            std::vector<int> valoresR, valoresG, valoresB;

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);
                    valoresR.push_back(cor.red());
                    valoresG.push_back(cor.green());
                    valoresB.push_back(cor.blue());
                }
            }

            std::sort(valoresR.begin(), valoresR.end());
            std::sort(valoresG.begin(), valoresG.end());
            std::sort(valoresB.begin(), valoresB.end());

            int corR = valoresR[k];
            int corG = valoresG[k];
            int corB = valoresB[k];

            imgProcessada.setPixelColor(x, y, QColor(corR, corG, corB));
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

// espelha imagem horizontalmente
void MainWindow::on_btnEspelhar_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgOriginal = pixmapOriginal.toImage();
    int largura = imgOriginal.width();
    int altura = imgOriginal.height();

    QImage imgProcessada(largura, altura, imgOriginal.format());

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            QColor cor = imgOriginal.pixelColor(x, y);

            int novoX = largura - 1 - x;

            imgProcessada.setPixelColor(novoX, y, cor);
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

// gira a imagem 90 graus
void MainWindow::on_btnRotacionar90_clicked()
{

    QPixmap pixmapAtual = ui->lblProcessada->pixmap();

    if (pixmapAtual.isNull()) {
        pixmapAtual = ui->lblOriginal->pixmap();
    }

    if (pixmapAtual.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgAtual = pixmapAtual.toImage();
    int largura = imgAtual.width();
    int altura = imgAtual.height();

    QImage imgProcessada(altura, largura, imgAtual.format());

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            QColor cor = imgAtual.pixelColor(x, y);

            int novoX = altura - 1 - y;
            int novoY = x;

            imgProcessada.setPixelColor(novoX, novoY, cor);
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

// faz histograma da imagem do lado direito
void MainWindow::on_btnHistogramaProcessada_clicked()
{

    QPixmap pixmapProcessada = ui->lblProcessada->pixmap();

    if (pixmapProcessada.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, aplique algum filtro na imagem primeiro!");
        return;
    }

    QImage img = pixmapProcessada.toImage();
    int largura = img.width();
    int altura = img.height();

    int vetorHistograma[256] = {0};
    int valorMaximo = 0;

    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {

            QColor cor = img.pixelColor(x, y);
            int cinza = (cor.red() + cor.green() + cor.blue()) / 3;

            vetorHistograma[cinza]++;

            if (vetorHistograma[cinza] > valorMaximo) {
                valorMaximo = vetorHistograma[cinza];
            }
        }
    }

    QPixmap grafico(256, 200);
    grafico.fill(Qt::white);

    if (valorMaximo == 0) return;

    QPainter pintor(&grafico);
    pintor.setPen(Qt::black);

    for (int i = 0; i < 256; i++) {
        int alturaBarra = (vetorHistograma[i] * 200) / valorMaximo;
        pintor.drawLine(i, 200, i, 200 - alturaBarra);
    }

    ui->lblHistograma->setPixmap(grafico);
}

// usa uma imagem de máscara pra escurecer partes
void MainWindow::on_btnAplicarMascara_clicked()
{

    QPixmap pixmapAtual = ui->lblProcessada->pixmap();
    if (pixmapAtual.isNull()) {
        pixmapAtual = ui->lblOriginal->pixmap();
    }

    if (pixmapAtual.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem base primeiro!");
        return;
    }

    QString nomeFicheiro = QFileDialog::getOpenFileName(this, "Escolher Imagem de Máscara", "", "Imagens (*.png *.jpg *.bmp *.jpeg)");

    if (nomeFicheiro.isEmpty()) {
        return;
    }

    QImage imgBase = pixmapAtual.toImage();
    QImage imgMascara(nomeFicheiro);

    if (imgMascara.isNull()) {
        QMessageBox::warning(this, "Erro", "Não foi possível carregar o ficheiro da máscara.");
        return;
    }

    imgMascara = imgMascara.scaled(imgBase.width(), imgBase.height(), Qt::IgnoreAspectRatio);

    QImage imgProcessada = imgBase.copy();

    for (int y = 0; y < imgBase.height(); y++) {
        for (int x = 0; x < imgBase.width(); x++) {

            QColor corBase = imgBase.pixelColor(x, y);
            QColor corMascara = imgMascara.pixelColor(x, y);

            int luminosidadeMascara = (corMascara.red() + corMascara.green() + corMascara.blue()) / 3;

            if (luminosidadeMascara < 128) {

                imgProcessada.setPixelColor(x, y, Qt::black);
            } else {

                imgProcessada.setPixelColor(x, y, corBase);
            }
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

// aplica filtro de moda no kernel
void MainWindow::on_btnModa_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    int N = ui->cbTamanhoKernel->currentText().split("x")[0].toInt();
    int offset = N / 2;

    QImage imgOriginal = pixmapOriginal.toImage();
    QImage imgProcessada = imgOriginal.copy();
    int largura = imgOriginal.width();
    int altura = imgOriginal.height();

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

            auto calcularModa = [](const std::map<int, int>& frequencias, int vCentro) {
                int maxRepeticoes = 0;
                int valorModa = vCentro;
                for (auto const& [cor, cont] : frequencias) {
                    if (cont > maxRepeticoes) {
                        maxRepeticoes = cont;
                        valorModa = cor;
                    } else if (cont == maxRepeticoes) {
                        if (std::abs(cor - vCentro) < std::abs(valorModa - vCentro))
                            valorModa = cor;
                    }
                }
                return valorModa;
            };

            imgProcessada.setPixelColor(x, y, QColor(calcularModa(freqR, corCentro.red()),
                                                     calcularModa(freqG, corCentro.green()),
                                                     calcularModa(freqB, corCentro.blue())));
        }
    }
    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

void MainWindow::on_btnSobel_clicked()
{
    QPixmap pixmapOriginal = ui->lblOriginal->pixmap();
    if (pixmapOriginal.isNull()) {
        QMessageBox::warning(this, "Aviso", "Por favor, carregue uma imagem primeiro!");
        return;
    }

    QImage imgOriginal = pixmapOriginal.toImage();
    QImage imgProcessada = imgOriginal.copy();

    int largura = imgOriginal.width();
    int altura = imgOriginal.height();

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

    // Varredura da imagem ignorando a borda de 1 pixel
    for (int y = 1; y < altura - 1; y++) {
        for (int x = 1; x < largura - 1; x++) {

            int somaRx = 0, somaGx = 0, somaBx = 0;
            int somaRy = 0, somaGy = 0, somaBy = 0;

            // Varredura do Kernel 3x3
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    QColor cor = imgOriginal.pixelColor(x + kx, y + ky);

                    int pesoX = Gx[ky + 1][kx + 1];
                    int pesoY = Gy[ky + 1][kx + 1];

                    // Acumula os valores de X
                    somaRx += cor.red() * pesoX;
                    somaGx += cor.green() * pesoX;
                    somaBx += cor.blue() * pesoX;

                    // Acumula os valores de Y
                    somaRy += cor.red() * pesoY;
                    somaGy += cor.green() * pesoY;
                    somaBy += cor.blue() * pesoY;
                }
            }

            int magR = static_cast<int>(std::sqrt(somaRx * somaRx + somaRy * somaRy));
            int magG = static_cast<int>(std::sqrt(somaGx * somaGx + somaGy * somaGy));
            int magB = static_cast<int>(std::sqrt(somaBx * somaBx + somaBy * somaBy));

            // Garante que o valor não ultrapasse o branco máximo (255)
            magR = qBound(0, magR, 255);
            magG = qBound(0, magG, 255);
            magB = qBound(0, magB, 255);

            imgProcessada.setPixelColor(x, y, QColor(magR, magG, magB));
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}