#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structs.h"
#include "cartas.h"

/*==================================================================*/
int valores_iguais (Card cartas[])
{
    int i, res;
    for (i = 1, res = 0; (cartas[i].valor < 13 && cartas[i].naipe < 4 && (res = (cartas[i].valor == cartas[i-1].valor))); i++);
    return (i == 1) ? 1 : res;
}

/*==================================================================*/
unsigned int trailingZ (MAO n)
{
    unsigned int count;
    for (count = 0; n > 0 && (n ^ 1); n >>= 1, count++);
    return count;
}

/*==================================================================*/
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

/*==================================================================*/
/** \brief Verifica se uma jogada é válida

@param jogada           As cartas selecionadas
@param ult_jogada       A última jogada do último jogador
@return                 Devolve 1 se for válida, 0 caso contrário
*/
int jogada_valida (const State *e)
{
    int res = 0;
    MAO jogada = e->seleccao;
    MAO ult_jogada = e->ult_jogada[(e->ult_jogador_valido + 3) % 4];
    Card* cartas = jogada2cartas(jogada);
    Card* ult_cartas = jogada2cartas(ult_jogada);
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

/*==================================================================*/
/** \brief Imprime o botão Jogar

@param e        O estado actual do jogo
*/
void imprime_bjogar (State e)
{
    char link[MAXLEN];

    printf(
        "<SVG WIDTH=%d HEIGHT=%d "
        "STYLE=\"POSITION:ABSOLUTE; TOP:350px; LEFT:200px\">",
        RECT_WIDTH, RECT_HEIGHT
    );
    if (jogada_valida(&e)) {
        e.jogador = (e.jogador + 1) % 4;
        e.ult_jogador_valido = 0;
        e.mao[0] = REM_SELECCAO(e.mao[0], e.seleccao);
        e.ult_jogada[0] = e.seleccao;
        e.seleccao = (MAO) 0;
        sprintf(link, "%s?q=%s", SCRIPT, estado2str(&e));
        printf(
            "<A XLINK:HREF=\"%s\">"
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"FILL:%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Jogar</TEXT></A></SVG>\n",
            link,
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A,
            TXT_X, TXT_Y
        );
    } else {
        printf(
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Jogar</TEXT></SVG>\n",
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D,
            TXT_X, TXT_Y
        );
    }
}

/*==================================================================*/
/** \brief Imprime o botão Limpar

@param e        O estado actual do jogo
*/
void imprime_blimpar (State e)
{
    char link[MAXLEN];
    printf(
        "<SVG WIDTH=%d HEIGHT=%d "
        "STYLE=\"POSITION:ABSOLUTE; TOP:350px; LEFT:325px\">",
        RECT_WIDTH, RECT_HEIGHT
    );
    if (e.seleccao != 0) {
        e.seleccao = 0;
        sprintf(link, "%s?q=%s", SCRIPT, estado2str(&e));
        printf(
            "<A XLINK:HREF=\"%s\">"
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Limpar</TEXT></A></SVG>\n",
            link,
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A,
            TXT_X, TXT_Y
        );
    } else {
        printf(
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Limpar</TEXT></SVG>\n",
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D,
            TXT_X, TXT_Y
        );
    }
}

/*==================================================================*/
void imprime_ult_jogada (const State *e)
{
    int j;                      /* jogador */
    int xc;                     /* x inicial */
    int yj;                     /* y inicial */
    int i;                      /* indice da carta a imprimir */
    MAO ult_jogada[4];
    Card c;

    for (i = 0; i < 4; i++)     /* guarda o array ult_jogada */
        ult_jogada[i] = e->ult_jogada[i];

    printf( /* opening SVG tag */
        "<SVG WIDTH=%d HEIGHT=%d "
        "STYLE=\"POSITION:ABSOLUTE; TOP:10px; LEFT:600px\">\n",
        SVG_WIDTH, 4 * SVG_HEIGHT
    );

    for (yj = YJ_INIT, j = 0; j < 4; yj += YJ_STEP, j++)
        for (xc = XUC_INIT, i = 0; i < 52; ult_jogada[j] >>= 1, i++)
            if (ult_jogada[j] & (MAO) 1) {
                c = mao2carta((MAO) 1 << i);
                printf(
                    "<IMAGE X=%d Y=%d WIDTH=80 HEIGHT=110 XLINK:HREF=\"%s/%c%c.svg\"/></A>\n",
                    xc, yj, BARALHO, VALORES[c.valor], NAIPES[c.naipe]
                );
                xc += XC_STEP;
            }
    printf("</SVG>\n");
}

/*==================================================================*/
/** \brief Imprime o html correspondente a uma carta

@param x        A coordenada x da carta
@param y        A coordenada y da carta
@param e        O estado actual do jogo
@param idx      O valor da carta (inteiro entre 0 e 51)
*/
void imprime_carta (const int x, int y, State e, const unsigned int idx)
{
    if (CARTA_EXISTE(e.seleccao, idx)) {
        y -= YC_SEL_STEP;
        e.seleccao = rem_carta(&e.seleccao, idx);
    } else {
        e.seleccao = add_carta(&e.seleccao, idx);
    }

    Card c = mao2carta((MAO) 1 << idx);
    char script[MAXLEN];
    sprintf(script, "%s?q=%s", SCRIPT, estado2str(&e));
    printf(
        "\t<A XLINK:HREF=\"%s\">"
        "<IMAGE X=%d Y=%d WIDTH=80 HEIGHT=110 XLINK:HREF=\"%s/%c%c.svg\"/></A>\n",
        script,
        x, y, BARALHO, VALORES[c.valor], NAIPES[c.naipe]
    );
}

/*==================================================================*/
/** \brief Imprime o estado do jogo

Esta função imprime a mão do jogador
@param e        O estado atual do jogo
*/
void imprime (const State *e)
{
    int j;                      /* jogador */
    int xc = XC_INIT;           /* x inicial */
    int yc = YC_INIT;           /* y inicial */
    int yj = YJ_INIT;           /* tabuleiros dos jogadores */
    int i;
    MAO mao = e->mao[0];

    /* -------------- bloco das cartas ------------------ */
    /* -------------------------------------------------- */
    printf( /* opening SVG tag */
        "<SVG WIDTH=440 HEIGHT=120 "
        "STYLE=\"POSITION:ABSOLUTE; TOP:200px; LEFT:100px\">\n"
    );
    /* imprime a mao do jogador */
    for (i = 0; mao > 0; mao >>= 1, i++)
        if (mao & (MAO) 1) {
            imprime_carta(xc, yc, *e, i);
            xc += XC_STEP;
        }
    printf("</SVG>\n");         /* closing SVG tag */
    /* -------------------------------------------------- */

    /* imprime a tabela de info sobre o jogo */
    printf(
        "<TABLE BORDER=\"4px\" BORDERCOLOR=\"BLACK\" "
        "STYLE=\"BACKGROUND-COLOR: YELLOW\">\n"
        "<TR>"
        "<TH>Jogador</TH><TH># de cartas</TH><TH>Pontos</TH>"
        "</TR>\n"
    );
    for (j = 0; j < 4; yj += YJ_STEP, j++)
        printf(
            "<TR><TD>%d</TD><TD>%d</TD><TD>0</TD></TR>\n",
            j+1, e->ncartas[j]
        );
    printf("</TABLE>\n");
    imprime_ult_jogada(e);
    imprime_bjogar(*e);
    imprime_blimpar(*e);
}

/*==================================================================*/
void bot_joga (State *e)
{
    if (e->jogador == e->ult_jogador_valido) {                  /* pode jogar qq coisa */
        unsigned int idx = trailingZ(e->mao[e->jogador]);       /* indice da carta mais pequena */
        e->mao[e->jogador] = REM_SELECCAO(e->mao[e->jogador], (MAO) 1 << idx);
    } else {    /* tem de jogar de acordo com a ultima jogada valida */
        /* printf("fazer qq merda aqui\n"); */
        e->jogador = (e->jogador + 3) % 4;
    }
}

/*==================================================================*/
/** \brief Trata os argumentos da CGI

Esta função recebe a query que é passada à cgi-bin e trata-a.
A query contém o estado do jogo.
Cada carta corresponde a um bit que está a 1 se essa carta pertence ao conjunto, e a 0 caso contrário.
Caso não seja passado nada à cgi-bin, ela assume que o jogo esta ainda para começar.
@param query    A query que é passada à cgi-bin
*/
void parse (char *query)
{
    State e;
    if ((query != NULL) && (strlen(query) != 0))
        e = str2estado(query);
    else
        initEstado(&e);

    for (; e.jogador != 0;)
        bot_joga(&e);

    imprime(&e);
}

/*==================================================================*/
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
        "<BODY BGCOLOR=\"%s\">\n",
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
