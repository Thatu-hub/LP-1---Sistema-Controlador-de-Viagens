#include "ControladorDeTransito.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <climits>
#include <utility>
#include <sys/stat.h>
#include <filesystem>

ControladorDeTransito::ControladorDeTransito() : proximoIdViagem(1) {

    // Nenhum dado é criado aqui: o sistema começa totalmente vazio, e é
    // responsabilidade do main() chamar carregarDados() logo em seguida,
    // caso existam dados salvos de uma execução anterior.
}

ControladorDeTransito::~ControladorDeTransito() {

    // Como este é o único "dono" de todos os objetos do sistema (todos
    // foram criados com "new" nos métodos de cadastro / iniciarViagem /
    // carregarDados), é aqui que devemos devolver essa memória ao sistema
    // operacional, usando "delete" em cada ponteiro guardado nos vetores.
    //
    // A ORDEM não importa neste caso, pois nenhum destrutor de Cidade,
    // Trajeto, Transporte, Passageiro ou Viagem tenta acessar outro objeto
    // durante sua destruição (eles só guardam ponteiros "de referência",
    // não são donos uns dos outros).

    for (Viagem* v : viagens)         delete v;
    for (Passageiro* p : passageiros) delete p;
    for (Transporte* t : transportes) delete t;
    for (Trajeto* tj : trajetos)      delete tj;
    for (Cidade* c : cidades)         delete c;
}

// Todas seguem o mesmo padrão simples: percorrem o vetor correspondente
// comparando o nome, e retornam o ponteiro encontrado (ou nullptr). Para os
// tamanhos de dados esperados neste trabalho (dezenas/centenas de
// cidades/transportes/passageiros), uma busca linear O(n) é mais do que
// suficiente e mantém o código simples de entender.


Cidade* ControladorDeTransito::buscarCidade(const std::string& nome) const {
    for (Cidade* c : cidades) {
        if (c->getNome() == nome) return c;
    }
    return nullptr;
}

Transporte* ControladorDeTransito::buscarTransporte(const std::string& nome) const {
    for (Transporte* t : transportes) {
        if (t->getNome() == nome) return t;
    }
    return nullptr;
}

Passageiro* ControladorDeTransito::buscarPassageiro(const std::string& nome) const {
    for (Passageiro* p : passageiros) {
        if (p->getNome() == nome) return p;
    }
    return nullptr;
}

// Cadastros

bool ControladorDeTransito::cadastrarCidade(const std::string& nome) {
    if (nome.empty()) {
        std::cout << "Erro: o nome da cidade nao pode ser vazio.\n";
        return false;
    }
    if (buscarCidade(nome) != nullptr) {
        std::cout << "Erro: ja existe uma cidade chamada \"" << nome << "\".\n";
        return false;
    }
    cidades.push_back(new Cidade(nome));
    std::cout << "Cidade \"" << nome << "\" cadastrada com sucesso.\n";
    return true;
}

bool ControladorDeTransito::cadastrarTrajeto(const std::string& nomeOrigem, const std::string& nomeDestino,
                                              char tipo, int distancia) {
    Cidade* origem = buscarCidade(nomeOrigem);
    Cidade* destino = buscarCidade(nomeDestino);

    if (origem == nullptr) {
        std::cout << "Erro: a cidade de origem \"" << nomeOrigem << "\" nao esta cadastrada.\n";
        return false;
    }
    if (destino == nullptr) {
        std::cout << "Erro: a cidade de destino \"" << nomeDestino << "\" nao esta cadastrada.\n";
        return false;
    }
    if (origem == destino) {
        std::cout << "Erro: a origem e o destino de um trajeto nao podem ser a mesma cidade.\n";
        return false;
    }
    if (tipo != 'A' && tipo != 'T') {
        std::cout << "Erro: tipo de trajeto invalido (use 'A' para Aquatico ou 'T' para Terrestre).\n";
        return false;
    }
    if (distancia <= 0) {
        std::cout << "Erro: a distancia do trajeto deve ser maior que zero.\n";
        return false;
    }

    trajetos.push_back(new Trajeto(origem, destino, tipo, distancia));
    std::cout << "Trajeto " << nomeOrigem << " -> " << nomeDestino
               << " (" << (tipo == 'A' ? "Aquatico" : "Terrestre") << ", " << distancia
               << " km) cadastrado com sucesso.\n";
    return true;
}

