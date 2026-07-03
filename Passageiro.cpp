#include "Passageiro.h"

// O construtor guarda o nome e a cidade inicial do passageiro. O ponteiro
// viagemAtual começa como nullptr: todo passageiro recém-cadastrado está
// parado, nunca em trânsito.
Passageiro::Passageiro(const std::string& nome, Cidade* localAtual)
    : nome(nome), localAtual(localAtual), viagemAtual(nullptr) {
}

std::string Passageiro::getNome() const {
    return nome;
}

Cidade* Passageiro::getLocalAtual() const {
    return localAtual;
}

void Passageiro::setLocalAtual(Cidade* local) {
    localAtual = local;
}

Viagem* Passageiro::getViagemAtual() const {
    return viagemAtual;
}

void Passageiro::setViagemAtual(Viagem* viagem) {
    viagemAtual = viagem;
}

bool Passageiro::estaDisponivel() const {
    return viagemAtual == nullptr;
}
