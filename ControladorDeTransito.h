#ifndef CONTROLADOR_DE_TRANSITO_H
#define CONTROLADOR_DE_TRANSITO_H

#include <vector>
#include <string>
#include "Cidade.h"
#include "Trajeto.h"
#include "Transporte.h"
#include "Passageiro.h"
#include "Viagem.h"

//   1. Ser DONA de todos os objetos do sistema (cidades, trajetos,
//      transportes, passageiros e viagens) — todos eles são alocados com
//      "new" e guardados em vetores de ponteiros aqui dentro. Por isso, o
//      destrutor desta classe é responsável por liberar (delete) toda essa
//      memória, evitando vazamentos.
//
//   2. Validar e registrar novos cadastros (cidades, trajetos, transportes,
//      passageiros).
//
//   3. Calcular o melhor caminho entre duas cidades usando o algoritmo de
//      Dijkstra sobre o grafo formado pelas cidades (vértices) e trajetos
//      (arestas), considerando apenas trajetos compatíveis com o tipo do
//      transporte escolhido.
//
//   4. Iniciar viagens — inclusive viagens COM CONEXÃO, quando não existe
//      trajeto direto entre a origem e o destino pedidos, criando uma
//      cadeia de trechos (objetos Viagem) ligados entre si.
//
//   5. Avançar o tempo do sistema hora a hora (avancarHoras), atualizando
//      cada viagem em andamento, encadeando automaticamente para o próximo
//      trecho quando um trecho termina, e efetivando a chegada final
//      (movendo passageiros e transporte) quando o ÚLTIMO trecho de uma
//      cadeia é concluído.
//
//   6. Gerar todos os relatórios exigidos pelo trabalho.
//
//   7. Persistir (salvar) e recarregar (carregar) todos os dados do sistema
//      em arquivos de texto simples, permitindo que o sistema "lembre" de
//      tudo ao ser reiniciado.

class ControladorDeTransito {
private:
    std::vector<Cidade*> cidades;
    std::vector<Trajeto*> trajetos;
    std::vector<Transporte*> transportes;
    std::vector<Passageiro*> passageiros;
    std::vector<Viagem*> viagens; // contém TODOS os trechos de TODAS as cadeias de viagem já criadas

    int proximoIdViagem; // contador interno usado para gerar IDs únicos e crescentes para cada trecho (Viagem)

    // ------------------------------------------------------------------
    // Funções auxiliares privadas (usadas internamente pela própria classe)
    // ------------------------------------------------------------------

    // Procura, em `cidades`, a cidade cujo nome seja exatamente `nome`.
    // Retorna nullptr se nenhuma cidade com esse nome estiver cadastrada.
    Cidade* buscarCidade(const std::string& nome) const;

    // Procura, em `transportes`, o transporte cujo nome seja exatamente `nome`.
    Transporte* buscarTransporte(const std::string& nome) const;

    // Procura, em `passageiros`, o passageiro cujo nome seja exatamente `nome`.
    Passageiro* buscarPassageiro(const std::string& nome) const;

    // Calcula o caminho de MENOR DISTÂNCIA TOTAL entre `origem` e `destino`,
    // usando apenas Trajetos cujo tipo seja igual a `tipoTransporte`.
    // Implementa o algoritmo de Dijkstra(segundo o google ele o A* são os mais utilizados para calculos de rotas de viagens)
    // vértices são as cidades cadastradas e cujas arestas são os trajetos
    // cadastrados (com peso = distância em km).
    // Retorna a sequência ORDENADA de Trajeto* que deve ser percorrida, do
    // primeiro ao último. Se origem == destino ou se não existir NENHUM
    // caminho possível, retorna um vetor vazio.
    std::vector<Trajeto*> calcularMelhorTrajeto(Cidade* origem, Cidade* destino, char tipoTransporte) const;

    // Converte uma lista de nomes de passageiros (strings) nos ponteiros
    // Passageiro* correspondentes, validando que:
    //   - cada nome corresponde a um passageiro realmente cadastrado;
    //   - cada passageiro está disponível (não está em outra viagem);
    //   - cada passageiro está fisicamente na cidade de origem informada.
    // Em caso de qualquer problema, preenche `erro` com uma mensagem
    // explicativa e retorna false; caso contrário, preenche `out` com os
    // ponteiros resolvidos e retorna true.

    bool resolverPassageiros(const std::vector<std::string>& nomes, Cidade* origem,
                              std::vector<Passageiro*>& out, std::string& erro) const;

