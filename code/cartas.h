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
#define XUC_INIT        (13 * XC_STEP)
#define YJ_INIT         0            /* y inicial para jogador */
#define YJ_STEP         150          /* salto do y para jogador */

/* definições dos botoes */
#define SVG_WIDTH       150
#define SVG_HEIGHT      200
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

/*================== headers do cartas.c ===========================*/
/*==================================================================*/
void bot_joga (ESTADO *e);
void parse (char *query);
CARTA mao2carta (MAO carta);
CARTA* jogada2cartas (MAO jogada);
int valores_iguais (CARTA cartas[]);
unsigned int trailingZ (MAO n);
unsigned int bitsUm (MAO n);
int jogada_valida (const ESTADO *e);
int carta_existe (MAO e, const unsigned int idx);
MAO add_carta (const MAO *e, const unsigned int idx);
MAO rem_carta (const MAO *e, const unsigned int idx);
void imprime_bjogar (ESTADO e);
void imprime_blimpar (ESTADO e);
void imprime_carta (const char *path, const int x, int y, ESTADO e, const unsigned int idx);
void imprime_ult_jogada (const char *path, const ESTADO *e);
void imprime (const char *path, const ESTADO *e);
