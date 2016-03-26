#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* =========================================================
 * Definição das Format Strings para printf e etc
 *
 *      jogador[n] := "mao[n]+(ult_jogada[n])+ncartas[n]"
 *
 *      "jogador[0]_jogador[1]_jogador[2]_jogador[3]_(ult_jogador)_(seleccao)"
 *
 * =========================================================
 * Última jogada:
 *
 *   == 0 -> PASSOU
 *   != 0 -> JOGOU
 *
 * =========================================================
*/

/* URL da CGI */
#define SCRIPT          "http://127.0.0.1/cgi-bin/cartas"

/* URL da pasta com as cartas */
#define BARALHO         "http://127.0.0.1/cards"

/* Ordem dos naipes */
#define NAIPES          "DCHS"
#define ESPADAS         (((MAO) 1) << 38)
#define COPAS           (((MAO) 1) << 25)
#define PAUS            (((MAO) 1) << 12)

/* Ordem das cartas */
#define VALORES         "3456789TJQKA2"

/* valores usados pela função imprime */
#define COR_TABULEIRO   "116611"    /* RGB em HEX */
#define XC_INIT         10          /* x inicial para cartas */
#define YC_INIT         10          /* y inicial para cartas */
#define XC_STEP         20          /* salto do x para cartas */
#define YC_STEP         150         /* salto do y para cartas */
#define YC_SEL_STEP     10          /* salto de cartas selecionadas */
#define YJ_INIT         0           /* y inicial para jogador */
#define YJ_STEP         150         /* salto do y para jogador */

/* definições do botão jogar */
#define SVG_WIDTH       150
#define SVG_HEIGHT      200
#define COR_BOT_A       "C99660"        /* cor dos botões activados */
#define COR_BOT_D       "999999"        /* cor dos botões não activados */
#define RECT_X          50
#define RECT_Y          50
#define RECT_WIDTH      100
#define RECT_HEIGHT     50
#define TXT_X           100
#define TXT_Y           80

/* comprimento máximo das strings */
#define MAXLEN          10240

#define PLAY_SINGLE     1
#define PLAY_PAIR       2
#define PLAY_TRIPLE     3
#define PLAY_FIVE       5

#define INDICE(N, V)            ((N) + ((V) * 4))
#define REM_SELECCAO(E, S)      ((E) & ~(S))

typedef unsigned long long int MAO;
typedef struct state ESTADO;
struct state {
    MAO mao[4];
    MAO ult_jogada[4];
    MAO seleccao;
    unsigned int ncartas[4];
    unsigned int ult_jogador;
};

ESTADO str2estado (const char *str)
{
    ESTADO e;
    sscanf(str, "q="
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%u_%llu",
        &e.mao[0], &e.ult_jogada[0], &e.ncartas[0],
        &e.mao[1], &e.ult_jogada[1], &e.ncartas[1],
        &e.mao[2], &e.ult_jogada[2], &e.ncartas[2],
        &e.mao[3], &e.ult_jogada[3], &e.ncartas[3],
        &e.ult_jogador, &e.seleccao
    );
    return e;
}

char *estado2str (const ESTADO e)
{
    static char str[MAXLEN];
    sprintf(str,
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%u_%llu",
        e.mao[0], e.ult_jogada[0], e.ncartas[0],
        e.mao[1], e.ult_jogada[1], e.ncartas[1],
        e.mao[2], e.ult_jogada[2], e.ncartas[2],
        e.mao[3], e.ult_jogada[3], e.ncartas[3],
        e.ult_jogador, e.seleccao
    );
    return str;
}

/*----------------------------------------------------------------------------*/
unsigned int carta_naipe (const MAO carta)
{
    if (carta > ESPADAS)
        return 3;
    else if (carta > COPAS)
        return 2;
    else if (carta > PAUS)
        return 1;
    else        /* OUROS */
        return 0;
}

