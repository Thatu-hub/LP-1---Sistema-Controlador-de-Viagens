#ifndef TRAJETO_H
#define TRAJETO_H

#include "Cidade.h"

// Representa uma ligação DIRETA e DIRECIONADA entre duas cidades
// (origem -> destino), com um tipo de trajeto (Aquático ou Terrestre) e uma
// distância em quilômetros.
//
// Um Trajeto é, em termos de teoria dos grafos, uma "aresta" do grafo cujos
// "vértices" são as cidades cadastradas no sistema. É esse grafo que o
// ControladorDeTransito percorre (usando o algoritmo de Dijkstra) para
// calcular o melhor caminho entre duas cidades quaisquer — seja um caminho
// direto (um único Trajeto) ou um caminho com conexão (vários Trajetos
// encadeados através de cidades intermediárias).
//
// Observação de projeto: o Trajeto é armazenado como via de MÃO ÚNICA
// (origem -> destino). Se o usuário quiser permitir também o caminho
// inverso (destino -> origem), ele deve cadastrar outro Trajeto separado.
// Essa escolha reflete situações do mundo real em que nem toda rota é
// bidirecional (por exemplo, rotas marítimas com correntes favoráveis em
// apenas um sentido, ou vias terrestres de mão única).

class Trajeto {
private:
    Cidade* origem;   // Ponteiro para a cidade de origem. A classe Trajeto
                        // NÃO é dona desse objeto (não faz "delete" dele);
                        // quem possui e libera as Cidades é o
                        // ControladorDeTransito.
    Cidade* destino;  // Ponteiro para a cidade de destino (mesma observação acima).
    char tipo;          // 'A' para Aquático, 'T' para Terrestre.
    int distancia;       // Distância do trajeto, em quilômetros. Deve ser > 0.

public:
    // Construtor: recebe os ponteiros de origem e destino (já validados e
    // já existentes no sistema), o tipo do trajeto e a distância.
    Trajeto(Cidade* origem, Cidade* destino, char tipo, int distancia);

    Cidade* getOrigem() const;
    Cidade* getDestino() const;
    char getTipo() const;
    int getDistancia() const;
};

#endif // TRAJETO_H
