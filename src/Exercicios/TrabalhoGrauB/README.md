# Projeto Grau B – Aplicativo de Edição de Imagens com OpenCV

**Integrante:** Luiz Felipe Hilgert

**Instituição:** Unisinos

---

## 1. Descrição do Projeto

Este projeto consiste no desenvolvimento de um aplicativo de edição de imagens utilizando Python e a biblioteca OpenCV.

O aplicativo possui dois modos de funcionamento: **Modo Foto**, para edição de imagens estáticas, e **Modo Vídeo**, para aplicação de filtros em tempo real utilizando a webcam.

O objetivo é aplicar conceitos de processamento digital de imagens, manipulação de canais de cores, operações matemáticas, filtros e interação com mouse e teclado.

## 2. Funcionalidades

### Modo Foto

- Carregamento de imagens.
- Seleção da imagem inteira ou dos canais B, G e R.
- Conversão para escala de cinza (Grayscale).
- Aplicação de 12 filtros:
  - Blur;
  - Gaussian Blur;
  - Sharpen;
  - Sobel Horizontal;
  - Sobel Vertical;
  - Canny;
  - Laplaciano;
  - Dilatação;
  - Erosão;
  - Grayscale;
  - Negativo;
  - Saturação.
- Operações matemáticas entre duas imagens:
  - Adição;
  - Subtração;
  - Blending.
- Inserção de cinco stickers com transparência Alpha.
- Posicionamento dos stickers por meio de cliques do mouse.
- Restauração da imagem original.
- Salvamento da imagem editada.

### Modo Vídeo

- Captura de imagens em tempo real pela webcam.
- Aplicação dos filtros Blur e Canny.
- Visualização da imagem original.
- Alternância entre os filtros pelo teclado.
- Captura e salvamento de frames.

## 3. Tecnologias Utilizadas

- **Python 3:** linguagem de programação utilizada no desenvolvimento.
- **OpenCV:** biblioteca para processamento e manipulação de imagens e vídeos.
- **NumPy:** biblioteca utilizada para operações com matrizes e manipulação de pixels.

## 4. Estrutura do Projeto

```text
GB_OpenCV/
│
├── main.py
├── requirements.txt
├── README.md
│
└── stickers/
    ├── estrela.png
    ├── coracao.png
    ├── oculos.png
    ├── sorriso.png
    └── balao.png
```

## 5. Instalação

É necessário ter Python instalado no computador.

Instale as dependências utilizando o comando:

```bash
pip install -r requirements.txt
```

Caso prefira, instale diretamente:

```bash
pip install opencv-python numpy
```

## 6. Execução

No terminal, acesse a pasta do projeto e execute:

```bash
python main.py
```

No menu principal, selecione:

- `1` – Modo Foto;
- `2` – Modo Vídeo;


### Utilização do Modo Foto

Ao selecionar o modo foto, informe o caminho da imagem que deseja editar.

Em seguida, utilize as opções apresentadas no menu para selecionar canais, aplicar filtros, realizar operações matemáticas, adicionar stickers, restaurar ou salvar a imagem.

Para posicionar os stickers, selecione um modelo e clique na posição desejada na janela da imagem.

Pressione `q` na janela para finalizar o posicionamento.

### Utilização do Modo Vídeo

Ao selecionar o modo vídeo, o aplicativo tentará acessar a webcam padrão do computador.

Controles disponíveis:

| Tecla | Função |
|---|---|
| `1` | Aplicar Blur |
| `2` | Aplicar Canny |
| `3` | Exibir imagem original |
| `s` | Salvar frame |
| `q` | Encerrar o modo vídeo |