bool ControladorDeTransito::cadastrarTransporte(const std::string& nome, char tipo, int capacidade,
                                                 int velocidade, int distanciaEntreDescansos,
                                                 int tempoDeDescanso, const std::string& localAtual) {
    if (nome.empty()) {
        std::cout << "Erro: o nome do transporte nao pode ser vazio.\n";
        return false;
    }
    if (buscarTransporte(nome) != nullptr) {
        std::cout << "Erro: ja existe um transporte chamado \"" << nome << "\".\n";
        return false;
    }
    Cidade* local = buscarCidade(localAtual);
    if (local == nullptr) {
        std::cout << "Erro: a cidade \"" << localAtual << "\" nao esta cadastrada.\n";
        return false;
    }
    if (tipo != 'A' && tipo != 'T') {
        std::cout << "Erro: tipo de transporte invalido (use 'A' para Aquatico ou 'T' para Terrestre).\n";
        return false;
    }
    if (capacidade <= 0) {
        std::cout << "Erro: a capacidade de passageiros deve ser maior que zero.\n";
        return false;
    }
    if (velocidade <= 0) {
        std::cout << "Erro: a velocidade deve ser maior que zero.\n";
        return false;
    }
    if (distanciaEntreDescansos < 0 || tempoDeDescanso < 0) {
        std::cout << "Erro: distancia entre descansos e tempo de descanso nao podem ser negativos.\n";
        return false;
    }

    transportes.push_back(new Transporte(nome, tipo, capacidade, velocidade,
                                          distanciaEntreDescansos, tempoDeDescanso, local));
    std::cout << "Transporte \"" << nome << "\" (" << (tipo == 'A' ? "Aquatico" : "Terrestre")
               << ") cadastrado com sucesso em " << localAtual << ".\n";
    return true;
}

bool ControladorDeTransito::cadastrarPassageiro(const std::string& nome, const std::string& localAtual) {
    if (nome.empty()) {
        std::cout << "Erro: o nome do passageiro nao pode ser vazio.\n";
        return false;
    }
    if (buscarPassageiro(nome) != nullptr) {
        std::cout << "Erro: ja existe um passageiro chamado \"" << nome << "\".\n";
        return false;
    }
    Cidade* local = buscarCidade(localAtual);
    if (local == nullptr) {
        std::cout << "Erro: a cidade \"" << localAtual << "\" nao esta cadastrada.\n";
        return false;
    }

    passageiros.push_back(new Passageiro(nome, local));
    std::cout << "Passageiro \"" << nome << "\" cadastrado com sucesso em " << localAtual << ".\n";
    return true;
}

// ============================================================================
// Cálculo do melhor trajeto: algoritmo de Dijkstra
//   - cada Cidade cadastrada é um VÉRTICE do grafo;
//   - cada Trajeto cadastrado (cujo tipo seja compatível com o transporte)
//     é uma ARESTA direcionada, cujo peso é a distância em quilômetros.
//
// Como todos os pesos (distâncias) são positivos, o algoritmo de Dijkstra é
// perfeitamente adequado e garante encontrar o caminho de menor distância
// total entre a origem e o destino, se ele existir.
// ============================================================================

