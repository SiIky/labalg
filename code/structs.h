/* =========================================================
 * Definição das Format Strings para printf e etc
 *
 *      jogador[n] := "mao[n]+(ult_jogada[n])+ncartas[n]+pontos[n]"
 *
 *      "jogador[0]&jogador[1]&jogador[2]&jogador[3]&(jogador)&(seleccao)&(ult_jogador_valido)&(ordena)&(decorrer)"
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
 *       ---|---------------|-----|---
 *        0 | ............. | ... | D
 *        1 | ............. | ... | C
 *        2 | ............. | ... | H
 *        3 | ............. | ... | S
 *       ---|---------------|-----|---
 *          | 34567891JQKA2 |
 *                   0
 */

/* comprimento máximo das strings */
#define MAXLEN          100240

#define INDICE_NAIPE(N, V)      (V + (N * 13))      /* ordenado por naipe (do stor) */
#define INDICE(N, V)            (N + (V * 4))       /* ordenado por figuras (nossa) */
#define REM_SELECCAO(E, S)      (E & ~S)            /* remove a seleccao de cartas de um dado estado */
#define UPDATE_DECORRER         (e.decorrer = update_decorrer(&e))

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
    unsigned int ordena;
    unsigned int decorrer;
} State;

/*==================================================================*/
unsigned int    trailingZ       (MAO n);
unsigned int    bitsUm          (MAO n);
unsigned int    update_ncartas  (const unsigned int ncartas, const MAO jogada);
unsigned int    update_decorrer (const State *e);
MAO             add_carta       (const MAO *e, const unsigned int idx);
MAO             rem_carta       (const MAO *e, const unsigned int idx);
int             carta_existe    (MAO e, const unsigned int idx);
Card            mao2carta       (unsigned int idx);
void            jogada2cartas   (Card *cartas, unsigned int b, const MAO jogada);
void            conta_cartas    (CardsCount *contas, const MAO e);
State           str2estado      (const char *str);
char*           estado2str      (const State *e);
void            baralhar        (State *e);
void            initEstado      (State *e);
int             test_play1      (const State *e);
int             test_play2      (const State *e);
int             test_play3      (const State *e);
int             test_play5      (const State *e);
int             tipodecombo     (const MAO *e);
int             test_4ofakind   (const MAO e);
int             test_fullhouse  (const MAO e);
int             test_flush      (const MAO e);
int             test_straight   (const MAO e);

/*==================================================================*/
/** \brief Determina se uma jogada de 1 carta é válida ou não de acordo com a última jogada

@param e        O estado do jogo
@return         Verdadeiro ou falso
*/
int test_play1 (const State *e)
{
    return (e->jogador == e->ult_jogador_valido ||
           e->seleccao > e->ult_jogada[e->ult_jogador_valido]);
}

/*==================================================================*/
/** \brief Determina se uma jogada de 2 cartas é válida ou não de acordo com a última jogada

@param e        O estado do jogo
@return         Verdadeiro ou falso
*/
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
/** \brief Determina se uma jogada de 3 cartas é válida ou não de acordo com a última jogada

@param e        O estado do jogo
@return         Verdadeiro ou falso
*/
int test_play3 (const State *e)
{
    Card jogada[3];
    jogada2cartas(jogada, 3, e->seleccao);
    Card ult_jogada[3];
    jogada2cartas(ult_jogada, 3, e->ult_jogada[e->ult_jogador_valido]);

    return (jogada[0].valor == jogada[1].valor  &&
            jogada[0].valor == jogada[2].valor  &&
           (e->jogador == e->ult_jogador_valido ||
           jogada[0].valor > ult_jogada[0].valor));
}

/*==================================================================*/
/** \brief Determina o tipo de combinação de 5 cartas de uma jogada

@param e        Jogada a testar
@return         O tipo de combinação ou 0 caso não seja uma combinação válida
*/
int tipodecombo (const MAO *e)
{
    int r = 0;

    if (test_straight(*e) && test_flush(*e))
        r = 5;
    else if (test_4ofakind(*e) < 13)
        r = 4;
    else if (test_fullhouse(*e) < 13)
        r = 3;
    else if (test_flush(*e) < 4)
        r = 2;
    else if (test_straight(*e) < 13)
        r = 1;

    return r;
}

