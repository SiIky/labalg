#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structs.h"
#include "bigtwo.h"
#include "bot.h"
#include "ui.h"

/*==================================================================*/
int valores_iguais (Card *c, int N)
{
    int r;
    for (r = 1; N > 0 && r != 0; N--)
        r = c[N].valor == c[N-1].valor;
    return r;
}

/*==================================================================*/
/** \brief Verifica se uma jogada é válida

@param jogada           As cartas selecionadas
@param ult_jogada       A última jogada do último jogador
@return                 Devolve 1 se for válida, 0 caso contrário
*/
int jogada_valida (const State *e)
{
    unsigned int nb = bitsUm(e->seleccao);
    unsigned int ub = bitsUm(e->seleccao);

    if (nb == ub && (!(carta_existe(e->mao[0], (MAO) 0)) || carta_existe(e->seleccao, (MAO) 0)))
        switch (nb) {
            case PLAY_SINGLE:
                return test_play1(e);
            case PLAY_PAIR:
                return test_play2(e);
            case PLAY_TRIPLE:
                return test_play3(e);
            case PLAY_FIVE:
                return test_play5(e);
                /*
                int combo_sel = tipodecombo(&(e->seleccao));
                int combo_ult = tipodecombo(&(e->ult_jogada[e->ult_jogador_valido]));
                if (combo_sel > combo_ult)
                     return 1;
                else if (combo_sel == combo_ult)
                    compara(seleccao, sel_combo, ult_jogada); // compara com a ult_jogada
                else
                    return 0;
                */
        }
    return 0;
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

    if (query != NULL && strlen(query) > 0)
        e = str2estado(query);
    else
        initEstado(&e);

    e.decorrer = update_decorrer(&e);

    for (; e.jogador != 0 && e.decorrer == 1;) {
        bot_joga(&e);
        e.decorrer = update_decorrer(&e);
    }

    /*if (e.decorrer == 0) {
        game_over();
    } else {*/
    imprime(&e);
    /*}*/
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