std::vector<Trajeto*> ControladorDeTransito::calcularMelhorTrajeto(Cidade* origem, Cidade* destino,
                                                                     char tipoTransporte) const {
    const int n = static_cast<int>(cidades.size());

    // Função auxiliar (lambda) que converte um ponteiro de Cidade no seu índice dentro do vetor `cidades`. Precisamos disso porque o Dijkstra
    // trabalha com vetores indexados por inteiros (0..n-1), o que
    // é mais simples e eficiente do que usar um mapa de ponteiro para
    // distância.

    auto indiceDe = [&](Cidade* c) -> int {
        for (int i = 0; i < n; i++) {
            if (cidades[i] == c) return i;
        }
        return -1;
    };

    const int origemIdx = indiceDe(origem);
    const int destinoIdx = indiceDe(destino);
    if (origemIdx == -1 || destinoIdx == -1 || origemIdx == destinoIdx) {
        // Cidade inválida (não deveria acontecer, pois já validamos antes de chamar esta função) ou origem igual ao destino.
        return {};
    }

    const int INFINITO = INT_MAX;

    // dist[i]        = menor distância conhecida, até agora, da origem até a cidade i.
    // trajetoUsado[i] = qual Trajeto* foi usado para CHEGAR à cidade i pelo caminho mais curto conhecido.
    // visitado[i]     = true se já finalizamos definitivamente o cálculo da menor distância até a cidade i.
    std::vector<int> dist(n, INFINITO);
    std::vector<Trajeto*> trajetoUsado(n, nullptr);
    std::vector<bool> visitado(n, false);

    dist[origemIdx] = 0;

    // Fila de prioridade (min-heap) de pares (distância acumulada, índice
    // da cidade), para sempre processarmos primeiro a cidade cuja menor
    // distância conhecida até agora é a mais curta — esse é o coração do
    // algoritmo de Dijkstra.
    using ParDistIndice = std::pair<int, int>;
    std::priority_queue<ParDistIndice, std::vector<ParDistIndice>, std::greater<ParDistIndice>> fila;
    fila.push({0, origemIdx});

    while (!fila.empty()) {
        auto [distAtual, u] = fila.top();
        fila.pop();

        // Pode haver entradas "desatualizadas" na fila (a mesma cidade
        // inserida mais de uma vez, com distâncias diferentes, por causa de
        // relaxamentos sucessivos). Se já processamos essa cidade
        // definitivamente, ignoramos esta entrada antiga.
        if (visitado[u]) continue;
        visitado[u] = true;

        // Otimização: se já chegamos à cidade de destino com a menor
        // distância possível, não precisamos continuar explorando o resto
        // do grafo.
        if (u == destinoIdx) break;

        // "Relaxamento" das arestas que saem da cidade u: para cada
        // Trajeto compatível que parte de cidades[u], verificamos se
        // passar por ele resulta em um caminho mais curto até o destino
        // desse trajeto do que o conhecido até agora.
        for (Trajeto* tj : trajetos) {
            if (tj->getTipo() != tipoTransporte) continue;        // ignora trajetos de tipo incompatível
            if (tj->getOrigem() != cidades[u]) continue;           // só nos interessam arestas que SAEM de u

            int v = indiceDe(tj->getDestino());
            if (v == -1) continue; // segurança extra (não deveria acontecer)

            int novaDistancia = dist[u] + tj->getDistancia();
            if (novaDistancia < dist[v]) {
                dist[v] = novaDistancia;
                trajetoUsado[v] = tj;
                fila.push({novaDistancia, v});
            }
        }
    }

    // Se a distância até o destino continua "infinita", significa que não
    // existe NENHUM caminho (direto ou com conexão) usando trajetos do tipo
    // do transporte escolhido.

    if (dist[destinoIdx] == INFINITO) {
        return {};
    }

    // Reconstrução do caminho: partimos do destino e "voltamos" seguindo,
    // para cada cidade, qual foi o trajeto usado para chegar até ela,
    // acumulando esses trajetos em ordem REVERSA (do destino até a
    // origem). No final, invertemos o vetor para obter a ordem correta
    // (da origem até o destino).
    std::vector<Trajeto*> caminho;
    int atual = destinoIdx;
    while (atual != origemIdx) {
        Trajeto* tj = trajetoUsado[atual];
        caminho.push_back(tj);
        atual = indiceDe(tj->getOrigem());
    }
    std::reverse(caminho.begin(), caminho.end());

    return caminho;
}

// Resolução (validação) da lista de passageiros de uma viagem

bool ControladorDeTransito::resolverPassageiros(const std::vector<std::string>& nomes, Cidade* origem,
                                                  std::vector<Passageiro*>& out, std::string& erro) const {
    out.clear();
    for (const std::string& nome : nomes) {
        Passageiro* p = buscarPassageiro(nome);
        if (p == nullptr) {
            erro = "o passageiro \"" + nome + "\" nao esta cadastrado.";
            return false;
        }
        if (!p->estaDisponivel()) {
            erro = "o passageiro \"" + nome + "\" ja esta em outra viagem.";
            return false;
        }
        if (p->getLocalAtual() != origem) {
            erro = "o passageiro \"" + nome + "\" nao está na cidade de origem (" + origem->getNome() + ").";
            return false;
        }
        out.push_back(p);
    }
    return true;
}

// Gestão de viagens

