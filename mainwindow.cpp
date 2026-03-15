#include "mainwindow.h"
#include "./ui_mainwindow.h"
// Bibliotecas adicionadas
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

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

    int larguraLabel = ui->lblOriginal->width();
    int alturaLabel = ui->lblOriginal->height();

    QPixmap imagemRedimensionada = imagem.scaled(larguraLabel, alturaLabel, Qt::KeepAspectRatio);

    ui->lblOriginal->setPixmap(imagemRedimensionada);
}


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

            // Define a cor cinza
            int cinza = (r + g + b) / 3;

            // Aplica a cor cinza em toda a imagem
            QColor novaCor(cinza, cinza, cinza);

            imagemProcessada.setPixelColor(x, y, novaCor);
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imagemProcessada));
}


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

            // Lógica da binarização
            if (cinza >= limiar) {
                // Branco (255)
                imagemProcessada.setPixelColor(x, y, QColor(255, 255, 255));
            } else {
                // Preto (0)
                imagemProcessada.setPixelColor(x, y, QColor(0, 0, 0));
            }
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imagemProcessada));
}


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


void MainWindow::on_btnPassaBaixas_clicked()
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

    for (int y = 1; y < altura - 1; y++) {
        for (int x = 1; x < largura - 1; x++) {

            int somaR = 0, somaG = 0, somaB = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {

                    QColor corVizinho = imgOriginal.pixelColor(x + kx, y + ky);

                    somaR += corVizinho.red();
                    somaG += corVizinho.green();
                    somaB += corVizinho.blue();
                }
            }

            int mediaR = somaR / 9;
            int mediaG = somaG / 9;
            int mediaB = somaB / 9;

            imgProcessada.setPixelColor(x, y, QColor(mediaR, mediaG, mediaB));
        }
    }

    ui->lblProcessada->setPixmap(QPixmap::fromImage(imgProcessada));
}

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

    QPainter pintor(&grafico);
    pintor.setPen(Qt::black);

    for (int i = 0; i < 256; i++) {
        int alturaBarra = (vetorHistograma[i] * 200) / valorMaximo;
        pintor.drawLine(i, 200, i, 200 - alturaBarra);
    }

    ui->lblHistograma->setPixmap(grafico);
}

