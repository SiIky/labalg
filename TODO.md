# TODO
- [o] Adaptar o codigo pra usar a struct (torna o programa mais facil de manter)
    - [X] Criar a struct e funcoes pra converter de e para o tipo ESTADO
    - [X] Adaptar as funcoes actuais pra usar as funcoes de conversao
    - [o] Actualizar a documentacao
    - [o] Corrigir erros de PT na documentacao
- [ ] Por os bots a jogar (nao interessa quem tem o 3 de Ouros pra ja, comeca sp o jogador)
    - [ ] Melhorar a funcao jogada_valida
        - [ ] Ter em conta naipes e valores das cartas
            - [X] Arranjar maneira de calcular o naipe e valor de uma carta dada uma carta
            - [ ] Dada uma jogada (de 0 ou + cartas), calcular o naipe e valor de cada uma
    - [ ] Jogadas de 1 carta
    - [ ] Jogadas de 2 cartas
    - [ ] Jogadas de 3 cartas
    - [ ] Jogadas de 5 cartas

# Bugs
- [X] Jogo crasha/congela ao clicar numa carta (prob nas funcoes imprime carta)
- [X] Cartas nao sao impressas depois de seleccionar uma carta