bool ControladorDeTransito::iniciarViagem(const std::string& nomeTransporte,
                                           const std::vector<std::string>& nomesPassageiros,
                                           const std::string& nomeOrigem, const std::string& nomeDestino) {
    // ---- 1. Validação das cidades de origem e destino ----
    Cidade* origem = buscarCidade(nomeOrigem);
    Cidade* destino = buscarCidade(nomeDestino);
    if (origem == nullptr) {
        std::cout << "Erro: a cidade de origem \"" << nomeOrigem << "\" nao esta cadastrada.\n";
        return false;
    }
    if (destino == nullptr) {
        std::cout << "Erro: a cidade de destino \"" << nomeDestino << "\" nao esta cadastrada.\n";
        return false;
    }
    if (origem == destino) {
        std::cout << "Erro: a origem e o destino da viagem nao podem ser a mesma cidade.\n";
        return false;
    }

    // ---- 2. Validação do transporte ----
    Transporte* transporte = buscarTransporte(nomeTransporte);
    if (transporte == nullptr) {
        std::cout << "Erro: o transporte \"" << nomeTransporte << "\" nao esta cadastrado.\n";
        return false;
    }
    if (!transporte->estaDisponivel()) {
        std::cout << "Erro: o transporte \"" << nomeTransporte << "\" ja esta em outra viagem.\n";
        return false;
    }
    if (transporte->getLocalAtual() != origem) {
        std::cout << "Erro: o transporte \"" << nomeTransporte << "\" nao esta na cidade de origem ("
                   << nomeOrigem << "). Ele esta atualmente em "
                   << transporte->getLocalAtual()->getNome() << ".\n";
        return false;
    }
    if (static_cast<int>(nomesPassageiros.size()) > transporte->getCapacidade()) {
        std::cout << "Erro: a quantidade de passageiros (" << nomesPassageiros.size()
                   << ") excede a capacidade do transporte \"" << nomeTransporte << "\" ("
                   << transporte->getCapacidade() << ").\n";
        return false;
    }

    // ---- 3. Validação dos passageiros ----
    std::vector<Passageiro*> passageirosResolvidos;
    std::string erro;
    if (!resolverPassageiros(nomesPassageiros, origem, passageirosResolvidos, erro)) {
        std::cout << "Erro: " << erro << "\n";
        return false;
    }

    // ---- 4. Cálculo do melhor caminho (direto ou com conexão) ----
    std::vector<Trajeto*> caminho = calcularMelhorTrajeto(origem, destino, transporte->getTipo());
    if (caminho.empty()) {
        std::cout << "Erro: nao existe nenhum trajeto (direto ou com conexão atraves de cidades "
                   << "intermediarias) compativel com o tipo do transporte \"" << nomeTransporte
                   << "\" entre " << nomeOrigem << " e " << nomeDestino << ".\n";
        return false;
    }

    // ---- 5. Criação da cadeia de trechos (um objeto Viagem por Trajeto do caminho) ----
    std::vector<Viagem*> novosTrechos;
    novosTrechos.reserve(caminho.size());
    for (Trajeto* tj : caminho) {
        Viagem* trecho = new Viagem(proximoIdViagem++, transporte, passageirosResolvidos,
                                     tj->getOrigem(), tj->getDestino(), origem, destino,
                                     tj->getDistancia());
        novosTrechos.push_back(trecho);
        viagens.push_back(trecho); // o controlador passa a "possuir" este novo trecho
    }

    // Encadeia cada trecho ao seu sucessor: trecho[i].proxima = trecho[i+1].
    // O último trecho mantém proxima == nullptr (valor padrão do construtor
    // de Viagem), indicando que, ao ser concluído, a cadeia inteira termina.
    for (std::size_t i = 0; i + 1 < novosTrechos.size(); i++) {
        novosTrechos[i]->setProxima(novosTrechos[i + 1]);
    }

    // ---- 6. Início efetivo da viagem: apenas o PRIMEIRO trecho começa agora ----
    // Os demais trechos (se houver) permanecem NAO_INICIADA e serão
    // automaticamente iniciados por avancarHoras(), assim que o trecho
    // anterior da cadeia for concluído.
    novosTrechos.front()->iniciarViagem();

    // ---- 7. Mensagem de confirmação para o usuário ----
    if (novosTrechos.size() == 1) {
        std::cout << "Viagem iniciada: " << nomeOrigem << " -> " << nomeDestino
                   << " usando \"" << nomeTransporte << "\" (" << caminho[0]->getDistancia()
                   << " km, trajeto direto).\n";
    } else {
        std::cout << "Viagem com conexao iniciada (" << novosTrechos.size() << " trechos): "
                   << nomeOrigem;
        for (Trajeto* tj : caminho) {
            std::cout << " -> " << tj->getDestino()->getNome();
        }
        std::cout << ", usando \"" << nomeTransporte << "\".\n";
    }
    return true;
}

void ControladorDeTransito::finalizarCadeia(Viagem* ultimoTrecho) {
    Transporte* transporte = ultimoTrecho->getTransporte();
    Cidade* destinoFinal = ultimoTrecho->getDestinoGeral();

    // Move fisicamente o transporte para a cidade de destino final da
    // cadeia e o libera, deixando-o disponível para novas viagens.
    transporte->setLocalAtual(destinoFinal);
    transporte->setViagemAtual(nullptr);

    // Faz o mesmo para cada passageiro que estava embarcado nesta cadeia de
    // viagens.
    for (Passageiro* p : ultimoTrecho->getPassageiros()) {
        p->setLocalAtual(destinoFinal);
        p->setViagemAtual(nullptr);
    }

    // Registra que a cidade de destino recebeu mais uma "visita" — usado no
    // relatório de cidades mais visitadas. Note que apenas o DESTINO FINAL
    // da cadeia é contabilizado, nunca as cidades intermediárias por onde a
    // viagem passou.
    destinoFinal->incrementarVisita();

    std::cout << "Chegada confirmada: o transporte \"" << transporte->getNome()
               << "\" e seus passageiros chegaram a " << destinoFinal->getNome() << ".\n";
}

