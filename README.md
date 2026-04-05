# PDI-Qt: Processamento Digital de Imagens

Este projeto é uma ferramenta de Processamento Digital de Imagens (PDI) desenvolvida em **C++** utilizando o framework **Qt 6**. O objetivo é aplicar conceitos teóricos de manipulação de matrizes de pixels, filtros espaciais e transformações geométricas através de uma interface gráfica moderna inspirada no *Fluent Design*.

## Funcionalidades

* **Manipulação:** Brilho, Binarização, Tons de Cinza e Quantização.
* **Filtros Passa-Baixas:** Média, Gaussiano, Mediana, Moda e Ordem K (com tamanho de Kernel configurável).
* **Geometria:** Rotação de 90° e Espelhamento.
* **Análise:** Geração de Histograma em tempo real (Original e Processada).
* **Lógica:** Aplicação de máscaras de imagem (Operação AND).

---

## Como Compilar e Executar

### 1. Pré-requisitos
Você precisará do **CMake** (3.16+) e de um compilador C++ (GCC, Clang ou MSVC). Além disso, é necessário instalar o **Qt 6 SDK**.

### 2. Instalando Dependências (Linux)

Escolha o comando de acordo com a sua distribuição:

* **Ubuntu / Debian / Mint:**
    ```bash
    sudo apt update
    sudo apt install cmake g++ qt6-base-dev libgl1-mesa-dev
    ```

* **Fedora / RHEL:**
    ```bash
    sudo dnf install cmake gcc-c++ qt6-qtbase-devel mesa-libGL-devel
    ```

* **Arch Linux:**
    ```bash
    sudo pacman -S cmake gcc qt6-base
    ```

### 3. Compilando (Windows & Linux)

Abra o terminal na pasta raiz do projeto e execute:

```bash
# 1. Configurar o projeto
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 2. Compilar
cmake --build build --config Release
```

O executável será gerado dentro da pasta `build`.

---

## Tecnologias Utilizadas
* **Linguagem:** C++17
* **Interface:** Qt 6 (Widgets)
* **Build System:** CMake
* **Estilo:** QSS (Qt Style Sheets) para Fluent Design