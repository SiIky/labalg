#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* URL da CGI */
#define SCRIPT          "http://127.0.0.1/cgi-bin/cartas"

/* URL da pasta com as cartas */
#define BARALHO         "http://127.0.0.1/cards"

/* Ordem dos naipes */
#define NAIPES          "DCHS"

/* Ordem das cartas */
#define VALORES         "3456789TJQKA2"

/* valores usados pela funcao imprime */
#define COR_TABULEIRO 116611    /* RGB em HEX */
#define XC_INIT 10              /* x inicial pra cartas */
#define YC_INIT 10              /* y inicial pra cartas */
#define XC_STEP 20              /* salto do x pra cartas */
#define YC_STEP 150             /* salto do y pra cartas */
#define YJ_INIT 0               /* y inicial pra jogador */
#define YJ_STEP 150             /* salto do y pra jogador */

/**
Estado inicial com todas as 52 cartas do baralho
Cada carta é representada por um bit que está
a 1 caso ela pertença à mão ou 0 caso contrário
*/
const unsigned long long int ESTADO_INICIAL = 0xfffffffffffff;

/** \brief Devolve o índice da carta

@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         O índice correspondente à carta
*/
int indice (const int naipe, const int valor)
{
    return naipe * 13 + valor;
}

/** \brief Adiciona uma carta ao estado

@param ESTADO   O estado atual
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         O novo estado
*/
unsigned long long int add_carta (unsigned long long int ESTADO, const int naipe, const int valor)
{
    int idx = indice(naipe, valor);
    return ESTADO | ((unsigned long long int) 1 << idx);
}

/** \brief Remove uma carta do estado

@param ESTADO   O estado atual
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         O novo estado
*/
unsigned long long int rem_carta (unsigned long long int ESTADO, const int naipe, const int valor)
{
    int idx = indice(naipe, valor);
    return ESTADO & ~((unsigned long long int) 1 << idx);
}

/** \brief Verifica se uma carta pertence ao estado

@param ESTADO   O estado atual
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@return         1 se a carta existe e 0 caso contrário
*/
int carta_existe (const unsigned long long int ESTADO, const int naipe, const int valor)
{
    int idx = indice(naipe, valor);
    unsigned long long int estado = ESTADO;
    return (estado >> idx) & 1;
}

/** \brief Imprime o html correspondente a uma carta

@param path     O URL correspondente à pasta que contém todas as cartas
@param x        A coordenada x da carta
@param y        A coordenada y da carta
@param mao[]    O estado atual
@param naipe    O naipe da carta (inteiro entre 0 e 3)
@param valor    O valor da carta (inteiro entre 0 e 12)
@param j        O jogador a que a carta pertence
*/
void imprime_carta (char *path, const int x, const int y, const unsigned long long int mao[], const int naipe, const int valor, const int j, const unsigned long long int ult_jogada[], const int ncartas[], const int ult_jogador)
{
    char script[10240];
    unsigned long long int tmp_mao[4];
    int i;
    for (i = 0; i < 4; i++)
        tmp_mao[i] = mao[i];
    tmp_mao[j] = rem_carta(mao[j], naipe, valor);
    sprintf(script, "%s?q=%llu+%llu+%d_%llu+%llu+%d_%llu+%llu+%d_%llu+%llu+%d_%d", SCRIPT, tmp_mao[0], ult_jogada[0], ncartas[0], tmp_mao[1], ult_jogada[1], ncartas[1], tmp_mao[2], ult_jogada[2], ncartas[2], tmp_mao[3], ult_jogada[3], ncartas[3], ult_jogador);
    printf("<a xlink:href = \"%s\"><image x = \"%d\" y = \"%d\" height = \"110\" width = \"80\" xlink:href = \"%s/%c%c.svg\" /></a>\n", script, x, y, path, VALORES[valor], NAIPES[naipe]);
}

