
import cv2 as cv
import numpy as np
import os


stickerSelecionado = None


# MouseSticker.py
def aplicar_sticker(fundo, sticker, x, y):

    hs, ws = sticker.shape[:2]

    b, g, r, a = cv.split(sticker)
    stickerBGR = cv.merge([b, g, r])

    alpha = a / 255.0
    alpha = cv.merge([alpha, alpha, alpha])

    roi = fundo[y:y+hs, x:x+ws]
    resultado = roi * (1 - alpha) + stickerBGR * alpha
    fundo[y:y+hs, x:x+ws] = resultado.astype(np.uint8)
    return fundo


# MouseSticker.py
def clique_mouse(event, x, y, flags, param):
    if event == cv.EVENT_LBUTTONDOWN:
        posicoesStickers.append((x, y))


def mostrar_filtros():
    print("\n1 Blur | 2Gaussian | 3 Sharpen | 4 Sobel H")
    print("5 Sobel  | 6 Canny | 7 Laplaciano | 8 Dilatacao")
    print("9 Erosao | 10 Grayscale | 11 Negativo | 12 Saturacao")


def aplicar_filtro(img, op):
# reduz ruidos e detalhes
    if op == "1":
        resultado = cv.blur(img, (5, 5))#maior, mais desfoque
#desfoca a imagem
    elif op == "2":
        resultado = cv.GaussianBlur(img, (5, 5), 1.5)#media ponderada
#aumenta a nitidez
    elif op == "3":
        kernel = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]])
        resultado = cv.filter2D(img, -1, kernel)
        #horozontais y, verticais x
#bordas horizontais 
    elif op == "4":
        if len(img.shape) == 3:
            img = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        resultado = cv.Sobel(img, cv.CV_64F, 1, 0, 3)
        resultado = cv.convertScaleAbs(resultado)
#bordas verticais
    elif op == "5":
        if len(img.shape) == 3:
            img = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        resultado = cv.Sobel(img, cv.CV_64F, 0, 1, 3)
        resultado = cv.convertScaleAbs(resultado)
#detecção de bordas
    elif op == "6":
        if len(img.shape) == 3:
            img = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        imgBlur = cv.GaussianBlur(img, (5, 5), 1.5)
        resultado = cv.Canny(imgBlur, 100, 150)
#destaca bordas em diferentes direcoes
    elif op == "7":
        if len(img.shape) == 3:
            img = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        resultado = cv.Laplacian(img, cv.CV_64F)
        resultado = cv.convertScaleAbs(resultado)
#aumenta regioes claras
    elif op == "8":
        kernel = np.ones((3, 3), np.uint8)
        resultado = cv.dilate(img, kernel)
#diminui regioes claras 
    elif op == "9":
        kernel = np.ones((3, 3), np.uint8)
        resultado = cv.erode(img, kernel)
#cinza
    elif op == "10":
        resultado = cv.cvtColor(img, cv.COLOR_BGR2GRAY) if len(img.shape) == 3 else img.copy()
#inverte
    elif op == "11":
        resultado = cv.bitwise_not(img)
#saturacao
    elif op == "12":
        if len(img.shape) == 2:
            resultado = img.copy()
        else:
            hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
            h, s, v = cv.split(hsv)
            s = cv.multiply(s, 1.5)
            s = np.clip(s, 0, 255).astype(np.uint8)
            resultado = cv.cvtColor(cv.merge([h, s, v]), cv.COLOR_HSV2BGR)

    else:
        resultado = img.copy()

    return resultado


# HelloChannels.py
def selecionar_canal(img):

    print("\n 1- Grayscale(cinza)  2- B 3- G 4- R")
    op = input("Canal: ")

  
    if op == "1":
        return cv.cvtColor(img, cv.COLOR_BGR2GRAY)

    elif op in ["2", "3", "4"]:

        img_canal = img.copy()

        altura = img.shape[0]
        largura = img.shape[1]

        for i in range(altura):
            for j in range(largura):

                # Obtem os valores dos canais B, G e R
                b = img.item(i, j, 0)
                g = img.item(i, j, 1)
                r = img.item(i, j, 2)

                if op == "2":
                    valor = b
                elif op == "3":
                    valor = g
                else:
                    valor = r

                # Aplica o canal escolhido nos tres canais
                img_canal[i, j, 0] = valor
                img_canal[i, j, 1] = valor
                img_canal[i, j, 2] = valor

        return img_canal

   
    return img.copy()


def operacoes_matematicas(img):

    caminho = input("Caminho da segunda imagem: ")
    img2 = cv.imread(caminho)

    if img2 is None:
        print("Erro ao carregar a segunda imagem.")
        return img.copy()

    if len(img.shape) == 2:
        img = cv.cvtColor(img, cv.COLOR_GRAY2BGR)

    img2 = cv.resize(img2, (img.shape[1], img.shape[0]))

    print("1- Adicao  2- Subtracao  3- Blending")
    op = input("Operacao: ")

    if op == "1":
        resultado = cv.add(img, img2)

    elif op == "2":
        resultado = cv.subtract(img, img2)

    elif op == "3":
        resultado = cv.addWeighted(img, 0.5, img2, 0.5, 0)

    else:
        resultado = img.copy()

    return resultado


