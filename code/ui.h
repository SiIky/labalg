/*==================================================================*/
/** \brief Imprime o botão Jogar

@param e        O estado actual do jogo
*/
void imprime_bjogar (State e)
{
    char link[MAXLEN];

    printf(
        "<SVG WIDTH=%d HEIGHT=%d "
        "STYLE=\"POSITION:ABSOLUTE; TOP:350px; LEFT:175px\">",
        RECT_WIDTH, RECT_HEIGHT
    );
    if (jogada_valida(&e)) {
        e.jogador = PROX_JOG(e.jogador);
        e.ult_jogador_valido = 0;
        e.mao[0] = REM_SELECCAO(e.mao[0], e.seleccao);
        e.ult_jogada[0] = e.seleccao;
        e.ncartas[0] -= bitsUm(e.seleccao);
        e.seleccao = (MAO) 0;
        sprintf(link, "%s?q=%s", SCRIPT, estado2str(&e));
        printf(
            "<A XLINK:HREF=\"%s\">"
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"FILL:%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Jogar</TEXT></A></SVG>\n",
            link,
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A,
            TXT_X, TXT_Y
        );
    } else {
        printf(
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Jogar</TEXT></SVG>\n",
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D,
            TXT_X, TXT_Y
        );
    }
}

/*==================================================================*/
/** \brief Imprime o botão Limpar

@param e        O estado actual do jogo
*/
void imprime_bpassar (State e)
{
    char link[MAXLEN];
    printf(
        "<SVG WIDTH=%d HEIGHT=%d "
        "STYLE=\"POSITION:ABSOLUTE; TOP:350px; LEFT:405px\">",
        RECT_WIDTH, RECT_HEIGHT
    );

    e.jogador = PROX_JOG(e.jogador);
    e.ult_jogada[0] = (MAO) 0;
    sprintf(link, "%s?q=%s", SCRIPT, estado2str(&e));
    printf(
        "<A XLINK:HREF=\"%s\">"
        "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:%s\"/>"
        "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Passar</TEXT></A></SVG>\n",
        link,
        RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A,
        TXT_X, TXT_Y
    );
}

/*==================================================================*/
/** \brief Imprime o botão Limpar

@param e        O estado actual do jogo
*/
void imprime_blimpar (State e)
{
    char link[MAXLEN];
    printf(
        "<SVG WIDTH=%d HEIGHT=%d "
        "STYLE=\"POSITION:ABSOLUTE; TOP:350px; LEFT:290px\">",
        RECT_WIDTH, RECT_HEIGHT
    );
    if (e.seleccao != 0) {
        e.seleccao = 0;
        sprintf(link, "%s?q=%s", SCRIPT, estado2str(&e));
        printf(
            "<A XLINK:HREF=\"%s\">"
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Limpar</TEXT></A></SVG>\n",
            link,
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_A,
            TXT_X, TXT_Y
        );
    } else {
        printf(
            "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:%s\"/>"
            "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Limpar</TEXT></SVG>\n",
            RECT_X, RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D,
            TXT_X, TXT_Y
        );
    }
}

