#ifndef VIAGEM_H
#define VIAGEM_H

#include <vector>
#include <string>
#include "Cidade.h"
#include "Transporte.h"
#include "Passageiro.h"

// Estados possíveis de um TRECHO de viagem (um único objeto Viagem).

enum class EstadoViagem {
    NAO_INICIADA = 0, // Trecho criado, mas ainda aguardando o trecho anterior da cadeia terminar
    EM_ANDAMENTO = 1, // Trecho sendo simulado hora a hora neste momento
    CONCLUIDA    = 2  // Trecho já chegou ao seu destino (destino DESSE trecho, não necessariamente o final)
};

// ============================================================================
// Classe Viagem:
// Representa um TRECHO de deslocamento entre duas cidades, usando um
// transporte específico e levando uma lista de passageiros.
//
// >>> Viagens com conexão <<<
// Quando não existe um Trajeto direto entre a cidade de origem e a cidade de
// destino solicitadas pelo usuário, mas existe um caminho passando por
// cidades intermediárias (ex.: A -> X -> Y -> B), o sistema cria uma CADEIA
// de objetos Viagem, um para cada trecho do caminho (A->X, X->Y, Y->B),
// ligados entre si pelo ponteiro "proxima":
//
//      [Viagem A->X] --proxima--> [Viagem X->Y] --proxima--> [Viagem Y->B] --proxima--> nullptr
//
// Somente o PRIMEIRO trecho da cadeia é iniciado imediatamente
// (estado = EM_ANDAMENTO). Os demais ficam com estado = NAO_INICIADA até que
// o trecho anterior termine — nesse momento, o ControladorDeTransito (dentro
// de avancarHoras) inicia automaticamente o próximo trecho da cadeia.
//
// Cada trecho guarda dois pares de cidades:
//   - origem / destino: as cidades DESTE trecho específico (ex.: X e Y);
//   - origemGeral / destinoGeral: a origem e o destino da viagem COMPLETA
//     que o usuário pediu (ex.: A e B). Esses dois ponteiros são IGUAIS em
//     todos os trechos de uma mesma cadeia, e servem apenas para que os
//     relatórios consigam informar "passageiro está em trânsito de A para B",
//     mesmo que o trecho atual seja, por exemplo, X->Y.
//
// >>> Regra de ouro dos trechos conectados <<<
// Passageiros e transporte só são fisicamente movidos para a cidade de
// destino quando o ÚLTIMO trecho da cadeia é concluído (é assim que o
// enunciado do trabalho define o comportamento). Essa movimentação final é
// feita pelo ControladorDeTransito (método finalizarCadeia), não pela
// própria classe Viagem — a Viagem apenas simula o avanço hora a hora e
// informa quando chegou ao FIM DO TRECHO

class Viagem {
private:
    int id;                              // Identificador único deste trecho (usado na persistência em disco)
    Transporte* transporte;              // Transporte usado neste trecho (e em toda a cadeia)
    std::vector<Passageiro*> passageiros; // Passageiros embarcados neste trecho (e em toda a cadeia)

    Cidade* origem;                      // Origem DESTE trecho específico
    Cidade* destino;                     // Destino DESTE trecho específico
    Cidade* origemGeral;                 // Origem da viagem completa solicitada (igual em toda a cadeia)
    Cidade* destinoGeral;                // Destino da viagem completa solicitada (igual em toda a cadeia)

    int distanciaTotal;                  // Distância (km) do Trajeto usado neste trecho
    int distanciaPercorrida;             // Quantos km já foram percorridos neste trecho até agora
    int distanciaDesdeUltimoDescanso;    // Km percorridos desde a última parada obrigatória de descanso
    int horasDescansoRestante;           // Quantas horas de descanso ainda faltam (0 = não está descansando)

    EstadoViagem estado;                 // Estado atual do trecho
    Viagem* proxima;                     // Próximo trecho da cadeia (nullptr se este for o último/único trecho)

public:
    Viagem(int id, Transporte* transporte, std::vector<Passageiro*> passageiros,
           Cidade* origem, Cidade* destino, Cidade* origemGeral, Cidade* destinoGeral,
           int distanciaTotal);

    // Inicia (ou reinicia, no caso de continuação de cadeia) este trecho:
    // muda o estado para EM_ANDAMENTO e associa este trecho como a
    // "viagemAtual" do transporte e de cada passageiro envolvido — é assim
    // que o resto do sistema sabe que eles estão em trânsito

    void iniciarViagem();

    // Simula EXATAMENTE UMA HORA de deslocamento deste trecho:
    //   1. Se o transporte está em período de descanso obrigatório, apenas
    //      consome uma hora desse descanso (e não anda).
    //   2. Caso contrário, avança a distância percorrida de acordo com a
    //      velocidade do transporte.
    //   3. Se a distância percorrida atingir a distância total do trecho,
    //      o estado muda para CONCLUIDA (chegada ao fim DESTE trecho).
    //   4. Caso ainda não tenha chegado, verifica se a distância percorrida
    //      desde o último descanso atingiu o limite do transporte; se sim,
    //      inicia um novo período de descanso obrigatório.
    // Não faz nada se o trecho não estiver com estado EM_ANDAMENTO

    void avancarUmaHora();

    // Monta uma string com uma descrição legível do estado atual do trecho
    // (usada no relatório de "viagens em andamento")

    std::string relatarEstado() const;

    bool isEmAndamento() const;
    bool isConcluida() const;
    bool isNaoIniciada() const;

    // ---- Getters ----
    int getId() const;
    Transporte* getTransporte() const;
    const std::vector<Passageiro*>& getPassageiros() const;
    Cidade* getOrigem() const;
    Cidade* getDestino() const;
    Cidade* getOrigemGeral() const;
    Cidade* getDestinoGeral() const;
    int getDistanciaTotal() const;
    int getDistanciaPercorrida() const;
    int getHorasDescansoRestante() const;
    int getDistanciaDesdeUltimoDescanso() const;
    EstadoViagem getEstado() const;

    Viagem* getProxima() const;
    void setProxima(Viagem* proxima);

    // Usado exclusivamente por ControladorDeTransito::carregarDados, para
    // restaurar o progresso exato (distância percorrida, descanso restante
    // etc.) de um trecho que já estava em andamento quando os dados foram
    // salvos anteriormente.

    void restaurarEstado(EstadoViagem estado, int distanciaPercorrida,
                          int horasDescansoRestante, int distanciaDesdeUltimoDescanso);
};

#endif
