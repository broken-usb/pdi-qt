# PDI-Qt: Processamento Digital de Imagens

Este projeto é uma ferramenta de Processamento Digital de Imagens (PDI) desenvolvida em **C++** utilizando o framework **Qt 6**. O objetivo é aplicar conceitos teóricos de manipulação de matrizes de pixels, filtros espaciais e transformações geométricas através de uma interface gráfica moderna.

## Funcionalidades

* **Manipulação:** Brilho, Binarização, Tons de Cinza e Quantização.
* **Filtros Passa-Baixas:** Média, Gaussiano, Mediana, Moda e Ordem K (com tamanho de Kernel configurável).
* **Geometria:** Rotação de 90° e Espelhamento.
* **Análise:** Geração de Histograma em tempo real (Original e Processada).
* **Lógica:** Aplicação de máscaras de imagem (Operação AND).

### Como Compilar e Executar

#### **Opção A: Via Qt Creator (Recomendado)**
Este projeto foi desenvolvido utilizando o **Qt Creator**. Para rodar por aqui:
1. Abra o **Qt Creator**.
2. Vá em `File` > `Open File or Project` e selecione o arquivo `CMakeLists.txt` na raiz do projeto.
3. Escolha o seu Kit (ex: Desktop Qt 6.x.x MSVC/GCC).
4. Clique no botão de **Play** (ícone verde no canto inferior esquerdo).

#### **Opção B: Via Terminal (CMake)**
Se preferir usar o terminal, certifique-se de ter o CMake (3.16+) e o SDK do Qt 6 instalados.

**1. Instale as Dependências (Linux):**
* **Ubuntu/Debian:** `sudo apt install cmake g++ qt6-base-dev libgl1-mesa-dev`
* **Fedora/RHEL:** `sudo dnf install cmake gcc-c++ qt6-qtbase-devel mesa-libGL-devel`
* **Arch Linux:** `sudo pacman -S cmake gcc qt6-base`

**2. Compile o Projeto:**
```bash
# Configurar o projeto
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compilar
cmake --build build --config Release
```