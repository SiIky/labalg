#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
URL da CGI
*/
#define SCRIPT		"http://127.0.0.1/cgi-bin/cartas"
/**
URL da pasta com as cartas
*/
#define BARALHO		"http://127.0.0.1/cards"

/**
Ordem dos naipes
*/
#define NAIPES		"DCHS"
/**
Ordem das cartas
*/
#define VALORES		"3456789TJQKA2"

#define TRUE 1
#define FALSE 0

/**
Estado inicial com todas as 52 cartas do baralho
Cada carta é representada por um bit que está
a 1 caso ela pertença à mão ou 0 caso contrário
*/
const long long int ESTADO_INICIAL = 0xfffffffffffff;

/** \brief Devolve o índice da carta

@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
@return		O índice correspondente à carta
*/
int indice (int naipe, int valor)
{
    return naipe * 13 + valor;
}

/** \brief Adiciona uma carta ao estado

@param ESTADO	O estado atual
@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
@return		O novo estado
*/
long long int add_carta (long long int ESTADO, int naipe, int valor)
{
    int idx = indice(naipe, valor);
    return ESTADO | ((long long int) 1 << idx);
}

/** \brief Remove uma carta do estado

@param ESTADO	O estado atual
@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
@return		O novo estado
*/
long long int rem_carta (long long int ESTADO, int naipe, int valor)
{
    int idx = indice(naipe, valor);
    return ESTADO & ~((long long int) 1 << idx);
}

/** \brief Verifica se uma carta pertence ao estado

@param ESTADO	O estado atual
@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
@return		1 se a carta existe e 0 caso contrário
*/
int carta_existe (long long int ESTADO, int naipe, int valor)
{
    int idx = indice(naipe, valor);
    return (ESTADO >> idx) & 1;
}

/** \brief Imprime o html correspondente a uma carta

@param path	o URL correspondente à pasta que contém todas as cartas
@param x A coordenada x da carta
@param y A coordenada y da carta
@param ESTADO	O estado atual
@param naipe	O naipe da carta (inteiro entre 0 e 3)
@param valor	O valor da carta (inteiro entre 0 e 12)
*/
void imprime_carta (char *path, int x, int y, long long int ESTADO, int naipe, int valor)
{
    char *suit = NAIPES;
    char *rank = VALORES;
    char script[10240];
    sprintf(script, "%s?q=%lld", SCRIPT, rem_carta(ESTADO, naipe, valor));
    printf("<a xlink:href = \"%s\"><image x = \"%d\" y = \"%d\" height = \"110\" width = \"80\" xlink:href = \"%s/%c%c.svg\" /></a>\n", script, x, y, path, rank[valor], suit[naipe]);
}

/** \brief Imprime o estado

Esta função está a imprimir o estado em quatro colunas: uma para cada naipe
@param path	o URL correspondente à pasta que contém todas as cartas
@param ESTADO	O estado atual
*/
void imprime (char *path, long long int mao[])
{
    int n; /* naipe palhaco */
    int v; /* valor palhaco */
    int x, y;
    int m; /* maos palhaco */

    printf("<svg height = \"800\" width = \"800\">\n");
    printf("<rect x = \"0\" y = \"0\" height = \"800\" width = \"800\" style = \"fill:#007700\"/>\n");

    for (m = 0; m < 4; m++) {
        for(y = 10, n = 0; n < 4; n++, y += 120) {
            for(x = 10, v = 0; v < 13; v++) {
                if(carta_existe(mao[m], n, v)) {
                    x += 20;
                    imprime_carta(path, x, y, mao[m], n, v);
                }
            }
        }
    }

    printf("</svg>\n");
}

void escolhe_jogador (int naipe, int valor, long long int mao[], int ncartas[])
{
    int j; /* jogador */

    while (TRUE) {
        j = random() % 4;
        if (ncartas[j] < 13) {
            add_carta (mao[j], naipe, valor);
            ++ncartas[j];
            break;
        }
    }
}

void baralhar (long long int mao[])
{
    int n; /* naipe */
    int v; /* valor */
    int ncartas[4]; /* contador de cartas de cada jogador */

    ncartas[0] = ncartas[1] = ncartas[2] = ncartas[3] = 0;

    for (n = 0; n < 4; n++) {
        for (v = 0; v < 13; v++) {
            /*
             * este do-while ta a dar cartas antes de testar
             * se o jogador ja tem ou nao 13 cartas
            do {
                j = random() % 4;
                add_carta (mao[j], n, v);
                ++ncartas[j];
            } while (ncartas[j] < 12);
            */
            escolhe_jogador(n, v, mao, ncartas);
        }
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
    long long int mao[4];

    if (sscanf(query, "q=%lld+%lld+%lld+%lld", &mao[0], &mao[1], &mao[2], &mao[3]) == 1) {
        imprime(BARALHO, mao);
    } else {
        baralhar(mao);
        imprime(BARALHO, mao);
    }
}

/*
void baralhar (long long int mao[])
{
    int n;
    int f;
    int j;
    int ncartas[4];

    for (n = 0; n < 4; n++) {
    for (f = 0; f < 13; f++) {
        do {
            j = random () % 4; } 
        while (ncartas[j] >= 13);
       mao[j] = add_carta (mao[j], n, f);
      ncartas[j]++; }
}
}
*/

/** \brief Função principal

Função principal do programa que imprime os cabeçalhos necessários e depois disso invoca
a função que vai imprimir o código html para desenhar as cartas
*/
int main ()
{
/*
 * Cabeçalhos necessários numa CGI
 */
    printf("Content-Type: text/html; charset=utf-8\n\n");
    printf("<header><title>Exemplo</title></header>\n");
    printf("<body>\n");

    printf("<h1>Exemplo de utilização</h1>\n");

/*
 * Ler os valores passados à cgi que estão na variável ambiente e passá-los ao programa
 */
    parse(getenv("QUERY_STRING"));

    printf("</body>\n");
    return 0;
}
