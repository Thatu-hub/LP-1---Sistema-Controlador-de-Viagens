#include "Trajeto.h"

// O construtor apenas copia os ponteiros e valores recebidos para os
// atributos internos. Toda a validação (se as cidades existem, se o tipo é
// válido, se a distância é positiva) é responsabilidade de quem CRIA o
// Trajeto — no nosso sistema, isso é feito em
// ControladorDeTransito::cadastrarTrajeto, ANTES de chamar este construtor.
// Mantemos o Trajeto "burro" de propósito: ele só guarda dados.

Trajeto::Trajeto(Cidade* origem, Cidade* destino, char tipo, int distancia)
    : origem(origem), destino(destino), tipo(tipo), distancia(distancia) {
}

Cidade* Trajeto::getOrigem() const {
    return origem;
}

Cidade* Trajeto::getDestino() const {
    return destino;
}

char Trajeto::getTipo() const {
    return tipo;
}

int Trajeto::getDistancia() const {
    return distancia;
}
