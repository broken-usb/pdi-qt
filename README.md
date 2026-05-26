# PDI-Qt: Processamento Digital de Imagens

Este projeto é uma ferramenta de Processamento Digital de Imagens (PDI) desenvolvida em C++ utilizando o framework Qt 6. O objetivo é aplicar conceitos teóricos de manipulação de matrizes de pixels, filtros espaciais e transformações geométricas através de uma interface gráfica de uso direto.

## Funcionalidades

### Manipulação Básica
* **Tons de Cinza:** Converte a imagem colorida utilizando a fórmula perceptual de luminância ITU-R BT.601.
* **Binarização:** Transforma a imagem em preto e branco absoluto com base em um limiar (threshold) numérico.
* **Brilho:** Ajusta a claridade somando ou subtraindo uma constante inteira de todos os canais RGB.
* **Quantização:** Reduz a paleta de cores da imagem para um número restrito de níveis de intensidade.
* **Mascarar:** Aplica uma operação lógica AND utilizando uma segunda imagem como máscara binária (tons escuros escondem, tons claros revelam).

### Filtros Passa-Baixas (Suavização)
* **Média:** Reduz ruídos de alta frequência substituindo cada pixel pela média aritmética da sua vizinhança no kernel NxN.
* **Mediana:** Remove ruídos isolados do tipo "sal e pimenta" selecionando o valor central ordenado da vizinhança.
* **Gaussiano:** Aplica um desfoque suave e natural utilizando um kernel gerado dinamicamente via distribuição normal com sigma ponderado (sigma = N/6.0).
* **Moda:** Substitui o pixel pelo tom mais frequente na vizinhança para uniformizar texturas e agrupar regiões comuns.
* **Ordem K:** Filtro não-linear que permite selecionar o k-ésimo menor valor ordenado dentro da matriz do kernel.

### Filtros Passa-Altas (Realce e Bordas)
* **Sobel:** Destaca contornos calculando a magnitude do gradiente nos eixos X e Y com pesos centralizados diferenciados.
* **Laplaciano:** Operador isotrópico de segunda derivada que detecta transições abruptas em todas as direções simultaneamente.
* **Prewitt:** Realça bordas de forma semelhante ao Sobel, mas utilizando pesos uniformes nas máscaras de convolução.
* **HighBoost:** Aumenta a nitidez (sharpening) da imagem ao amplificar e somar a máscara de detalhes de alta frequência de volta à imagem original.

### Geometria e Transformações
* **Espelhamento:** Inverte a matriz de pixels horizontalmente, criando um efeito de espelho no eixo vertical.
* **Rotação de 90°:** Gira a imagem atual em 90 graus no sentido horário, operando de forma acumulativa sobre a edição ativa.

### Efeitos Criativos
* **ASCII Art:** Converte a imagem em uma composição baseada em caracteres de texto coloridos, compensando matematicamente a proporção retangular da fonte monoespaçada para evitar distorções.

### Análise de Imagem
* **Histograma (Original e Editada):** Gera gráficos em tempo real mostrando a distribuição exata da quantidade de pixels por nível de luminância.
* **Equalização:** Maximiza o contraste da imagem distribuindo uniformemente as frequências acumuladas (CDF) de forma independente para cada canal de cor (R, G, B).

## Como Compilar e Executar

Este projeto suporta oficialmente a compilação e execução exclusiva através do ambiente de desenvolvimento integrado **Qt Creator**.

1. Abra o **Qt Creator**.
2. Acesse o menu `File` > `Open File or Project...` e selecione o arquivo `CMakeLists.txt` localizado na raiz do projeto.
3. Configure o projeto escolhendo o seu Kit de compilação disponível (ex: Desktop Qt 6.x.x MSVC/GCC/Clang).
4. Clique no botão de execução **Play** (ícone verde no canto inferior esquerdo) para compilar e iniciar o aplicativo automaticamente.

## Créditos e Licenças

O ícone utilizado no aplicativo foi obtido a partir da seguinte fonte:
* **Ícone da Câmera:** Disponível em [Game-Icons.net](https://game-icons.net/1x1/delapouite/photo-camera.html#download).
* **Autor:** [Delapouite](https://delapouite.com/).
* **Licença:** [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/).