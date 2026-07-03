#include "Transporte.h"

// O construtor recebe todos os dados cadastrais do transporte e a cidade
// onde ele começa (localAtual). O atributo "viagemAtual" sempre começa como
// nullptr, pois um transporte recém-cadastrado nunca está em trânsito.
Transporte::Transporte(const std::string& nome, char tipo, int capacidade, int velocidade,
                        int distanciaEntreDescansos, int tempoDeDescanso, Cidade* localAtual)
    : nome(nome), tipo(tipo), capacidade(capacidade), velocidade(velocidade),
      distanciaEntreDescansos(distanciaEntreDescansos), tempoDeDescanso(tempoDeDescanso),
      localAtual(localAtual), viagemAtual(nullptr) {
}

std::string Transporte::getNome() const { return nome; }
char Transporte::getTipo() const { return tipo; }
int Transporte::getCapacidade() const { return capacidade; }
int Transporte::getVelocidade() const { return velocidade; }
int Transporte::getDistanciaEntreDescansos() const { return distanciaEntreDescansos; }
int Transporte::getTempoDeDescanso() const { return tempoDeDescanso; }

Cidade* Transporte::getLocalAtual() const {
    return localAtual;
}

void Transporte::setLocalAtual(Cidade* local) {
    // Chamado pelo ControladorDeTransito quando o transporte efetivamente
    // chega a uma nova cidade (fim de uma cadeia de viagens).
    localAtual = local;
}

Viagem* Transporte::getViagemAtual() const {
    return viagemAtual;
}

void Transporte::setViagemAtual(Viagem* viagem) {
    // Chamado por Viagem::iniciarViagem() (associa o transporte à viagem)
    // e por ControladorDeTransito::finalizarCadeia() (libera o transporte,
    // passando nullptr, quando a cadeia de viagens termina).
    viagemAtual = viagem;
}

bool Transporte::estaDisponivel() const {
    // O transporte está disponível se, e somente se, não estiver associado
    // a nenhuma viagem em andamento.
    return viagemAtual == nullptr;
}
