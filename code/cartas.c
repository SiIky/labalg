#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cartas.h"
#include "structs.h"

/*----------------------------------------------------------------------------*/
CARTA mao2carta (MAO carta)
{
    CARTA c;
    c.naipe = 0;
    for (c.valor = 0; carta > TERNOS; c.valor++)
        carta >>= 4;
    for (c.naipe = 0; (carta ^ 1); carta >>= 1)
        c.naipe++;
    return c;
}

/*----------------------------------------------------------------------------*/
/** \brief Devolve uma lista de cartas (pares naipes/figuras)

@param jogada   A jogada a converter
@return         Os pares naipe/figura ordenados por figuras
*/
CARTA* jogada2cartas (MAO jogada)
{
    static CARTA cartas[5];
    int i, w;
    /* fazer coisas aqui */
    for (i = w = 0; jogada > 0 && w < 5; jogada >>= 1, i++)
        if (jogada % 2 == 1)
            cartas[w++] = mao2carta((MAO) 1 << i);

    cartas[w].naipe = 20;
    cartas[w].valor = 20;

    return cartas;
}

/*----------------------------------------------------------------------------*/
int valores_iguais (CARTA cartas[])
{
    int i, res;
    for (i = 1, res = 0; (cartas[i].valor < 13 && cartas[i].naipe < 4 && (res = (cartas[i].valor == cartas[i-1].valor))); i++);
    return (i == 1) ? 1 : res;
}

/*----------------------------------------------------------------------------*/
unsigned int trailingZ (MAO n)
{
    unsigned int count;
    for (count = 0; n > 0 && (n ^ 1); n >>= 1, count++);
    return count;
}

/*----------------------------------------------------------------------------*/
/** \brief Devolve o número de bits a 1

@param n        Número a calcular
@return         O número de bits 1
*/
unsigned int bitsUm (MAO n)
{
    unsigned int count;
    for (count = 0; n > 0; count += n % 2, n >>= 1);
    return count;
}

/*----------------------------------------------------------------------------*/
/** \brief Verifica se uma jogada é válida

@param jogada           As cartas selecionadas
@param ult_jogada       A última jogada do último jogador
@return                 Devolve 1 se for válida, 0 caso contrário
*/
int jogada_valida (const ESTADO *e)
{
    int res = 0;
    MAO jogada = e->seleccao;
    MAO ult_jogada = e->ult_jogada[(e->ult_jogador_valido + 3) % 4];
    CARTA* cartas = jogada2cartas(jogada);
    CARTA* ult_cartas = jogada2cartas(ult_jogada);
    unsigned int bits = bitsUm(jogada);
    unsigned int ult_bits = bitsUm(ult_jogada);

    if (e->jogador == e->ult_jogador_valido) { /* pode jogar qq coisa */
        switch (bits) {
            case PLAY_SINGLE:
                res = 1;
                break;
            case PLAY_PAIR:
            case PLAY_TRIPLE:
                /* ha algum erro aqui */
                res = valores_iguais(cartas);
                break;
            case PLAY_FIVE:
                res = 1;
                break;
            default:
                res = 0;
                break;
        }
    } else if (bits == ult_bits) { /* tem de jogar de acordo com a ult jogada valida */
        switch (bits) {
            case PLAY_SINGLE:
                res = (jogada > ult_jogada);
                break;
            case PLAY_PAIR:
                res = (valores_iguais(cartas) && (ult_cartas[0].valor < cartas[0].valor || ult_cartas[1].naipe < cartas[1].naipe));
                break;
            case PLAY_TRIPLE:
                res = (valores_iguais(cartas) && (ult_cartas[0].valor < cartas[0].valor));
                break;
            case PLAY_FIVE:
                res = 1;
                break;
            default:
                res = 0;
                break;
        }
    } else {
        res = 0;
    }
    return res;
}

/*----------------------------------------------------------------------------*/
/** \brief Adiciona uma carta ao estado

@param e        Uma mão
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         A nova mão
*/
MAO add_carta (const MAO *e, const unsigned int idx)
{
    return (*e | ((MAO) 1 << idx));
}

