#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structs.h"
#include "cartas.h"
#include "ui.h"

/*==================================================================*/
int valores_iguais (Card *cartas)
{
    int i, res;
    for (i = 1, res = 0; (cartas[i].valor < 13 && cartas[i].naipe < 4 && (res = (cartas[i].valor == cartas[i-1].valor))); i++);
    return (i == 1) ? 1 : res;
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

    if (!(carta_existe(e->mao[0], (MAO) 0)) || carta_existe(e->seleccao, (MAO) 0))
        switch (nb) {
            case PLAY_SINGLE:
                return test_play1(e);
                break;
            case PLAY_PAIR:
                return test_play2(e);
                break;
            case PLAY_TRIPLE:
                return test_play3(e);
                break;
            case PLAY_FIVE:
                /* return test_play5(e); */
                /*
                   int sel_combo = tipoecombo(&(e->seleccao));
                   if (sel_combo > 0)
                       compara(seleccao, sel_combo, ult_jogada); // compara com a ult_jogada
                   else
                       return 0;
                 */
                return 1;
                break;
            default:
                return 0;
                break;
        }
    else
        return 0;
}

/*==================================================================*/
void bot_joga (State *e)
{
    /*
     for (i = 5; a_jogar == 0 && i > 0; i--) {
        a_jogar = BOT_PROCURA_JOGADA(i);
     */
    bot_play1(e);

    if (e->ult_jogada[e->jogador] != 0) {
        e->ult_jogador_valido = e->jogador;
        e->ncartas[e->jogador] = update_ncartas(e->ncartas[e->jogador], e->ult_jogada[e->jogador]);
    }
    e->jogador = PROX_JOG(e->jogador);
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
