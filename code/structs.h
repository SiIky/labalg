/* =========================================================
 * Definição das Format Strings para printf e etc
 *
 *      jogador[n] := "mao[n]+(ult_jogada[n])+ncartas[n]"
 *
 *      "jogador[0]_jogador[1]_jogador[2]_jogador[3]_(jogador)_(seleccao)_(ult_jogador_valido)"
 *
 * =========================================================
 * Última jogada:
 *
 *   == 0 -> PASSOU
 *   != 0 -> JOGOU
 *
 * =========================================================
 *  Imaginando que o tipo `MAO` e uma matriz e seja `MAO e;`
 *  O valor de uma posicao `(n,v)` na matriz `e` e dado por
 *
 *      e[n][v]         =>      (e & ((MAO) 1 << INDICE(N, V)))
 *
 *  Pra escrever numa posicao `(n,v)` da matriz `e`
 *
 *      e[n][v] = 0     =>      (e & ~((MAO) 1 << INDICE(N, V)))
 *      e[n][v] = 1     =>      (e |  ((MAO) 1 << INDICE(N, V)))
 * =========================================================
 */

/* comprimento máximo das strings */
#define MAXLEN          10240
#define TERNOS          (((MAO) 1) << 3)

#define INDICE_NAIPE(N, V)      ((V) + ((N) * 13))      /* ordenado por naipe (do stor) */
#define INDICE(N, V)            ((N) + ((V) * 4))       /* ordenado por figuras (nossa) */
#define REM_SELECCAO(E, S)      ((E) & ~(S))            /* remove a seleccao de cartas de um dado estado */

/* valor de uma matriz/estado E na posicao/indice I */
#define VALMX(E,I)      ((E) & ((MAO) 1 << I))

/* Escreve N (0 ou 1) na posicao/indice I da matriz/estado E */
#define ESCMX(E,I,N)    ((N) == 0) ? \
                        (rem_carta((E), (I))) : \
                        (add_carta((E), (I));

typedef struct card {
    unsigned int naipe, valor;
} CARTA;

typedef unsigned long long int MAO;
typedef struct state {
    MAO mao[4];
    MAO ult_jogada[4];
    MAO seleccao;
    unsigned int ncartas[4];
    unsigned int jogador;
    unsigned int ult_jogador_valido;
} ESTADO;

/*================== headers do structs.c ==========================*/
/*==================================================================*/
MAO add_carta (const MAO *e, const unsigned int idx);
MAO rem_carta (const MAO *e, const unsigned int idx);
int carta_existe (MAO e, const unsigned int idx);
CARTA mao2carta (MAO carta);
CARTA* jogada2cartas (MAO jogada);
ESTADO str2estado (const char *str);
char* estado2str (const ESTADO *e);
void baralhar (ESTADO *e);
void initEstado (ESTADO *e);

/*==================================================================*/
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

/*==================================================================*/
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

/*==================================================================*/
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

/*==================================================================*/
CARTA mao2carta (MAO carta)
{
    CARTA c;
    for (c.valor = 0; carta > TERNOS; c.valor++)
        carta >>= 4;
    for (c.naipe = 0; (carta ^ 1); carta >>= 1)
        c.naipe++;
    return c;
}

/*==================================================================*/
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

/*==================================================================*/
ESTADO str2estado (const char *str)
{
    ESTADO e;
    sscanf(str, "q="
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%u_%llu_%u",
        &e.mao[0], &e.ult_jogada[0], &e.ncartas[0],
        &e.mao[1], &e.ult_jogada[1], &e.ncartas[1],
        &e.mao[2], &e.ult_jogada[2], &e.ncartas[2],
        &e.mao[3], &e.ult_jogada[3], &e.ncartas[3],
        &e.jogador, &e.seleccao, &e.ult_jogador_valido
    );
    return e;
}

/*==================================================================*/
char* estado2str (const ESTADO *e)
{
    static char str[MAXLEN];
    sprintf(str,
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%llu+%llu+%u_"
        "%u_%llu_%u",
        e->mao[0], e->ult_jogada[0], e->ncartas[0],
        e->mao[1], e->ult_jogada[1], e->ncartas[1],
        e->mao[2], e->ult_jogada[2], e->ncartas[2],
        e->mao[3], e->ult_jogada[3], e->ncartas[3],
        e->jogador, e->seleccao, e->ult_jogador_valido
    );
    return str;
}

/*==================================================================*/
/** \brief Dá as cartas a cada jogador no início do jogo

@param e        O estado actual do jogo
@return e       O novo estado do jogo
*/
void baralhar (ESTADO *e)
{
    int j;      /* jogador */
    int i;

    for (i = 0; i < 52; i++) {
        do j = random() % 4; while (e->ncartas[j] >= 13);
        e->mao[j] = add_carta(&e->mao[j], i);
        e->ncartas[j]++;
    }
}

/*==================================================================*/
void initEstado (ESTADO *e)
{
    int i;
    e->seleccao = (MAO) 0;                     /* cartas selecionadas pelo jogador */
    for (i = 0; i < 4; i++) {
        e->mao[i] = (MAO) 0;                   /* começam todas vazias */
        e->ult_jogada[i] = (MAO) 0;            /* começam todas vazias */
        e->ncartas[i] = 0;                     /* jogadores começam com 0 cartas */
    }
    baralhar(e);
    for (i = 0; i < 4 && (e->mao[i] % 2 == 0); i++);
    e->ult_jogador_valido = e->jogador = i;
}