void ControladorDeTransito::avancarHoras(int horas) {
    if (horas <= 0) {
        std::cout << "Erro: o numero de horas a avançar deve ser maior que zero.\n";
        return;
    }

    // Simulamos a passagem do tempo HORA A HORA. Isso é essencial para que
    // as regras de descanso obrigatório (que dependem de quantos km foram
    // percorridos "até agora") sejam respeitadas corretamente, mesmo quando
    // o usuário pede para avançar várias horas de uma só vez.
    for (int h = 0; h < horas; h++) {
        // Percorremos o vetor `viagens`, que contém TODOS os trechos já
        // criados (de todas as cadeias, concluídos ou não). Para cada
        // trecho que esteja realmente em andamento, simulamos uma hora.
        for (Viagem* v : viagens) {
            if (!v->isEmAndamento()) {
                continue; // trechos NAO_INICIADA ou já CONCLUIDA não são afetados
            }

            v->avancarUmaHora();

            // Se, com essa hora, o trecho acabou de ser concluído,
            // precisamos decidir o que fazer a seguir:
            if (v->isConcluida()) {
                Viagem* proxima = v->getProxima();
                if (proxima == nullptr) {
                    // Este era o ÚLTIMO trecho da cadeia: a viagem completa
                    // termina aqui, e efetivamos a chegada física dos
                    // passageiros e do transporte ao destino final.
                    finalizarCadeia(v);
                } else {
                    // Ainda existem trechos pela frente na cadeia: iniciamos
                    // automaticamente o próximo trecho. Ele passará a ser
                    // simulado a partir da PRÓXIMA hora do laço (o trecho
                    // anterior já "gastou" a hora atual chegando ao seu
                    // próprio destino).
                    proxima->iniciarViagem();
                    std::cout << "Trecho concluido (" << v->getOrigem()->getNome() << " -> "
                               << v->getDestino()->getNome() << "). Iniciando o proximo trecho: "
                               << proxima->getOrigem()->getNome() << " -> "
                               << proxima->getDestino()->getNome() << ".\n";
                }
            }
        }
    }

    std::cout << horas << " hora(s) avancada(s) com sucesso. Estado do sistema atualizado.\n";
}

// Relatórios / consultas

void ControladorDeTransito::relatarPosicaoPassageiros() const {
    std::cout << "\n=== Posicao dos Passageiros ===\n";
    if (passageiros.empty()) {
        std::cout << "Nenhum passageiro cadastrado.\n";
        return;
    }
    for (Passageiro* p : passageiros) {
        std::cout << "- " << p->getNome() << ": ";
        Viagem* v = p->getViagemAtual();
        if (v == nullptr) {
            // Passageiro parado: informamos apenas a cidade onde ele está.
            std::cout << "na cidade " << p->getLocalAtual()->getNome() << ".\n";
        } else {
            // Passageiro em trânsito: informamos a viagem GERAL (origem e
            // destino finais solicitados pelo usuário), o transporte usado
            // e, como informação extra, qual é o trecho específico sendo
            // percorrido agora (relevante em viagens com conexão).
            std::cout << "em transito de " << v->getOrigemGeral()->getNome() << " para "
                       << v->getDestinoGeral()->getNome() << ", usando \"" << v->getTransporte()->getNome()
                       << "\" (trecho atual: " << v->getOrigem()->getNome() << " -> "
                       << v->getDestino()->getNome() << ").\n";
        }
    }
}

void ControladorDeTransito::relatarPosicaoTransportes() const {
    std::cout << "\n=== Posicao dos Transportes ===\n";
    if (transportes.empty()) {
        std::cout << "Nenhum transporte cadastrado.\n";
        return;
    }
    for (Transporte* t : transportes) {
        std::cout << "- " << t->getNome() << ": ";
        Viagem* v = t->getViagemAtual();
        if (v == nullptr) {
            std::cout << "na cidade " << t->getLocalAtual()->getNome() << ".\n";
        } else {
            std::cout << "em transito de " << v->getOrigemGeral()->getNome() << " para "
                       << v->getDestinoGeral()->getNome() << " (trecho atual: "
                       << v->getOrigem()->getNome() << " -> " << v->getDestino()->getNome() << ").\n";
        }
    }
}

