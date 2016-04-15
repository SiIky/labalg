#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* =========================================================
 * Definição das Format Strings para printf e etc
 *
 *      jogador[n] := "mao[n]+(ult_jogada[n])+ncartas[n]"
 *
 *      "jogador[0]_jogador[1]_jogador[2]_jogador[3]_(ult_jogador)_(seleccao)_(ult_jogador_valido)"
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
#define ESPADAS         (((MAO) 1) << 3)
#define COPAS           (((MAO) 1) << 2)
#define PAUS            (((MAO) 1) << 1)

/* Ordem das cartas */
#define VALORES         "3456789TJQKA2"
#define TERNOS          (((MAO) 1) << 3)

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

#define INDICE_NAIPE(N, V)      ((V) + ((N) * 13)) /* ordenado por naipe (do stor) */
#define INDICE(N, V)            ((N) + ((V) * 4)) /* ordenado por figuras (nossa) */
#define REM_SELECCAO(E, S)      ((E) & ~(S))

typedef struct card {
    unsigned int naipe, valor;
} CARTA;

typedef unsigned long long int MAO;
typedef struct state {
    MAO mao[4];
    MAO ult_jogada[4];
    MAO seleccao;
    int ncartas[4];
    int ult_jogador;
    int ult_jogador_valido;
} ESTADO;

/* MAO procura_valor (ESTADO e); */
/* MAO procura_naipe (ESTADO e); */

ESTADO str2estado (const char *str)
{
    ESTADO e;
    sscanf(str, "q="
        "%llu+%llu+%d_"
        "%llu+%llu+%d_"
        "%llu+%llu+%d_"
        "%llu+%llu+%d_"
        "%d_%llu_%d",
        &e.mao[0], &e.ult_jogada[0], &e.ncartas[0],
        &e.mao[1], &e.ult_jogada[1], &e.ncartas[1],
        &e.mao[2], &e.ult_jogada[2], &e.ncartas[2],
        &e.mao[3], &e.ult_jogada[3], &e.ncartas[3],
        &e.ult_jogador, &e.seleccao, &e.ult_jogador_valido
    );
    return e;
}

char* estado2str (const ESTADO *e)
{
    static char str[MAXLEN];
    sprintf(str,
        "%llu+%llu+%d_"
        "%llu+%llu+%d_"
        "%llu+%llu+%d_"
        "%llu+%llu+%d_"
        "%d_%llu_%d",
        e->mao[0], e->ult_jogada[0], e->ncartas[0],
        e->mao[1], e->ult_jogada[1], e->ncartas[1],
        e->mao[2], e->ult_jogada[2], e->ncartas[2],
        e->mao[3], e->ult_jogada[3], e->ncartas[3],
        e->ult_jogador, e->seleccao, e->ult_jogador_valido
    );
    return str;
}

/*----------------------------------------------------------------------------*/
CARTA mao2carta (MAO carta)
{
    CARTA c;
    c.naipe = 0;
    for (c.valor = 0; carta > TERNOS; c.valor++)
        carta >>= 4;

    switch (carta) {
        case ESPADAS:
            c.naipe = 3;
            break;
        case COPAS:
            c.naipe = 2;
            break;
        case PAUS:
            c.naipe = 1;
            break;
        default: /* OUROS */
            c.naipe = 0;
            break;
    }

    return c;
}

/*----------------------------------------------------------------------------*/
CARTA* jogada2cartas (MAO jogada)
{
    static CARTA cartas[6];
    int i, w;
    /* fazer coisas aqui */
    for (i = w = 0; jogada > 0; jogada >>= 1, i++)
        if (jogada % 2)
            cartas[w++] = mao2carta((MAO) 1 << i);

    return cartas;
}

