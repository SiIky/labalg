/* =========================================================
 * Definição das Format Strings para printf e etc
 *
 *      jogador[n] := "mao[n]+(ult_jogada[n])+ncartas[n]+pontos[n]"
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
 *          | 0123456789ABC | DEF
 *       ---|---------------|-----|----------
 *        0 | ............. | ... | Diamonds
 *        1 | ............. | ... | Clubs
 *        2 | ............. | ... | Heart
 *        3 | ............. | ... | Spades
 *       ---|---------------|-----|----------
 *          | 34567891JQKA2 |
 *                   0
 */

/* comprimento máximo das strings */
#define MAXLEN          10240
#define TERNOS          ((MAO) 1 << 3)

#define PROX_JOG(J)             ((J + 1) % 4)
#define JOG_ANT(J)              ((J + 3) % 4)
#define INDICE_NAIPE(N, V)      (V + (N * 13))      /* ordenado por naipe (do stor) */
#define INDICE(N, V)            (N + (V * 4))       /* ordenado por figuras (nossa) */
#define REM_SELECCAO(E, S)      (E & ~S)            /* remove a seleccao de cartas de um dado estado */

typedef struct {
    unsigned int naipes[4];
    unsigned int valores[13];
} CardsCount;

typedef struct {
    unsigned int naipe;
    unsigned int valor;
} Card;

typedef unsigned long long int MAO;
typedef struct {
    MAO mao[4];
    MAO ult_jogada[4];
    MAO seleccao;
    unsigned int ncartas[4];
    unsigned int jogador;
    unsigned int ult_jogador_valido;
    unsigned int pontos[4];
} State;

/*==================================================================*/
MAO             add_carta       (const MAO *e, const unsigned int idx);
MAO             rem_carta       (const MAO *e, const unsigned int idx);
int             carta_existe    (MAO e, const unsigned int idx);
Card            mao2carta       (MAO carta);
void            jogada2cartas   (Card *cartas, unsigned int b, const MAO jogada);
void            conta_cartas    (CardsCount *contas, const MAO e);
State           str2estado      (const char *str);
char*           estado2str      (const State *e);
void            baralhar        (State *e);
void            initEstado      (State *e);
unsigned int    trailingZ       (MAO n);
unsigned int    bitsUm          (MAO n);
int             test_play1      (const State *e);
int             test_play2      (const State *e);
int             test_play3      (const State *e);
/*int             test_play5      (const State *e);*/
int             tipodecombo     (const MAO *e);
int             test_4ofakind   (const MAO e);
int             test_fullhouse  (const MAO e);
int             test_flush      (const MAO e);
int             test_straight   (const MAO e);

/*==================================================================*/
int test_play1 (const State *e)
{
    return (e->jogador == e->ult_jogador_valido ||
           e->seleccao > e->ult_jogada[e->ult_jogador_valido]);
}

/*==================================================================*/
int test_play2 (const State *e)
{
    Card jogada[2];
    jogada2cartas(jogada, 2, e->seleccao);
    Card ult_jogada[2];
    jogada2cartas(ult_jogada, 2, e->ult_jogada[e->ult_jogador_valido]);

    return (jogada[0].valor == jogada[1].valor &&
           (e->jogador == e->ult_jogador_valido ||
           jogada[0].valor > ult_jogada[0].valor ||
           (jogada[0].valor == ult_jogada[0].valor &&
           jogada[1].naipe > ult_jogada[1].naipe)));
}

/*==================================================================*/
int test_play3 (const State *e)
{
    Card jogada[3];
    jogada2cartas(jogada, 3, e->seleccao);
    Card ult_jogada[3];
    jogada2cartas(ult_jogada, 3, e->ult_jogada[e->ult_jogador_valido]);

    return (jogada[0].valor == jogada[1].valor &&
           jogada[0].valor == jogada[2].valor &&
           jogada[0].valor > ult_jogada[0].valor);
}

/*==================================================================*/
int tipodecombo (const MAO *e)
{
    if (test_straight(*e) && test_flush(*e))
        return 5;
    else if (test_4ofakind(*e) < 13)
        return 4;
    else if (test_fullhouse(*e) < 13)
        return 3;
    else if (test_flush(*e) < 4)
        return 2;
    else if (test_straight(*e) < 13)
        return 1;

    return 0;
}

/*==================================================================*/
int test_straight (const MAO e)
{
    Card cartas[5];
    jogada2cartas(cartas, 5, e);
    int res = cartas[0].valor == (cartas[1].valor - 1) &&
           cartas[1].valor == (cartas[2].valor - 1) &&
           cartas[2].valor == (cartas[3].valor - 1) &&
           cartas[3].valor == (cartas[4].valor - 1);
    printf("test_straight: %d\n", res);
    return res;
}

/*==================================================================*/
int test_flush (const MAO e)
{
    Card cartas[5];
    jogada2cartas(cartas, 5, e);
    int res = cartas[0].naipe == cartas[1].naipe &&
            cartas[0].naipe == cartas[2].naipe &&
            cartas[0].naipe == cartas[3].naipe &&
            cartas[0].naipe == cartas[4].naipe;
    printf("test_flush: %d\n", res);
    return res; /* tem de devolver o naipe da carta mais alta (cartas[4].valor) */
}

/*==================================================================*/
int test_fullhouse (const MAO e)
{
    CardsCount cartas;
    conta_cartas(&cartas, e);
    int v, j;

    /* procura por um triplo */
    for (j = 0; j < 13 && cartas.valores[j] != 3; j++);
    if (cartas.valores[j] != 3)
        return 13;

    /* procura por um par */
    for (v = 0; v < 13 && cartas.valores[v] != 2; v++);
    if (cartas.valores[v] != 2)
        return 13;

    return j; /* o valor da carta mais alta, pra comparacoes */
}