void ControladorDeTransito::relatarViagensEmAndamento() const {
    std::cout << "\n=== Viagens em Andamento ===\n";
    bool encontrouAlguma = false;
    for (Viagem* v : viagens) {
        if (v->isEmAndamento()) {
            std::cout << "- " << v->relatarEstado() << "\n";
            encontrouAlguma = true;
        }
    }
    if (!encontrouAlguma) {
        std::cout << "Nenhuma viagem em andamento no momento.\n";
    }
}

void ControladorDeTransito::relatarCidadesMaisVisitadas() const {
    std::cout << "\n=== Cidades Mais Visitadas ===\n";
    if (cidades.empty()) {
        std::cout << "Nenhuma cidade cadastrada.\n";
        return;
    }

    // Copiamos o vetor de PONTEIROS (não copiamos os objetos Cidade em si,
    // apenas os endereços) para podermos ordenar sem alterar a ordem
    // original de cadastro guardada em `cidades`.
    std::vector<Cidade*> ordenadas = cidades;
    std::sort(ordenadas.begin(), ordenadas.end(), [](Cidade* a, Cidade* b) {
        return a->getVisitas() > b->getVisitas(); // ordem decrescente de visitas
    });

    for (Cidade* c : ordenadas) {
        std::cout << "- " << c->getNome() << ": " << c->getVisitas() << " visita(s).\n";
    }
}

void ControladorDeTransito::relatarCidades() const {
    std::cout << "\n=== Cidades Cadastradas ===\n";
    if (cidades.empty()) {
        std::cout << "Nenhuma cidade cadastrada.\n";
        return;
    }
    for (Cidade* c : cidades) {
        std::cout << "- " << c->getNome() << "\n";
    }
}

void ControladorDeTransito::relatarTrajetos() const {
    std::cout << "\n=== Trajetos Cadastrados ===\n";
    if (trajetos.empty()) {
        std::cout << "Nenhum trajeto cadastrado.\n";
        return;
    }
    for (Trajeto* t : trajetos) {
        std::cout << "- " << t->getOrigem()->getNome() << " -> " << t->getDestino()->getNome()
                   << " [" << (t->getTipo() == 'A' ? "Aquatico" : "Terrestre") << "] "
                   << t->getDistancia() << " km\n";
    }
}


// Formato dos arquivos: texto simples, um registro por linha, campos
// separados pelo caractere '|' (escolhido por ser raro em nomes próprios,
// ao contrário da vírgula ou do espaço). Cada função de salvamento é
// simétrica à sua função de carregamento correspondente.

// Função auxiliar (visível apenas dentro deste arquivo, por isso dentro de
// um namespace anônimo) que separa uma linha de texto em campos, usando um
// caractere delimitador.

namespace {
    std::vector<std::string> dividirCampos(const std::string& linha, char delimitador) {
        std::vector<std::string> campos;
        std::stringstream ss(linha);
        std::string campo;
        while (std::getline(ss, campo, delimitador)) {
            campos.push_back(campo);
        }
        return campos;
    }
}

