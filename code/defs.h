/* comprimento máximo das strings */
#define MAXLEN 10240

/* URL da CGI */
#define SCRIPT          "http://127.0.0.1/cgi-bin/cartas"

/* URL da pasta com as cartas */
#define BARALHO         "http://127.0.0.1/cards"

/* Ordem dos naipes */
#define NAIPES          "DCHS"
#define ESPADAS         (((MAO) 1) << 3)
#define COPAS           (((MAO) 1) << 2)
#define PAUS            (((MAO) 1) << 1)

/* Ordem das cartas */
#define VALORES         "3456789TJQKA2"
#define TERNOS          (((MAO) 1) << 3)

/* valores usados pela função imprime */
#define COR_TABULEIRO   "116611"    /* RGB em HEX */
#define XC_INIT         10          /* x inicial para cartas */
#define YC_INIT         10          /* y inicial para cartas */
#define XC_STEP         20          /* salto do x para cartas */
#define YC_STEP         150         /* salto do y para cartas */
#define YC_SEL_STEP     10          /* salto de cartas selecionadas */
#define YJ_INIT         0           /* y inicial para jogador */
#define YJ_STEP         150         /* salto do y para jogador */

/* definições do botão jogar */
#define SVG_WIDTH       150
#define SVG_HEIGHT      200
#define COR_BOT_A       "C99660"        /* cor dos botões activados */
#define COR_BOT_D       "999999"        /* cor dos botões não activados */
#define RECT_X          50
#define RECT_Y          50
#define RECT_WIDTH      100
#define RECT_HEIGHT     50
#define TXT_X           100
#define TXT_Y           80

#define PLAY_SINGLE     1
#define PLAY_PAIR       2
#define PLAY_TRIPLE     3
#define PLAY_FIVE       5

#define INDICE_NAIPE(N, V)      ((V) + ((N) * 13))      /* ordenado por naipe (do stor) */
#define INDICE(N, V)            ((N) + ((V) * 4))       /* ordenado por figuras (nossa) */
#define REM_SELECCAO(E, S)      ((E) & ~(S))            /* remove a seleccao de cartas de um dado estado */