/*----------------------------------------------------------------------------*/
/** \brief Remove uma carta do estado

@param e        Uma mão
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         A nova mão
*/
MAO rem_carta (const MAO *e, const unsigned int idx)
{
    return (*e & ~((MAO) 1 << idx));
}

/*----------------------------------------------------------------------------*/
/** \brief Verifica se uma carta pertence ao estado

@param e        Uma mão
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         1 se a carta existe, 0 caso contrário
*/
int carta_existe (MAO e, const unsigned int idx)
{
    return ((e >> idx) & ((MAO) 1));
}

/*----------------------------------------------------------------------------*/
/** \brief Imprime o botão Jogar

@param e        O estado actual do jogo
*/
void imprime_bjogar (ESTADO e)
{
    char link[MAXLEN];

    if (jogada_valida(&e)) {
        e.jogador = (e.jogador + 1) % 4;
        e.mao[0] = REM_SELECCAO(e.mao[0], e.seleccao);
        e.ult_jogada[0] = e.seleccao;
        e.seleccao = (MAO) 0;
        sprintf(link, "%s?q=%s", SCRIPT, estado2str(&e));
        printf(
            "\t<SVG WIDTH=%d HEIGHT=%d>"
            "<A XLINK:HREF=\"%s\">"
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:#%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"midle\" TEXT-ALIGNT=\"center\" FONT-FAMILY=\"serif\" FONT-WEIGHT=\"bold\">Jogar</TEXT></A></SVG>\n",
            SVG_WIDTH, SVG_HEIGHT,
            link,
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A,
            TXT_X, TXT_Y
        );
    } else {
        printf(
            "\t<SVG WIDTH=%d HEIGHT=%d>"
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:#%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"midle\" TEXT-ALIGNT=\"center\" FONT-FAMILY=\"serif\" FONT-WEIGHT=\"bold\">Jogar</TEXT></SVG>\n",
            SVG_WIDTH, SVG_HEIGHT,
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D,
            TXT_X, TXT_Y
        );
    }
}

/*----------------------------------------------------------------------------*/
/** \brief Imprime o botão Limpar

@param e        O estado actual do jogo
*/
void imprime_blimpar (ESTADO e)
{
    char link[MAXLEN];
    printf(
        "\t<SVG WIDTH=%d HEIGHT=%d>",
        SVG_WIDTH, SVG_HEIGHT
    );

    if (e.seleccao != 0) {
        e.seleccao = 0;
        sprintf(link, "%s?q=%s", SCRIPT, estado2str(&e));
        printf(
            "<A XLINK:HREF=\"%s\">"
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:#%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"midle\" TEXT-ALIGN=\"center\" FONT-FAMILY=\"serif\" FONT-WEIGHT=\"bold\">Limpar</TEXT></A></SVG>\n",
            link,
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A,
            TXT_X, TXT_Y
        );
    } else {
        printf(
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:#%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"midle\" TEXT-ALIGN=\"center\" FONT-FAMILY=\"serif\" FONT-WEIGHT=\"bold\">Limpar</TEXT></SVG>\n",
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D,
            TXT_X, TXT_Y
        );
    }
}

void imprime_ult_jogada (const char *path, const ESTADO *e)
{
    int j;                      /* jogador */
    int xc = XUC_INIT;          /* x inicial */
    int yc = YC_INIT;           /* y inicial */
    int yj = 0;                 /* tabuleiros dos jogadores */
    int i;
    MAO ult_jogada[4];
    for (i = 0; i < 4; i++)
        ult_jogada[i] = e->ult_jogada[i];

    for (j = 0; j < 4; yj += YJ_STEP, j++)
        for (i = 0; ult_jogada[j] > 0; ult_jogada[j] >>= 1, i++)
            if (ult_jogada[j] & (MAO) 1)
                imprime_carta(path, xc, yc, *e, i);
}