void ControladorDeTransito::salvarDados(const std::string& pasta) const {
    // Cria a pasta de destino, caso ainda não exista. Se ela já existir,
    // mkdir() retorna um erro que simplesmente ignoramos (não é um problema
    // real para o nosso propósito).
    std::filesystem::create_directories(pasta);

    //cidades.txt : nome|visitas
    {
        std::ofstream out(pasta + "/cidades.txt");
        for (Cidade* c : cidades) {
            out << c->getNome() << "|" << c->getVisitas() << "\n";
        }
    }

    // trajetos.txt : origem|destino|tipo|distancia
    {
        std::ofstream out(pasta + "/trajetos.txt");
        for (Trajeto* t : trajetos) {
            out << t->getOrigem()->getNome() << "|" << t->getDestino()->getNome() << "|"
                << t->getTipo() << "|" << t->getDistancia() << "\n";
        }
    }

    //  transportes.txt : nome|tipo|capacidade|velocidade|distDescanso|tempoDescanso|localAtual
    {
        std::ofstream out(pasta + "/transportes.txt");
        for (Transporte* t : transportes) {
            out << t->getNome() << "|" << t->getTipo() << "|" << t->getCapacidade() << "|"
                << t->getVelocidade() << "|" << t->getDistanciaEntreDescansos() << "|"
                << t->getTempoDeDescanso() << "|" << t->getLocalAtual()->getNome() << "\n";
        }
    }

    // passageiros.txt : nome|localAtual
    {
        std::ofstream out(pasta + "/passageiros.txt");
        for (Passageiro* p : passageiros) {
            out << p->getNome() << "|" << p->getLocalAtual()->getNome() << "\n";
        }
    }

    //  viagens.txt : id|estado|transporte|pass1,pass2,...|origem|destino|origemGeral|destinoGeral|distanciaTotal|distanciaPercorrida|horasDescansoRestante|distDesdeDescanso|idProxima
    {
        std::ofstream out(pasta + "/viagens.txt");
        for (Viagem* v : viagens) {
            out << v->getId() << "|" << static_cast<int>(v->getEstado()) << "|"
                << v->getTransporte()->getNome() << "|";

            const std::vector<Passageiro*>& pass = v->getPassageiros();
            for (std::size_t i = 0; i < pass.size(); i++) {
                out << pass[i]->getNome();
                if (i + 1 < pass.size()) out << ",";
            }

            out << "|" << v->getOrigem()->getNome() << "|" << v->getDestino()->getNome() << "|"
                << v->getOrigemGeral()->getNome() << "|" << v->getDestinoGeral()->getNome() << "|"
                << v->getDistanciaTotal() << "|" << v->getDistanciaPercorrida() << "|"
                << v->getHorasDescansoRestante() << "|" << v->getDistanciaDesdeUltimoDescanso() << "|"
                << (v->getProxima() != nullptr ? v->getProxima()->getId() : -1) << "\n";
        }
    }

    std::cout << "Dados salvos com sucesso na pasta \"" << pasta << "\".\n";
}