/*----------------------------------------------------------------------------*/
unsigned int carta_valor (MAO carta, const unsigned int naipe)
{
    unsigned int i;
    carta >>= (13 * naipe);
    for (i = 0; carta % 2 == 0; carta >>= 1, i++);
    return i;
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
int jogada_valida (const MAO jogada, const MAO ult_jogada)
{
    int res = 0;
    unsigned int bits = bitsUm(jogada);
    unsigned int ult_bits = bitsUm(ult_jogada);

    if (ult_jogada == 0) {
        switch (bits) {
            case PLAY_SINGLE:
                res = 1;
                break;
            case PLAY_PAIR:
                /* 2 cartas de valores iguais */
                break;
            case PLAY_TRIPLE:
                /* 3 cartas de valores iguais */
                break;
            case PLAY_FIVE:
                res = 1;
                break;
            default:
                res = 0;
                break;
        }
    } else if (bits == ult_bits) {
        switch (bits) {
            case PLAY_SINGLE:
                res = (jogada > ult_jogada);
                break;
            case PLAY_PAIR:
                /* cartas de valores iguais */
                break;
            case PLAY_TRIPLE:
                /* cartas de valores iguais */
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
MAO add_carta (const MAO e, const int naipe, const int valor)
{
    unsigned int idx = INDICE(naipe, valor);
    return (e | ((MAO) 1 << idx));
}

/*----------------------------------------------------------------------------*/
/** \brief Remove uma carta do estado

@param e        Uma mão
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         A nova mão
*/
MAO rem_carta (const MAO e, const int naipe, const int valor)
{
    unsigned int idx = INDICE(naipe, valor);
    return (e & ~((MAO) 1 << idx));
}

/*----------------------------------------------------------------------------*/
/** \brief Verifica se uma carta pertence ao estado

@param e        Uma mão
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         1 se a carta existe, 0 caso contrário
*/
int carta_existe (MAO e, const int naipe, const int valor)
{
    unsigned int idx = INDICE(naipe, valor);
    return ((e >> idx) & 1);
}

/*----------------------------------------------------------------------------*/
/** \brief Imprime o botão Jogar

@param e        O estado actual do jogo
*/
void imprime_bjogar (ESTADO e)
{
    char link[MAXLEN];
    unsigned int uj = e.ult_jogador % 4; /* assegura que a primeira jogada decorra direito (por ult_jogador comecar com 7) */

    if (jogada_valida(e.seleccao, e.ult_jogada[uj])) {
        e.ult_jogador = 0;
        e.mao[0] = REM_SELECCAO(e.mao[0], e.seleccao);
        e.ult_jogada[0] = e.seleccao;
        e.seleccao = (MAO) 0;

        sprintf(link, "%s?q=%s", SCRIPT, estado2str(e));
        printf("<svg width=%d height=%d>\n", SVG_WIDTH, SVG_HEIGHT);
        printf("<a xlink:href = \"%s\">\n", link);
        printf("<rect x=%d y=%d width=%d height=%d ry=5 style=\"fill:#%s\" />\n", RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A);
        printf("<text x=%d y=%d text-anchor = \"midle\" text-alignt = \"center\" font-family = \"serif\" font-weight = \"bold\">Jogar</text></a></svg>\n", TXT_X, TXT_Y);
    } else {
        printf("<svg width=%d height=%d>\n", SVG_WIDTH, SVG_HEIGHT);
        printf("<rect x=%d y=%d width=%d height=%d ry=5 style=\"fill:#%s\" />\n", RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D);
        printf("<text x=%d y=%d text-anchor = \"midle\" text-alignt = \"center\" font-family = \"serif\" font-weight = \"bold\">Jogar</text></svg>\n", TXT_X, TXT_Y);
    }
}

/*----------------------------------------------------------------------------*/
/** \brief Imprime o botão Limpar

@param e        O estado actual do jogo
*/
void imprime_blimpar (ESTADO e)
{
    char link[MAXLEN];
    if (e.seleccao != 0) {
        e.seleccao = 0;
        sprintf(link, "%s?q=%s", SCRIPT, estado2str(e));
        printf("<svg width=%d height=%d>\n", SVG_WIDTH, SVG_HEIGHT);
        printf("<a xlink:href = \"%s\">\n", link);
        printf("<rect x=%d y=%d width=%d height=%d ry=5 style=\"fill:#%s\" />\n", RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A);
        printf("<text x=%d y=%d text-anchor = \"midle\" text-alignt = \"center\" font-family = \"serif\" font-weight = \"bold\">Limpar</text></a></svg>\n", TXT_X, TXT_Y);
    } else {
        printf("<svg width=%d height=%d>\n", SVG_WIDTH, SVG_HEIGHT);
        printf("<rect x=%d y=%d width=%d height=%d ry=5 style=\"fill:#%s\" />\n", RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D);
        printf("<text x=%d y=%d text-anchor = \"midle\" text-alignt = \"center\" font-family = \"serif\" font-weight = \"bold\">Limpar</text></svg>\n", TXT_X, TXT_Y);
    }
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
void imprime_carta (const char *path, const int x, int y, ESTADO e, const int naipe, const int valor)
{
    char script[MAXLEN];
    if (carta_existe(e.seleccao, naipe, valor)) {
        y -= YC_SEL_STEP;
        e.seleccao = rem_carta(e.seleccao, naipe, valor);
        sprintf(script, "%s?q=%s", SCRIPT, estado2str(e));
    } else {
        e.seleccao = add_carta(e.seleccao, naipe, valor);
        sprintf(script, "%s?q=%s", SCRIPT,estado2str(e));
    }
    printf("<a xlink:href=\"%s\"><image x=\"%d\" y=\"%d\" height=\"110\" width=\"80\" xlink:href=\"%s/%c%c.svg\"/></a>\n", script, x, y, path, VALORES[valor], NAIPES[naipe]);
}

/*----------------------------------------------------------------------------*/
/** \brief Imprime o estado do jogo

Esta função imprime a mão do jogador
@param *path    O URL correspondente à pasta que contém todas as cartas
@param e        O estado atual do jogo
*/
void imprime (const char *path, const ESTADO e)
{
    int n;                      /* naipe */
    int v;                      /* valor */
    /* int j; */                /* jogador */
    int xc = XC_INIT;           /* x inicial */
    int yc = YC_INIT;           /* y inicial */
    int yj = 0;                 /* tabuleiros dos jogadores */

    printf("<svg height = \"200\" width = \"400\">\n");
    /* for (j = 0; j < 4; yj += YC_STEP, yc += YC_STEP, j++) { */
    /* printf("Jogador %d\n", j); */
    printf("<rect x=\"0\" y=\"%d\" height=\"130\" width=\"400\" style=\"fill:#%s\"/>\n", yj, COR_TABULEIRO);
    printf("<svg height = \"800\" width = \"800\">\n");

    for (xc = XC_INIT, v = 0; v < 13; v++)
        for (n = 0; n < 4; n++)
            if (carta_existe(e.mao[0], n, v)) {
                    xc += XC_STEP;
                    imprime_carta(path, xc, yc, e, n, v);
            }

    printf("</svg>\n");
    /* } */
    printf("</svg>\n");
    imprime_bjogar(e);
    imprime_blimpar(e);
}

/*----------------------------------------------------------------------------*/
/** \brief Dá as cartas a cada jogador no início do jogo

@param e        O estado actual do jogo
@return e       O novo estado do jogo
*/
ESTADO baralhar (void)
{
    ESTADO e;   /* estado do jogo */
    int n;      /* naipe */
    int v;      /* valor */
    int j;      /* jogador */
    int i;

    e.seleccao = 0;                     /* cartas selecionadas pelo jogador */
    e.ult_jogador = 7;                  /* último jogador */
    for (i = 0; i < 4; i++) {
        e.mao[i] = 0;                   /* começam todas vazias */
        e.ult_jogada[i] = 0;            /* começam todas vazias */
        e.ncartas[i] = 0;               /* jogadores começam com 0 cartas */
    }

    for (n = 0; n < 4; n++)
        for (v = 0; v < 13; v++) {
            do j = random() % 4; while (e.ncartas[j] >= 13);
            e.mao[j] = add_carta(e.mao[j], n, v);
            e.ncartas[j]++;
        }
    return e;
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
    if ((query != NULL) && (strlen(query) != 0))
        imprime(BARALHO, str2estado(query));
    else
        imprime(BARALHO, baralhar());
}

/*----------------------------------------------------------------------------*/
/** \brief Função principal

Função principal do programa que imprime os cabeçalhos necessários e depois disso invoca
a função que vai imprimir o código html para desenhar as cartas
*/
int main (void)
{
    srandom(time(NULL));

    /* Cabeçalhos necessários numa CGI */
    printf("Content-Type: text/html; charset=utf-8\n\n");
    printf("<header><title>Big Two</title></header>\n");
    printf("<body>\n");
    printf("<h1>Big Two</h1>\n");

    /* Ler os valores passados à cgi que estão na variável ambiente e passá-los ao programa */
    parse(getenv("QUERY_STRING"));
    printf("</body>\n");

    return 0;
}