/*----------------------------------------------------------------------------*/
/** \brief Imprime o html correspondente a uma carta

@param path     O URL correspondente à pasta que contém todas as cartas
@param x        A coordenada x da carta
@param y        A coordenada y da carta
@param e        O estado actual do jogo
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
*/
void imprime_carta (const char *path, const int x, int y, ESTADO e, const unsigned int idx)
{
    if (carta_existe(e.seleccao, idx)) {
        y -= YC_SEL_STEP;
        e.seleccao = rem_carta(&e.seleccao, idx);
    } else {
        e.seleccao = add_carta(&e.seleccao, idx);
    }

    CARTA c = mao2carta((MAO) 1 << idx);
    char script[MAXLEN];
    sprintf(script, "%s?q=%s", SCRIPT, estado2str(&e));
    printf(
        "\t<SVG WIDTH=90 HEIGHT=120><A XLINK:HREF=\"%s\">"
        "<IMAGE X=%d Y=%d WIDTH=80 HEIGHT=110 XLINK:HREF=\"%s/%c%c.svg\"/></A></SVG>\n",
        script,
        x, y, path, VALORES[c.valor], NAIPES[c.naipe]
    );
}

/*----------------------------------------------------------------------------*/
/** \brief Imprime o estado do jogo

Esta função imprime a mão do jogador
@param *path    O URL correspondente à pasta que contém todas as cartas
@param e        O estado atual do jogo
*/
void imprime (const char *path, const ESTADO *e)
{
    int j;                      /* jogador */
    int xc = XC_INIT;           /* x inicial */
    int yc = YC_INIT;           /* y inicial */
    int yj = 0;                 /* tabuleiros dos jogadores */
    int i;
    MAO mao = e->mao[0];

    /* imprime a mao do jogador e os botoes */
    for (i = 0; mao > 0; mao >>= 1, i++)
        if (mao & (MAO) 1) {
            imprime_carta(path, xc, yc, *e, i);
        }
    imprime_bjogar(*e);
    imprime_blimpar(*e);

    /* imprime o numero de cartas dos bots */
    for (j = 1; j < 4; yj += YC_STEP, j++)
        printf(
            "\t\t<P>Jogador %d: %d cartas</P>\n",
            j+1, e->ncartas[j]
        );
    imprime_ult_jogada(path, e);
}

/*----------------------------------------------------------------------------*/
void bot_joga (ESTADO *e)
{
    if (e->jogador == e->ult_jogador_valido) {                  /* pode jogar qq coisa */
        unsigned int idx = trailingZ(e->mao[e->jogador]);       /* indice da carta mais pequena */
        e->mao[e->jogador] = REM_SELECCAO(e->mao[e->jogador], (MAO) 1 << idx);
        parse(estado2str(e));
    } else {    /* tem de jogar de acordo com a ultima jogada valida */
        printf("fazer qq merda aqui\n");
    }
}

/*----------------------------------------------------------------------------*/
/** \brief Trata os argumentos da CGI

Esta função recebe a query que é passada à cgi-bin e trata-a.
A query contém o estado do jogo.
Cada carta corresponde a um bit que está a 1 se essa carta pertence ao conjunto, e a 0 caso contrário.
Caso não seja passado nada à cgi-bin, ela assume que o jogo esta ainda para começar.
@param query    A query que é passada à cgi-bin
*/
void parse (char *query)
{
    ESTADO e;
    if ((query != NULL) && (strlen(query) != 0))
        e = str2estado(query);
    else
        initEstado(&e);

    if (e.jogador == 0)
        imprime(BARALHO, &e);
    else
        bot_joga(&e);
}

/*----------------------------------------------------------------------------*/
/** \brief Função principal

Função principal do programa que imprime os cabeçalhos necessários e depois disso invoca
a função que vai imprimir o código html para desenhar as cartas
*/
int main (void)
{
    srandom(time(NULL));
    printf(     /* Cabeçalhos necessários numa CGI */
        "data: text/html, charset=utf-8\n\n"
        "<!DOCTYPE HTML>\n"
        "<HTML>\n"
        "<HEAD>\n"
        "\t<TITLE>Big Two</TITLE>\n"
        "\t<META CHARSET=\"utf-8\"/>\n"
        "</HEAD>\n"
        "<BODY BGCOLOR=\"#%s\">\n",
        COR_TABULEIRO
    );
    /*
     * Ler os valores passados à cgi que estão na 
     * variável ambiente e passá-los ao programa
     */
    parse(getenv("QUERY_STRING"));
    printf(
        "</BODY>\n"
        "</HTML>"
    );
    return 0;
}