void ControladorDeTransito::carregarDados(const std::string& pasta) {
    // Antes de carregar, descartamos qualquer dado que já esteja em
    // memória, para evitar duplicação caso este método seja chamado mais
    // de uma vez durante a execução (por exemplo, pela opção de menu
    // "Carregar dados agora").
    for (Viagem* v : viagens)         delete v;
    for (Passageiro* p : passageiros) delete p;
    for (Transporte* t : transportes) delete t;
    for (Trajeto* tj : trajetos)      delete tj;
    for (Cidade* c : cidades)         delete c;
    viagens.clear();
    passageiros.clear();
    transportes.clear();
    trajetos.clear();
    cidades.clear();
    proximoIdViagem = 1;

    //  cidades.txt
    // Precisa ser carregado PRIMEIRO, pois trajetos, transportes,
    // passageiros e viagens dependem de ponteiros para Cidade já existentes.
    {
        std::ifstream in(pasta + "/cidades.txt");
        std::string linha;
        while (std::getline(in, linha)) {
            if (linha.empty()) continue;
            auto campos = dividirCampos(linha, '|');
            if (campos.size() < 2) continue; // linha corrompida/incompleta: ignora

            Cidade* c = new Cidade(campos[0]);
            int visitas = std::stoi(campos[1]);
            for (int i = 0; i < visitas; i++) c->incrementarVisita();
            cidades.push_back(c);
        }
    }

    //trajetos.txt ----
    {
        std::ifstream in(pasta + "/trajetos.txt");
        std::string linha;
        while (std::getline(in, linha)) {
            if (linha.empty()) continue;
            auto campos = dividirCampos(linha, '|');
            if (campos.size() < 4) continue;

            Cidade* origem = buscarCidade(campos[0]);
            Cidade* destino = buscarCidade(campos[1]);
            if (origem == nullptr || destino == nullptr) continue; // dado inconsistente: ignora

            trajetos.push_back(new Trajeto(origem, destino, campos[2][0], std::stoi(campos[3])));
        }
    }

    //transportes.txt
    {
        std::ifstream in(pasta + "/transportes.txt");
        std::string linha;
        while (std::getline(in, linha)) {
            if (linha.empty()) continue;
            auto campos = dividirCampos(linha, '|');
            if (campos.size() < 7) continue;

            Cidade* local = buscarCidade(campos[6]);
            if (local == nullptr) continue;

            transportes.push_back(new Transporte(campos[0], campos[1][0], std::stoi(campos[2]),
                                                   std::stoi(campos[3]), std::stoi(campos[4]),
                                                   std::stoi(campos[5]), local));
        }
    }

    // passageiros.txt
    {
        std::ifstream in(pasta + "/passageiros.txt");
        std::string linha;
        while (std::getline(in, linha)) {
            if (linha.empty()) continue;
            auto campos = dividirCampos(linha, '|');
            if (campos.size() < 2) continue;

            Cidade* local = buscarCidade(campos[1]);
            if (local == nullptr) continue;

            passageiros.push_back(new Passageiro(campos[0], local));
        }
    }

    // viagens.txt
    // O carregamento das viagens é feito em DUAS passadas, pois um trecho
    // pode referenciar (via idProxima) um outro trecho que ainda não foi
    // lido do arquivo (por exemplo, o trecho 1 pode citar o trecho 3, que
    // só aparece mais adiante no arquivo).

    //   Passada 1: cria todos os objetos Viagem (sem ligar "proxima" ainda),
    //              guardando, para cada um, o idProxima que foi lido.
    //   Passada 2: agora que todos os trechos existem em memória, resolve
    //              cada idProxima no ponteiro Viagem* correspondente.

    {
        std::ifstream in(pasta + "/viagens.txt");
        std::string linha;

        // Guarda, para cada trecho recém-criado, o id do seu "próximo"
        // trecho (ou -1, se for o último da cadeia), para resolver na
        // segunda passada.
        std::vector<std::pair<Viagem*, int>> pendentesDeLigacao;
        int maiorIdEncontrado = 0;

        while (std::getline(in, linha)) {
            if (linha.empty()) continue;
            auto campos = dividirCampos(linha, '|');
            if (campos.size() < 13) continue; // linha corrompida/incompleta: ignora

            int id = std::stoi(campos[0]);
            EstadoViagem estado = static_cast<EstadoViagem>(std::stoi(campos[1]));
            Transporte* transporte = buscarTransporte(campos[2]);

            // O campo 3 é a lista de nomes de passageiros separada por
            // vírgulas (pode estar vazia, se ninguém embarcou nesse trecho).
            std::vector<Passageiro*> passageirosDoTrecho;
            if (!campos[3].empty()) {
                for (const std::string& nomePassageiro : dividirCampos(campos[3], ',')) {
                    Passageiro* p = buscarPassageiro(nomePassageiro);
                    if (p != nullptr) passageirosDoTrecho.push_back(p);
                }
            }

            Cidade* origem = buscarCidade(campos[4]);
            Cidade* destino = buscarCidade(campos[5]);
            Cidade* origemGeral = buscarCidade(campos[6]);
            Cidade* destinoGeral = buscarCidade(campos[7]);
            int distanciaTotal = std::stoi(campos[8]);
            int distanciaPercorrida = std::stoi(campos[9]);
            int horasDescansoRestante = std::stoi(campos[10]);
            int distDesdeDescanso = std::stoi(campos[11]);
            int idProxima = std::stoi(campos[12]);

            // Se alguma referência essencial não pôde ser resolvida, o
            // registro está inconsistente e é descartado com segurança.
            if (transporte == nullptr || origem == nullptr || destino == nullptr ||
                origemGeral == nullptr || destinoGeral == nullptr) {
                continue;
            }

            Viagem* trecho = new Viagem(id, transporte, passageirosDoTrecho, origem, destino,
                                         origemGeral, destinoGeral, distanciaTotal);
            trecho->restaurarEstado(estado, distanciaPercorrida, horasDescansoRestante, distDesdeDescanso);

            viagens.push_back(trecho);
            pendentesDeLigacao.push_back({trecho, idProxima});
            maiorIdEncontrado = std::max(maiorIdEncontrado, id);

            // Se este trecho estava EM_ANDAMENTO no momento em que os dados
            // foram salvos, precisamos "religar" o transporte e os
            // passageiros a ele — essa associação (viagemAtual) não é
            // salva diretamente nos arquivos de transporte/passageiro,
            // pois ela é totalmente derivada do estado da viagem.
            if (estado == EstadoViagem::EM_ANDAMENTO) {
                transporte->setViagemAtual(trecho);
                for (Passageiro* p : passageirosDoTrecho) {
                    p->setViagemAtual(trecho);
                }
            }
        }

        // Segunda passada: agora que TODOS os trechos já existem em
        // memória, resolvemos cada idProxima no ponteiro Viagem*
        // correspondente.
        for (auto& par : pendentesDeLigacao) {
            Viagem* trecho = par.first;
            int idProxima = par.second;
            if (idProxima == -1) continue; // este já é o último trecho da sua cadeia

            for (Viagem* candidato : viagens) {
                if (candidato->getId() == idProxima) {
                    trecho->setProxima(candidato);
                    break;
                }
            }
        }

        // O próximo ID de viagem a ser gerado deve continuar de onde
        // paramos, para nunca repetir um ID já usado.
        proximoIdViagem = maiorIdEncontrado + 1;
    }

    std::cout << "Dados carregados com sucesso da pasta \"" << pasta << "\".\n";
}
