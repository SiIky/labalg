# TODO
- [ ] Fazer um template HTML pro "tabuleiro" do jogo
- [ ] Por os bots a jogar
    - [ ] Melhorar a funcao jogada_valida
        - [X] Ter em conta naipes e valores das cartas
            - [X] Arranjar maneira de calcular o naipe e valor de uma carta dada uma carta
            - [X] Dada uma jogada (de 0 ou + cartas), calcular o naipe e valor de cada uma
    - [ ] Jogadas de 1 carta
    - [ ] Jogadas de 2 cartas
    - [ ] Jogadas de 3 cartas
    - [ ] Jogadas de 5 cartas
    - [ ] Funcao que imprima as jogadas anteriores
- [ ] Separar as funcoes em ficheiros externos pra tornar o programa mais facil de ler
    - [X] `all.h`: todos os includes necessarios
    - [X] `structs.h`: definicoes de tipos, funcoes relacionadas com esses mesmos tipos e prototipos de todas as funcoes
    - [X] `defs.h`: configuracoes, macros, etc...
    - [ ] `ui.h`: funcoes relativas a interface
- [X] Adaptar o codigo pra usar a struct (torna o programa mais facil de manter)
    - [X] Criar a struct e funcoes pra converter de e para o tipo ESTADO
    - [X] Adaptar as funcoes actuais pra usar as funcoes de conversao
    - [X] Actualizar a documentacao
    - [X] Corrigir erros de PT na documentacao

# Bugs
- [X] 500 Internal Error (erro do apache, possivel que seja causado pelo executavel, ou entao e so o meu PC)
- [X] Jogo crasha/congela ao clicar numa carta (prob nas funcoes imprime carta)
- [X] Cartas nao sao impressas depois de seleccionar uma carta
- [ ] Funcao `jogada_valida` devolve falso, independentemente das cartas seleccionadas
