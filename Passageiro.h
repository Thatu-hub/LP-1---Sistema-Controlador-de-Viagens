#ifndef PASSAGEIRO_H
#define PASSAGEIRO_H

#include <string>
#include "Cidade.h"

// Forward declaration de Viagem — mesmo motivo explicado em Transporte.h:
// evitar dependência circular de includes, já que Passageiro guarda apenas
// um PONTEIRO para Viagem.

class Viagem;

// Representa uma pessoa cadastrada no sistema, que pode embarcar em viagens
// (respeitando a capacidade do transporte escolhido).
//
// Assim como a classe Transporte, o Passageiro guarda:
//   - "localAtual": a cidade onde ele está parado (ou a última cidade
//     confirmada de partida, se estiver em trânsito);
//   - "viagemAtual": ponteiro para a Viagem (trecho) que ele está realizando
//     agora. Se for nullptr, o passageiro está livre, parado em localAtual.
//     Caso contrário, está em trânsito.

class Passageiro {
private:
    std::string nome;
    Cidade* localAtual;
    Viagem* viagemAtual;

public:
    Passageiro(const std::string& nome, Cidade* localAtual);

    std::string getNome() const;

    Cidade* getLocalAtual() const;
    void setLocalAtual(Cidade* local);

    Viagem* getViagemAtual() const;
    void setViagemAtual(Viagem* viagem);

    // Retorna true se o passageiro NÃO está em nenhuma viagem em andamento,
    // ou seja, se ele está livre para ser escalado para uma nova viagem.
    bool estaDisponivel() const;
};

#endif