    // Chamada quando o ÚLTIMO trecho de uma cadeia de viagem é concluído.
    // Move fisicamente o transporte e todos os passageiros envolvidos para
    // a cidade de destino final da cadeia, libera o transporte e os
    // passageiros (voltando a ficar disponíveis) e incrementa o contador de
    // visitas da cidade de destino.
    void finalizarCadeia(Viagem* ultimoTrecho);

public:
    ControladorDeTransito();

    // O destrutor libera TODA a memória alocada dinamicamente pelo sistema
    // ao longo da execução (cidades, trajetos, transportes, passageiros e
    // viagens), já que esta classe é a única "dona" desses objetos.
    ~ControladorDeTransito();

    // Cada método de cadastro retorna `true` em caso de sucesso e `false`
    // se algum dado for inválido (nome duplicado, cidade inexistente, tipo
    // inválido, valores numéricos não positivos, etc.), imprimindo uma
    // mensagem de erro explicativa no próprio processo.

    bool cadastrarCidade(const std::string& nome);

    bool cadastrarTrajeto(const std::string& nomeOrigem, const std::string& nomeDestino,
                           char tipo, int distancia);

    bool cadastrarTransporte(const std::string& nome, char tipo, int capacidade, int velocidade,
                              int distanciaEntreDescansos, int tempoDeDescanso,
                              const std::string& localAtual);

    bool cadastrarPassageiro(const std::string& nome, const std::string& localAtual);


    // Tenta iniciar uma viagem entre `nomeOrigem` e `nomeDestino`, usando o
    // transporte `nomeTransporte` e os passageiros de `nomesPassageiros`.
    // Realiza todas as validações necessárias (existência das entidades,
    // disponibilidade do transporte e dos passageiros, transporte estar na
    // cidade de origem, capacidade suficiente, existência de algum caminho
    // possível — direto ou com conexão). Se tudo estiver certo, cria a
    // cadeia de trechos (um ou mais objetos Viagem) e inicia o primeiro
    // trecho imediatamente. Retorna true em caso de sucesso, false caso
    // contrário (imprimindo o motivo do erro).
    bool iniciarViagem(const std::string& nomeTransporte, const std::vector<std::string>& nomesPassageiros,
                        const std::string& nomeOrigem, const std::string& nomeDestino);

    // Avança `horas` horas na simulação do sistema. Para cada hora, percorre
    // todas as viagens (trechos) atualmente EM_ANDAMENTO e chama
    // Viagem::avancarUmaHora(). Quando um trecho é concluído:
    //   - se existir um próximo trecho na cadeia, ele é iniciado
    //     automaticamente (passa a ser simulado a partir da próxima hora);
    //   - se NÃO existir próximo trecho (ou seja, este era o último trecho
    //     da cadeia), a chegada final é efetivada através de
    //     finalizarCadeia().
    void avancarHoras(int horas);


    // Lista cada passageiro cadastrado, informando se ele está parado em
    // alguma cidade ou em trânsito (e, nesse caso, informando origem,
    // destino e transporte usado, como pede o enunciado).
    void relatarPosicaoPassageiros() const;

    // Mesma ideia acima, mas para os transportes cadastrados.
    void relatarPosicaoTransportes() const;

    // Lista todos os trechos de viagem atualmente EM_ANDAMENTO.
    void relatarViagensEmAndamento() const;

    // Lista todas as cidades cadastradas, ordenadas da mais visitada para a
    // menos visitada (funcionalidade obrigatória "cidades mais visitadas").
    void relatarCidadesMaisVisitadas() const;

    // Relatórios simples de apoio (não obrigatórios, mas úteis para o
    // usuário conferir o que já foi cadastrado no sistema).
    void relatarCidades() const;
    void relatarTrajetos() const;



    // Salva TODO o estado atual do sistema (cidades, trajetos, transportes,
    // passageiros e viagens, incluindo viagens em andamento com seu
    // progresso exato) em arquivos de texto dentro da pasta `dados`,
    // criando essa pasta caso ela ainda não exista.

    void salvarDados(const std::string& pasta) const;

    // Carrega todo o estado do sistema a partir dos arquivos de texto
    // salvos anteriormente na pasta `pasta`. Caso os arquivos não existam
    // (por exemplo, na primeira execução do programa), o sistema
    // simplesmente permanece vazio, sem gerar erro fatal.
    void carregarDados(const std::string& pasta);
};

#endif
