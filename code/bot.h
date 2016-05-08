#define PROX_JOG(J)             ((J + 1) % 4)
#define JOG_ANT(J)              ((J + 3) % 4)

void    bot_joga        (State *e);
int     bot_play1       (State *e);
int     bot_play23      (State *e, unsigned int N);
int     escolhe_jogada  (State *e);

/*==================================================================*/
/** \brief Função que é chamada quando é a vez do bot jogar

@param e        O estado do jogo
*/
void bot_joga (State *e)
{
    unsigned int nb;
    int r = 0;

    if (e->ult_jogador_valido == e->jogador) { /* pode jogar qq coisa */
        r = escolhe_jogada(e);
    } else {
        nb = bitsUm(e->ult_jogada[e->ult_jogador_valido]);
        switch (nb) {
            case PLAY_FIVE:
                /* r = bot_play5(e); */
                break;
            case PLAY_TRIPLE:
            case PLAY_PAIR:
                r = bot_play23(e, nb);
                break;
            case PLAY_SINGLE:
                r = bot_play1(e);
                break;
        }
    }

    if (r != 0) /* jogou */
        e->ult_jogador_valido = e->jogador;
    else /* passou */
        e->ult_jogada[e->jogador] = (MAO) 0;

    e->ncartas[e->jogador] = update_ncartas(e->ncartas[e->jogador], e->ult_jogada[e->jogador]);
    e->jogador = PROX_JOG(e->jogador);
}

/*==================================================================*/
/** \brief Função do bot para jogar 1 carta

@param e        O estado do jogo
*/
int bot_play1 (State *e)
{
    MAO a_jogar = 0;
    unsigned int idx, i;
    int r = 0;

    if (e->jogador == e->ult_jogador_valido) {
        idx = trailingZ(e->mao[e->jogador]);
        a_jogar = (MAO) 1 << idx;
    } else {
        idx = trailingZ(e->ult_jogada[e->ult_jogador_valido]);
        for (i = idx; i < 52 && !(carta_existe(e->mao[e->jogador], i)); i++);
        a_jogar = ((carta_existe(e->mao[e->jogador], i)) ?
                  (MAO) 1 << i :
                  (MAO) 0);
    }

    if (a_jogar != (MAO) 0) {
        e->mao[e->jogador] = REM_SELECCAO(e->mao[e->jogador], a_jogar);
        r = 1;
    }
    e->ult_jogada[e->jogador] = a_jogar;

    return r;
}

/*==================================================================*/
/** \brief Função do bot para jogar pares e trios

@param e        O estado do jogo
*/
int bot_play23 (State *e, unsigned int N)
{
    unsigned int i, n;
    unsigned int idx = 0;
    int r = 0;
    MAO a_jogar = 0;
    MAO mao = e->mao[e->jogador];
    MAO ult_jogada = e->ult_jogada[e->ult_jogador_valido];
    CardsCount c;

    if (e->jogador != e->ult_jogador_valido) {
        for (i = 1; i < N; i++) {
            idx = trailingZ(ult_jogada);
            ult_jogada = rem_carta(&ult_jogada, idx);
        }
        idx = trailingZ(ult_jogada);
        for (i = 0; i < idx; i++)
            mao = rem_carta(&mao, i);
    }

    conta_cartas(&c, mao);
    for (i = 0; i < 13 && c.valores[i] < N; i++);
    if (c.valores[i] >= N) {
        for (n = 0; n < 4 && (N > 0); n++) {
            idx = INDICE(n, i);
            if (carta_existe(e->mao[e->jogador], idx)) {
                a_jogar = add_carta(&(e->mao[e->jogador]), idx);
                N--;
            }
        }
        r = 1;
    }

    e->mao[e->jogador] = REM_SELECCAO(e->mao[e->jogador], a_jogar);
    e->ult_jogada[e->jogador] = a_jogar;

    return r;
}

/*==================================================================*/
/** \brief Função do bot para escolher o tipo de jogada mais alta, quando pode jogar à escolha

@param e        O estado do jogo
*/
int escolhe_jogada (State *e)
{
    int r, i;
    for (i = 3, r = 0; i > 0 && r == 0; i--)
        switch (i) {
            case 1:
                r = bot_play1(e);
                break;
            case 2:
                r = bot_play23(e, 2);
                break;
            case 3:
                r = bot_play23(e, 3);
                break;
            /*case 4:
                r = bot_play5(e);
                break;*/
        }
    return r;
}