/*----------------------------------------------------------------------------*/
int valores_iguais (CARTA cartas[], int N)
{
    int i, res;
    for (i = 1, res = 0; (i < N && (res = cartas[i].valor == cartas[i-1].valor)); i++);
    return res;
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

/* ====================================================== */
/*
int jogada_single (const ESTADO *e)
{
    int r = 0;
    int b = bitsUm(e->seleccao);
    r = (b == 1) && (e->seleccao > e->ult_jogada[e->ult_jogador_valido]);
    return r;
}

int jogada_pair (const ESTADO *e)
{
    int r = 0;
    int b = bitsUm(e->seleccao);
    if (b == 2) {
        int ns = carta_naipe(e->seleccao);
        int nu = carta_naipe(e->seleccao);
        if (ns == nu)
            // placeholder
        else if (ns > nu)
            r = 1;
    }
    return r;
}
*/
/* ====================================================== */

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
    CARTA* cartas = jogada2cartas(jogada);
    CARTA* ult_cartas = jogada2cartas(ult_jogada);

    if (ult_jogada == (MAO) 0) {
        switch (bits) {
            case PLAY_SINGLE:
                res = 1;
                break;
            case PLAY_PAIR:
            case PLAY_TRIPLE:
                /* ha algum erro aqui */
                res = valores_iguais(cartas, bits+1);
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
                /* mudar isto pra funcionar nos casos em que os valores da jogada sao iguais aos da ult_jogada */
                res = (valores_iguais(cartas, bits+1) && (ult_cartas[0].valor < cartas[0].valor || ult_cartas[1].naipe < cartas[1].naipe));
                break;
            case PLAY_TRIPLE:
                res = (valores_iguais(cartas, bits+1) && (ult_cartas[0].valor < cartas[0].valor));
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
int carta_existe (const MAO e, const int naipe, const int valor)
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
    e.ult_jogador %= 4; /* assegura que a primeira jogada decorra direito (por ult_jogador comecar com 7) */

    if (jogada_valida(e.seleccao, e.ult_jogada[e.ult_jogador])) {
        e.ult_jogador = 0;
        e.mao[0] = REM_SELECCAO(e.mao[0], e.seleccao);
        e.ult_jogada[0] = e.seleccao;
        e.seleccao = (MAO) 0;
        sprintf(link, "%s?q=%s", SCRIPT, estado2str(&e));
        printf(
            "\t<svg width=%d height=%d>"
            "<a xlink:href=\"%s\">"
            "<rect x=%d y=%d width=%d height=%d ry=5 style=\"fill:#%s\"/>"
            "<text x=%d y=%d text-anchor=\"midle\" text-alignt=\"center\" font-family=\"serif\" font-weight=\"bold\">Jogar</text></a></svg>\n",
            SVG_WIDTH, SVG_HEIGHT,
            link,
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A,
            TXT_X, TXT_Y
        );
    } else {
        printf(
            "\t<svg width=%d height=%d>"
            "<rect x=%d y=%d width=%d height=%d ry=5 style=\"fill:#%s\"/>"
            "<text x=%d y=%d text-anchor=\"midle\" text-alignt=\"center\" font-family=\"serif\" font-weight=\"bold\">Jogar</text></svg>\n",
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
        "\t<svg width=%d height=%d>",
        SVG_WIDTH, SVG_HEIGHT
    );

    if (e.seleccao != 0) {
        e.seleccao = 0;
        sprintf(link, "%s?q=%s", SCRIPT, estado2str(&e));
        printf(
            "<a xlink:href = \"%s\">"
            "<rect x=%d y=%d width=%d height=%d ry=5 style=\"fill:#%s\"/>"
            "<text x=%d y=%d text-anchor=\"midle\" text-alignt=\"center\" font-family=\"serif\" font-weight=\"bold\">Limpar</text></a></svg>\n",
            link,
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A,
            TXT_X, TXT_Y
        );
    } else {
        printf(
            "<rect x=%d y=%d width=%d height=%d ry=5 style=\"fill:#%s\"/>"
            "<text x=%d y=%d text-anchor=\"midle\" text-alignt=\"center\" font-family=\"serif\" font-weight=\"bold\">Limpar</text></svg>\n",
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D,
            TXT_X, TXT_Y
        );
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
    } else {
        e.seleccao = add_carta(e.seleccao, naipe, valor);
    }

    sprintf(script, "%s?q=%s", SCRIPT,estado2str(&e));

    printf(
        "\t<a xlink:href=\"%s\">"
        "<image x=\"%d\" y=\"%d\" width=\"80\" height=\"110\" xlink:href=\"%s/%c%c.svg\"/></a>\n",
        script,
        x, y, path, VALORES[valor], NAIPES[naipe]
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
    int n;                      /* naipe */
    int v;                      /* valor */
    int j;                      /* jogador */
    int xc = XC_INIT;           /* x inicial */
    int yc = YC_INIT;           /* y inicial */
    int yj = 0;                 /* tabuleiros dos jogadores */

    printf(
        "<rect x=\"0\" y=\"%d\" width=\"1366\" height=\"768\" style=\"fill:#%s\"/>\n",
        yj, COR_TABULEIRO
    );
    for (xc = XC_INIT, v = 0; v < 13; v++)
        for (n = 0; n < 4; n++)
            if (carta_existe(e->mao[0], n, v)) {
                    xc += XC_STEP;
                    imprime_carta(path, xc, yc, *e, n, v);
            }

    printf("\t<ul>\n");
    for (j = 1; j < 4; yj += YC_STEP, j++)
        printf(
            "\t\t<li>Jogador %d: %d cartas</li>\n",
            j+1, e->ncartas[j]
        );
    printf("\t</ul>\n");
    /*
        printf(
            "\n\t<rect x=%d y=%d width=%d height=%d ry=5 style=\"fill:#%s\"/>"
            "<text x=%d y=%d text-anchor=\"midle\" text-alignt=\"center\" font-family=\"serif\" font-weight=\"bold\">"
            "Jogador %d: %d</text>",
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D,
            TXT_X, TXT_Y,
            j, e->ncartas[j]
        );
    */

    imprime_bjogar(*e);
    imprime_blimpar(*e);
}

/*----------------------------------------------------------------------------*/
/** \brief Dá as cartas a cada jogador no início do jogo

@param e        O estado actual do jogo
@return e       O novo estado do jogo
*/
ESTADO* baralhar (void)
{
    ESTADO *e = (ESTADO*) malloc(sizeof(ESTADO));   /* estado do jogo */
    int n;      /* naipe */
    int v;      /* valor */
    int j;      /* jogador */
    int i;

    e->seleccao = 0;                     /* cartas selecionadas pelo jogador */
    e->ult_jogador = -1;                 /* último jogador */
    e->ult_jogador_valido = -1;          /* último jogador a jogar uma jogada valida */
    for (i = 0; i < 4; i++) {
        e->mao[i] = 0;                   /* começam todas vazias */
        e->ult_jogada[i] = 0;            /* começam todas vazias */
        e->ncartas[i] = 0;               /* jogadores começam com 0 cartas */
    }

    for (n = 0; n < 4; n++)
        for (v = 0; v < 13; v++) {
            do j = random() % 4; while (e->ncartas[j] >= 13);
            e->mao[j] = add_carta(e->mao[j], n, v);
            e->ncartas[j]++;
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
    /* ESTADO e; */
    if ((query != NULL) && (strlen(query) != 0)) {
        ESTADO e = str2estado(query);
        /* (e.ult_jogador == 3) ? { */
        imprime(BARALHO, &e);
            /* } : { bot_joga; }; */
    } else {
        /* e = baralhar(); */
        imprime(BARALHO, baralhar());
    }
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
    printf(
        "Content-Type: text/html; charset=utf-8\n\n"
        "<head><title>Big Two</title></head>\n"
        "<body>\n"
        "<h1>Big Two</h1>\n"
        "<svg width=\"1366\" height=\"768\">"
    );

    /* Ler os valores passados à cgi que estão na variável ambiente e passá-los ao programa */
    parse(getenv("QUERY_STRING"));
    printf(
        "</svg>\n"
        "</body>\n"
    );

    return 0;
}
