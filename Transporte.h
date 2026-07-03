#ifndef TRANSPORTE_H
#define TRANSPORTE_H

#include <string>
#include "Cidade.h"

// Declaração adiantada (forward declaration) da classe Viagem.
// Precisamos disso porque Transporte guarda um PONTEIRO para a Viagem que
// ele está realizando no momento, mas Viagem.h também precisa incluir
// Transporte.h (pois uma Viagem guarda um Transporte*). Se cada um incluísse
// o outro diretamente, teríamos uma dependência circular que o compilador
// não consegue resolver. A solução clássica em C++ é: como aqui só
// precisamos de um PONTEIRO (não do objeto completo), basta "avisar" o
// compilador que a classe Viagem existe, sem incluir o arquivo inteiro.

class Viagem;

// ============================================================================
// Classe Transporte
// ----------------------------------------------------------------------------
// Representa um meio de transporte (navio, balsa, ônibus, trem, etc.) capaz
// de se mover entre cidades ao longo de trajetos compatíveis com o seu tipo
// (Aquático só percorre trajetos Aquáticos; Terrestre só percorre trajetos
// Terrestres).
//
// Regras de negócio representadas por esta classe:
//   - "capacidade": número máximo de passageiros que podem viajar juntos
//     nesse transporte em uma mesma viagem;
//   - "velocidade": quantos quilômetros o transporte percorre por hora de
//     simulação (usado por Viagem::avancarUmaHora);
//   - "distanciaEntreDescansos" e "tempoDeDescanso": a cada X quilômetros
//     percorridos, o transporte precisa OBRIGATORIAMENTE descansar por Y
//     horas antes de continuar a viagem (representa paradas para
//     reabastecimento, troca de tripulação, manutenção, etc.).
//
// Estado de localização:
//   - "localAtual" guarda a cidade onde o transporte está fisicamente
//     parado, OU a última cidade confirmada de onde ele partiu (caso esteja
//     em trânsito);
//   - "viagemAtual" é um ponteiro para a Viagem (trecho) que o transporte
//     está realizando agora. Se for nullptr, o transporte está livre e
//     parado em localAtual. Se não for nullptr, o transporte está em
//     trânsito, e a localização "real" durante a viagem deve ser consultada
//     através dessa Viagem (origem/destino do trecho atual).

class Transporte {
private:
    std::string nome;
    char tipo;                       // 'A' = Aquático, 'T' = Terrestre
    int capacidade;                   // Número máximo de passageiros simultâneos
    int velocidade;                   // Quilômetros percorridos por hora
    int distanciaEntreDescansos;     // Km percorridos entre duas paradas obrigatórias de descanso
    int tempoDeDescanso;              // Quantas horas dura cada parada de descanso
    Cidade* localAtual;                // Cidade atual (ou última cidade de partida, se em trânsito)
    Viagem* viagemAtual;               // Viagem (trecho) em andamento; nullptr se o transporte está livre

public:
    Transporte(const std::string& nome, char tipo, int capacidade, int velocidade,
               int distanciaEntreDescansos, int tempoDeDescanso, Cidade* localAtual);

    // leitura dos atributos
    std::string getNome() const;
    char getTipo() const;
    int getCapacidade() const;
    int getVelocidade() const;
    int getDistanciaEntreDescansos() const;
    int getTempoDeDescanso() const;

    //Localização
    Cidade* getLocalAtual() const;
    void setLocalAtual(Cidade* local);

    //Controle de viagem em andamento
    Viagem* getViagemAtual() const;
    void setViagemAtual(Viagem* viagem);

    // Retorna true se o transporte NÃO está em nenhuma viagem em andamento
    // no momento (ou seja, se viagemAtual == nullptr), e portanto pode ser
    // escalado para uma nova viagem.

    bool estaDisponivel() const;
};

#endif
