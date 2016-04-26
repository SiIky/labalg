/* URL da CGI */
#define SCRIPT          "http://127.0.0.1/cgi-bin/bigtwo"

/* URL da pasta com as cartas */
#define BARALHO         "http://127.0.0.1/cards"

/* ordem dos naipes e valores */
#define NAIPES          "DCHS"
#define VALORES         "3456789TJQKA2"

/* valores usados pela função imprime */
#define COR_TABULEIRO   "#116611"    /* RGB em HEX */
#define XC_INIT         0            /* x inicial para cartas */
#define YC_INIT         10          /* y inicial para cartas */
#define XC_STEP         30           /* salto do x para cartas */
#define YC_SEL_STEP     10           /* salto de cartas selecionadas */
#define XUC_INIT        0
#define YJ_INIT         0            /* y inicial para jogador */
#define YJ_STEP         120          /* salto do y para jogador */

/* definições dos botoes */
#define SVG_WIDTH       200
#define SVG_HEIGHT      120
#define COR_BOT_A       "#C99660"        /* cor dos botões activados */
#define COR_BOT_D       "#999999"        /* cor dos botões não activados */
#define RECT_X          0                /* posicao dos botoes */
#define RECT_Y          0                /* posicao dos botoes */
#define RECT_WIDTH      100              /* largura dos botoes */
#define RECT_HEIGHT     50               /* altura dos botoes */
#define TXT_X           20               /* posicao do texto nos botoes */
#define TXT_Y           30               /* posicao do texto nos botoes */

/* funcao `jogada_valida` */
#define PLAY_SINGLE     1
#define PLAY_PAIR       2
#define PLAY_TRIPLE     3
#define PLAY_FIVE       5

/*==================================================================*/
void    bot_joga                (State *e);
void    parse                   (char *query);
int     valores_iguais          (Card cartas[]);
int     jogada_valida           (const State *e);
void    imprime_bjogar          (State e);
void    imprime_blimpar         (State e);
void    imprime_carta           (const int x, int y, State e, const unsigned int idx);
void    imprime_ult_jogada      (const State *e);
void    imprime                 (const State *e);