/** \brief Imprime o estado

Esta função está a imprimir o estado em quatro linhas: uma para cada jogador
@param path     O URL correspondente à pasta que contém todas as cartas
@param mao[]    O estado atual
*/
void imprime (char *path, const unsigned long long int mao[], const unsigned long long int ult_jogada[], const int ult_jogador, const int ncartas[])
{
    int n;              /* naipe */
    int v;              /* valor */
    int j;              /* jogador */
    int xc = XC_INIT;   /* x inicial */
    int yc = YC_INIT;   /* y inicial */
    int yj = 0;         /* tabuleiros dos jogadores */

    printf("<svg height = \"800\" width = \"800\">\n");

    for (j = 0; j < 4; yj += YC_STEP, yc += YC_STEP, j++) {
        printf("Jogador %d\n", j);
        printf("<rect x = \"0\" y = \"%d\" height = \"130\" width = \"400\" style = \"fill:#%d\"/>\n", yj, COR_TABULEIRO);
        printf("<svg height = \"800\" width = \"800\">\n");
        for (xc = 10, n = 0; n < 4; n++) {
            for (v = 0; v < 13; v++) {
                if (carta_existe(mao[j], n, v)) {
                    xc += XC_STEP;
                    imprime_carta(path, xc, yc, mao, n, v, j, ult_jogada, ncartas, ult_jogador);
                }
            }
        }
        printf("</svg>\n");
    }
    printf("</svg>\n");
}

void baralhar (unsigned long long int mao[], int ncartas[])
{
    int n;      /* naipe */
    int v;      /* valor */
    int j;      /* jogador */

    for (n = 0; n < 4; n++)
        for (v = 0; v < 13; v++) {
            do {
                j = random() % 4;
            } while (ncartas[j] >= 13);
            mao[j] = add_carta(mao[j], n, v);
            ncartas[j]++;
        }
}

/** \brief Trata os argumentos da CGI

Esta função recebe a query que é passada à cgi-bin e trata-a.
Neste momento, a query contém o estado que é um inteiro que representa um conjunto de cartas.
Cada carta corresponde a um bit que está a 1 se essa carta está no conjunto e a 0 caso contrário.
Caso não seja passado nada à cgi-bin, ela assume que todas as cartas estão presentes.
@param query A query que é passada à cgi-bin
*/
void parse (char *query)
{
    unsigned long long int mao[4] = {0};                /* comecam todas vazias */
    unsigned long long int ult_jogada[4] = {0};         /* comecam todas vazias */
    int ult_jogador = -1;                               /* ultimo jogador */
    int ncartas[4] = {0};                               /* jogadores comecam com 0 cartas */

    if (sscanf(query, "q=%llu+%llu+%d_%llu+%llu+%d_%llu+%llu+%d_%llu+%llu+%d_%d", &mao[0], &ult_jogada[0], &ncartas[0], &mao[1], &ult_jogada[1], &ncartas[1], &mao[2], &ult_jogada[2], &ncartas[2], &mao[3], &ult_jogada[3], &ncartas[3], &ult_jogador) == 1) {
        imprime(BARALHO, mao, ult_jogada, ult_jogador, ncartas);
    } else {
        baralhar(mao, ncartas);
        imprime(BARALHO, mao, ult_jogada, ult_jogador, ncartas);
    }
}

/** \brief Função principal

Função principal do programa que imprime os cabeçalhos necessários e depois disso invoca
a função que vai imprimir o código html para desenhar as cartas
*/
int main (void)
{
    /*
    int jactual = (jactual + 1) % 4;
    */
    srandom(time(NULL));
    /* Cabeçalhos necessários numa CGI */
    printf("Content-Type: text/html; charset=utf-8\n\n");
    printf("<header><title>Big Two</title></header>\n");
    printf("<body>\n");
    printf("<h1>Big Two</h1>\n");
    /* if (jactual == 0) { */ /* vez do jogador */
    /* Ler os valores passados à cgi que estão na variável ambiente e passá-los ao programa */
    parse(getenv("QUERY_STRING"));
    printf("</body>\n");
    /* } else {
        faz_jogada();
    } */
    return 0;
}
