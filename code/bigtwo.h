/* funcao `jogada_valida` */
#define PLAY_SINGLE     1
#define PLAY_PAIR       2
#define PLAY_TRIPLE     3
#define PLAY_FIVE       5

/*==================================================================*/
void    parse                   (char *query);
int     valores_iguais          (Card *c, int N);
int     jogada_valida           (const State *e);
void    imprime_bjogar          (State e);
void    imprime_blimpar         (State e);
void    imprime_carta           (const int x, int y, State e, const unsigned int idx);
void    imprime_ult_jogada      (const State *e);
void    imprime                 (const State *e);