/*==================================================================*/
/** \brief Determina se uma jogada (seleccao) de 5 cartas é um Straight

@param e        Jogada a testar
@return         Verdadeiro ou falso
*/
int test_straight (const MAO e)
{
    Card cartas[5];
    jogada2cartas(cartas, 5, e);
    return cartas[0].valor == (cartas[1].valor - 1) &&
           cartas[1].valor == (cartas[2].valor - 1) &&
           cartas[2].valor == (cartas[3].valor - 1) &&
           cartas[3].valor == (cartas[4].valor - 1);
}

/*==================================================================*/
/** \brief Determina se uma jogada (seleccao) de 5 cartas é um Flush

@param e        Jogada a testar
@return         Verdadeiro ou falso
*/
int test_flush (const MAO e)
{
    Card cartas[5];
    jogada2cartas(cartas, 5, e);
    return cartas[0].naipe == cartas[1].naipe &&
           cartas[0].naipe == cartas[2].naipe &&
           cartas[0].naipe == cartas[3].naipe &&
           cartas[0].naipe == cartas[4].naipe;
}

/*==================================================================*/
/** \brief Determina se uma jogada (seleccao) de 5 cartas é um Full House e devolve o valor do triplo caso seja

@param e        Jogada a testar
@return         O valor do triplo, caso seja um Full House, 13 caso contrário
*/
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
        j = 13;

    return j; /* o valor da carta mais alta, pra comparacoes */
}

/*==================================================================*/
/** \brief Determina se uma jogada (seleccao) de 5 cartas é um 4 of a kind e devolve a figura das 4 cartas caso seja 

@param e        Jogada a testar
@return         O valor das 4 cartas, caso seja um 4 of a kind, 13 caso contrário
*/
int test_4ofakind (const MAO e)
{
    CardsCount cartas;
    conta_cartas(&cartas, e);
    int v;

    for (v = 0; v < 13 && cartas.valores[v] != 4; v++);
    if (cartas.valores[v] != 4)
        v = 13;

    return v; /* o valor da carta mais alta, pra comparacoes */
}

/*==================================================================*/
/** \brief Determina se uma jogada (seleccao) de 5 cartas é válida ou não

@param e        O estado do jogo
@return         Se a seleccao é válida ou não
*/
int test_play5 (const State *e)
{
    return (test_straight(e->seleccao) < 13 && test_flush(e->seleccao))      ||
           (test_4ofakind(e->seleccao) < 13)                                 ||
           (test_fullhouse(e->seleccao) < 13)                                ||
           (test_flush(e->seleccao))                                         ||
           (test_straight(e->seleccao) < 13);
}