/*==================================================================*/
int test_4ofakind (const MAO e)
{
    CardsCount cartas;
    conta_cartas(&cartas, e);
    int v;

    for (v = 0; v < 13 && cartas.valores[v] != 4; v++);
    if (cartas.valores[v] != 4)
        return 13;

    return v; /* o valor da carta mais alta, pra comparacoes */
}

/*==================================================================*/
void joga_single (State *e)
{
    MAO a_jogar;
    unsigned int idx, i;
    if (e->jogador == e->ult_jogador_valido) {
        idx = trailingZ(e->mao[e->jogador]);
        a_jogar = (carta_existe(e->mao[e->jogador], ((MAO) 1 << idx)) ?
                  (MAO) 1 << idx :
                  0);
    } else {
        idx = trailingZ(e->ult_jogada[e->ult_jogador_valido]);
        for (i = idx; i < 52 && (carta_existe(e->mao[e->jogador], ((MAO) 1 << i))); i++);
        a_jogar = (carta_existe(e->mao[e->jogador], ((MAO) 1 << i)) ?
                  (MAO) 1 << i :
                  0);
    }
    e->mao[e->jogador] = REM_SELECCAO(e->mao[e->jogador], a_jogar);
    e->ult_jogada[e->jogador] = a_jogar;
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
/** \brief Adiciona uma carta ao estado

@param e        Uma mão
@param idx      O indice da carta (inteiro entre 0 e 51)
@return         A nova mão
*/
MAO add_carta (const MAO *e, const unsigned int idx)
{
    return (*e | ((MAO) 1 << idx));
}

/*==================================================================*/
/** \brief Remove uma carta do estado

@param e        Uma mão
@param idx      O indice da carta (inteiro entre 0 e 51)
@return         A nova mão
*/
MAO rem_carta (const MAO *e, const unsigned int idx)
{
    return (*e & ~((MAO) 1 << idx));
}

/*==================================================================*/
/** \brief Verifica se uma carta pertence ao estado

@param e        Uma mão
@param idx      O indice da carta (inteiro entre 0 e 51)
@return         1 se a carta existe, 0 caso contrário
*/
int carta_existe (MAO e, const unsigned int idx)
{
    return ((e >> idx) & ((MAO) 1));
}

/*==================================================================*/
Card mao2carta (MAO carta)
{
    Card c;
    for (c.valor = 0; carta > TERNOS; c.valor++)
        carta >>= 4;
    for (c.naipe = 0; !(carta_existe(carta, 0)); c.naipe++)
        carta >>= 1;
    return c;
}

/*==================================================================*/
/** \brief Devolve uma lista de cartas (pares naipes/figuras)

@param jogada   A jogada a converter
@return         Os pares naipe/figura ordenados por figuras
*/
void jogada2cartas (Card *cartas, unsigned int b, const MAO jogada)
{
    unsigned int i, w;
    for (i = w = 0; i < 52 && w < b; i++)
        if (carta_existe(jogada, i))
            cartas[w++] = mao2carta((MAO) 1 << i);
}

/*==================================================================*/
void conta_cartas (CardsCount *contas, const MAO e)
{
    Card c;
    unsigned int i;
    for (i = 0; i < 52; i++)
        if (carta_existe(e, i)) {
            c = mao2carta((MAO) 1 << i);
            contas->valores[c.valor]++;
            contas->naipes[c.naipe]++;
        }
}

/*==================================================================*/
State str2estado (const char *str)
{
    State e;
    sscanf(str, "q="
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%u_%llu_%u",
        &e.mao[0], &e.ult_jogada[0], &e.ncartas[0], &e.pontos[0],
        &e.mao[1], &e.ult_jogada[1], &e.ncartas[1], &e.pontos[1],
        &e.mao[2], &e.ult_jogada[2], &e.ncartas[2], &e.pontos[2],
        &e.mao[3], &e.ult_jogada[3], &e.ncartas[3], &e.pontos[3],
        &e.jogador, &e.seleccao, &e.ult_jogador_valido
    );
    return e;
}

/*==================================================================*/
char* estado2str (const State *e)
{
    static char str[MAXLEN];
    sprintf(str,
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%llu+%llu+%u+%u_"
        "%u_%llu_%u",
        e->mao[0], e->ult_jogada[0], e->ncartas[0], e->pontos[0],
        e->mao[1], e->ult_jogada[1], e->ncartas[1], e->pontos[1],
        e->mao[2], e->ult_jogada[2], e->ncartas[2], e->pontos[2],
        e->mao[3], e->ult_jogada[3], e->ncartas[3], e->pontos[3],
        e->jogador, e->seleccao, e->ult_jogador_valido
    );
    return str;
}

/*==================================================================*/
/** \brief Dá as cartas a cada jogador no início do jogo

@param e        O estado actual do jogo
@return e       O novo estado do jogo
*/
void baralhar (State *e)
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
void initEstado (State *e)
{
    int i;
    e->seleccao = (MAO) 0;
    for (i = 0; i < 4; i++) {
        e->mao[i] = (MAO) 0;
        e->ult_jogada[i] = (MAO) 0;
        e->ncartas[i] = 0;
        e->pontos[i] = 0;
    }
    baralhar(e);
    for (i = 0; i < 4 && (e->mao[i] % 2 == 0); i++);
    e->ult_jogador_valido = e->jogador = i;
}