/*==================================================================*/
void imprime_ult_jogada (const State *e)
{
    int j;                      /* jogador */
    int xc;                     /* x inicial, das cartas */
    int yj;                     /* y inicial, dos jogadores */
    int i;                      /* indice da carta a imprimir */
    Card c;

    printf( /* opening SVG tag */
        "<SVG WIDTH=%d HEIGHT=%d "
        "STYLE=\"POSITION:ABSOLUTE; TOP:10px; LEFT:600px\">\n",
        SVG_WIDTH, 4 * SVG_HEIGHT
    );

    yj = YJ_INIT;
    for (j = 0; j < 4; j++) {
        if (e->ult_jogada[j] != 0) { /* imprime as cartas jogadas */
            xc = XUC_INIT;
            for (i = 0; i < 52; i++) {
                if (carta_existe(e->ult_jogada[j], i)) {
                    c = mao2carta((MAO) 1 << i);
                    printf(
                        "<IMAGE X=%d Y=%d WIDTH=80 HEIGHT=110 XLINK:HREF=\"%s/%c%c.svg\"/>\n",
                        xc, yj, BARALHO, VALORES[c.valor], NAIPES[c.naipe]
                    );
                    xc += XC_STEP;
                }
            }
        } else { /* imprime um rect a dizer passou */
            printf(
                "<RECT X=%d Y=%d WIDTH=%d HEIGHT=%d RY=5 STYLE=\"fill:%s\"/>"
                "<TEXT X=%d Y=%d TEXT-ANCHOR=\"MIDLE\" TEXT-ALIGN=\"CENTER\" FONT-FAMILY=\"SERIF\" FONT-WEIGHT=\"BOLD\">Passou</TEXT>\n",
                RECT_X, j*RECT_Y, RECT_WIDTH, RECT_HEIGHT, COR_BOT_D,
                TXT_X, j*RECT_Y+20
            );
        }
        yj += YJ_STEP;
    }
    printf("</SVG>\n");
}

/*==================================================================*/
/** \brief Imprime o html correspondente a uma carta

@param x        A coordenada x da carta
@param y        A coordenada y da carta
@param e        O estado actual do jogo
@param idx      O valor da carta (inteiro entre 0 e 51)
*/
void imprime_carta (const int x, int y, State e, const unsigned int idx)
{
    if (carta_existe(e.seleccao, idx)) {
        y -= YC_SEL_STEP;
        e.seleccao = rem_carta(&e.seleccao, idx);
    } else {
        e.seleccao = add_carta(&e.seleccao, idx);
    }

    Card c = mao2carta((MAO) 1 << idx);
    char script[MAXLEN];
    sprintf(script, "%s?q=%s", SCRIPT, estado2str(&e));
    printf(
        "\t<A XLINK:HREF=\"%s\">"
        "<IMAGE X=%d Y=%d WIDTH=80 HEIGHT=110 XLINK:HREF=\"%s/%c%c.svg\"/></A>\n",
        script,
        x, y, BARALHO, VALORES[c.valor], NAIPES[c.naipe]
    );
}

/*==================================================================*/
/** \brief Imprime o estado do jogo

Esta função imprime a mão do jogador
@param e        O estado atual do jogo
*/
void imprime (const State *e)
{
    int j;                      /* jogador */
    int xc = XC_INIT;           /* x inicial */
    int yc = YC_INIT;           /* y inicial */
    int yj = YJ_INIT;           /* tabuleiros dos jogadores */
    int i;
    MAO mao = e->mao[0];

    /* -------------- bloco das cartas ------------------ */
    /* -------------------------------------------------- */
    printf( /* opening SVG tag */
        "<SVG WIDTH=440 HEIGHT=120 "
        "STYLE=\"POSITION:ABSOLUTE; TOP:200px; LEFT:100px\">\n"
    );
    /* imprime a mao do jogador */
    for (i = 0; mao > 0; mao >>= 1, i++)
        if (mao & (MAO) 1) {
            imprime_carta(xc, yc, *e, i);
            xc += XC_STEP;
        }
    printf("</SVG>\n");         /* closing SVG tag */
    /* -------------------------------------------------- */

    /* imprime a tabela de info sobre o jogo */
    printf(
        "<TABLE BORDER=\"4px\" BORDERCOLOR=\"BLACK\" "
        "STYLE=\"BACKGROUND-COLOR: YELLOW\">\n"
        "<TR>"
        "<TH>Jogador</TH><TH># de cartas</TH><TH>Pontos</TH>"
        "</TR>\n"
    );
    for (j = 0; j < 4; yj += YJ_STEP, j++)
        printf(
            "<TR><TD>%u</TD><TD>%u</TD><TD>%u</TD></TR>\n",
            j+1, e->ncartas[j], e->pontos[j]
        );
    printf("</TABLE>\n");
    imprime_ult_jogada(e);
    imprime_bjogar(*e);
    imprime_blimpar(*e);
    imprime_bpassar(*e);
}