/*==================================================================*/
/** \brief Calcula o índice da carta mais baixa de uma mão

@param n        A mão a calcular
@return         O índice da carta mais baixa
*/
unsigned int trailingZ (MAO n)
{
    unsigned int count;
    for (count = 0; n > 0 && (n % 2 == 0); n >>= 1, count++);
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
/** \brief Actualiza o número de cartas de um jogador

@param ncartas  O número de cartas de um jogador
@param jogada   A última jogada de um jogador
@return         O número de cartas actualizado
*/
unsigned int update_ncartas (const unsigned int ncartas, const MAO jogada)
{
    unsigned int i = bitsUm(jogada);
    return ncartas - i;
}

/*==================================================================*/
/** \brief Calcula o naipe e valor de uma carta

@param idx      O índice da carta
@return         O par naipe/figura
*/
Card mao2carta (unsigned int idx)
{
    Card c;
    c.valor = (int) idx / 4;
    c.naipe = idx % 4;
    return c;
}

/*==================================================================*/
/** \brief Devolve uma lista de cartas (pares naipes/figuras)

@param cartas   Array onde vão ser guardadas as cartas
@param b        Número de cartas que jogada tem
@param jogada   A jogada a converter
@return         Os pares naipe/figura ordenados por figuras
*/
void jogada2cartas (Card *cartas, unsigned int b, const MAO jogada)
{
    unsigned int i, w;
    for (i = w = 0; i < 52 && w < b; i++)
        if (carta_existe(jogada, i))
            cartas[w++] = mao2carta(i);
}

/*==================================================================*/
/** \brief Devolve uma lista de cartas (pares naipes/figuras)

@param contas   Apontador para a estrutura onde vai ser guardado o resultado
@param e        A mão a converter
@return         O número de cartas de cada naipe e cada valor
*/
void conta_cartas (CardsCount *contas, const MAO e)
{
    Card c;
    unsigned int i;

    for (i = 0; i < 4; i++)
        contas->naipes[i] = 0;
    for (i = 0; i < 13; i++)
        contas->valores[i] = 0;
    for (i = 0; i < 52; i++)
        if (carta_existe(e, i)) {
            c = mao2carta(i);
            contas->valores[c.valor]++;
            contas->naipes[c.naipe]++;
        }
}

/*==================================================================*/
/** \brief Actualiza o campo decorrer do estado do jogo

@param e        O estado do jogo
@return         O campo decorrer actualizado
*/
unsigned int update_decorrer (const State *e)
{
    int j;
    for (j = 0; j < 4 && e->ncartas[j] > 0; j++);
    return (e->ncartas[j] > 0) ? 1 : 0;
}

/*==================================================================*/
/** \brief Transforma uma string e devolve o estado d jogo

@param str      A string a converter para estado
@return         O estado de jogo de acordo com a string
*/
State str2estado (const char *str)
{
    State e;
    sscanf(str, "q="
        "%llu+%llu+%u+%u&"
        "%llu+%llu+%u+%u&"
        "%llu+%llu+%u+%u&"
        "%llu+%llu+%u+%u&"
        "%u&%llu&%u&"
        "%u&%u",
        &e.mao[0], &e.ult_jogada[0], &e.ncartas[0], &e.pontos[0],
        &e.mao[1], &e.ult_jogada[1], &e.ncartas[1], &e.pontos[1],
        &e.mao[2], &e.ult_jogada[2], &e.ncartas[2], &e.pontos[2],
        &e.mao[3], &e.ult_jogada[3], &e.ncartas[3], &e.pontos[3],
        &e.jogador, &e.seleccao, &e.ult_jogador_valido,
        &e.ordena, &e.decorrer
    );
    return e;
}

/*==================================================================*/
/** \brief Transforma uma string e devolve o estado do jogo

@param str      A string a converter para estado
@return         O estado de jogo de acordo com a string
*/
char* estado2str (const State *e)
{
    static char str[MAXLEN];
    sprintf(str,
        "%llu+%llu+%u+%u&"
        "%llu+%llu+%u+%u&"
        "%llu+%llu+%u+%u&"
        "%llu+%llu+%u+%u&"
        "%u&%llu&%u&"
        "%u&%u",
        e->mao[0], e->ult_jogada[0], e->ncartas[0], e->pontos[0],
        e->mao[1], e->ult_jogada[1], e->ncartas[1], e->pontos[1],
        e->mao[2], e->ult_jogada[2], e->ncartas[2], e->pontos[2],
        e->mao[3], e->ult_jogada[3], e->ncartas[3], e->pontos[3],
        e->jogador, e->seleccao, e->ult_jogador_valido,
        e->ordena, e->decorrer
    );
    return str;
}

/*==================================================================*/
/** \brief Dá as cartas a cada jogador no início do jogo

@param e        O estado actual do jogo
@return         O novo estado do jogo
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
/** \brief Inicializa o jogo

@param e        O estado do jogo vazio
@return         O novo estado do jogo
*/
void initEstado (State *e)
{
    int i;

    e->seleccao = (MAO) 0;
    e->ordena = 1;
    e->decorrer = 1;

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