def modo_foto():

    global stickerSelecionado, posicoesStickers

    caminho = input("Caminho da imagem: ")
    imgOriginal = cv.imread(caminho)

    if imgOriginal is None:
        print("erro")
        return

    imgResultado = imgOriginal.copy()
    posicoesStickers = []
    stickerSelecionado = None

    janela = "Modo Foto"

    cv.namedWindow(janela)
    cv.setMouseCallback(janela, clique_mouse)

    while True:

        imgTela = imgResultado.copy()

        if stickerSelecionado is not None:
            for x, y in posicoesStickers:
                imgTela = aplicar_sticker(
                    imgTela, stickerSelecionado, x, y
                )

        cv.imshow(janela, imgTela)
        tecla = cv.waitKey(1)

        print("\nMODO FOTO ")
        print("1 Selecionar canal")
        print("2 Aplicar filtro")
        print("3 Operacoes com duas imagens")
        print("4 Escolher sticker")
        print("5 Resetar")
        print("6 Salvar")
        print("7 Voltar")

        op = input("Escolha:")

        if op == "1":

            imgResultado = selecionar_canal(imgOriginal)
            posicoesStickers = []

        elif op == "2":

            mostrar_filtros()
            codigo = input("Filtro: ")

            if codigo in [str(i) for i in range(1, 13)]:
                imgResultado = aplicar_filtro(imgResultado, codigo)
                posicoesStickers = []
            else:
                print("Filtro invalido.")

        elif op == "3":

            imgResultado = operacoes_matematicas(imgResultado)
            posicoesStickers = []

        elif op == "4":

            print("1 estrela  2 coracao  3 oculos  4 carinha  5 balao")
            n = input("Sticker: ")

            nomes = {
                "1": "estrela.png",
                "2": "coracao.png",
                "3": "oculos.png",
                "4": "sorriso.png",
                "5": "balao.png"
            }

            if n in nomes:

                caminhoSticker = os.path.join(
                    os.path.dirname(os.path.abspath(__file__)),
                    "stickers",
                    nomes[n]
                )

                stickerSelecionado = cv.imread(
                    caminhoSticker,
                    cv.IMREAD_UNCHANGED
                )

                if stickerSelecionado is None:
                    print("Nao foi possivel carregar o sticker.")
                    stickerSelecionado = None

                elif len(stickerSelecionado.shape) != 3 or stickerSelecionado.shape[2] != 4:
                    print("O sticker precisa ser um PNG com transparencia.")
                    stickerSelecionado = None

                else:

                    print("Clique na imagem para posicionar.")
                    print("Pressione q  para terminar.")

                    posicoesStickers = []

                    while True:

                        imgTela = imgResultado.copy()

                        for x, y in posicoesStickers:
                            imgTela = aplicar_sticker(
                                imgTela, stickerSelecionado, x, y
                            )

                        cv.imshow(janela, imgTela)

                        tecla = cv.waitKey(1) & 0xFF

                        if tecla == ord("q"):
                            break

            else:
                print("Sticker invalido.")

        elif op == "5":

            imgResultado = imgOriginal.copy()
            posicoesStickers = []
            stickerSelecionado = None

        elif op == "6":

            nome = input("Nome do arquivo: ")

            imagemSalvar = imgResultado.copy()

            if stickerSelecionado is not None:
                for x, y in posicoesStickers:
                    imagemSalvar = aplicar_sticker(
                        imagemSalvar, stickerSelecionado, x, y
                    )

            print(
                "Salvo." if cv.imwrite(nome, imagemSalvar)
                else "Erro ao salvar."
            )

        elif op == "7":

            cv.destroyAllWindows()
            break

def modo_video():

    cap = cv.VideoCapture(0)
    print("1 Blur  2 Canny  3 Original  s Salvar  q Sair")
    filtro = "3"

    while True:

        ret, frame = cap.read()
        if not ret:
            break
        if filtro == "1":
            resultado = cv.blur(frame, (5, 5))
        elif filtro == "2":
            gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            gray = cv.GaussianBlur(gray, (5, 5), 1.5)
            resultado = cv.Canny(gray, 100, 150)
        else:
            resultado = frame.copy()
        cv.imshow("Modo video", resultado)

        tecla = cv.waitKey(1) & 0xFF
        if tecla == ord("q"):
            break
        elif tecla == ord("1"):
            filtro = "1"
        elif tecla == ord("2"):
            filtro = "2"
        elif tecla == ord("3"):
            filtro = "3"
        elif tecla == ord("s"):
            cv.imwrite("salvo.png", resultado)
            print("Salvo.")

    cap.release()
    cv.destroyAllWindows()


def main():

    while True:
        print("1 Modo Foto")
        print("2 Modo Video")
        op = input("Escolha: ")

        if op == "1":
            modo_foto()

        elif op == "2":
            modo_video()


if __name__ == "__main__":
    main()
