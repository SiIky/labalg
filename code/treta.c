void parse(char *query) {
    long long int mao[4];
    if(sscanf(query, "q=%lld+%lld+%lld+%lld", &mao[0], &mao[1], &mao[2], &mao[3]) == 1) {
        imprime(BARALHO, mao);
    } else {
        imprime(BARALHO, mao);
    }
}
