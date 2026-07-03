
// Ponto de entrada do sistema.
//
// Responsável por exibir o menu, ler entradas do usuário, chamar o
// ControladorDeTransito e salvar/carregar os dados.


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include "ControladorDeTransito.h"

// Pasta padrão (relativa ao diretório onde o executável é rodado) onde os
// dados do sistema são salvos e carregados.
static const std::string PASTA_DADOS = "dados";


// Exibe `mensagem` e lê uma linha inteira de texto digitada pelo usuário
// (permite nomes com espaços, por exemplo "Rio de Janeiro").
std::string lerLinha(const std::string& mensagem) {
    std::cout << mensagem;
    std::string linha;
    std::getline(std::cin, linha);
    return linha;
}

// Exibe `mensagem` e lê um número inteiro do teclado. Caso o usuário digite
// algo que não seja um número inteiro válido, a pergunta é repetida até que
// uma entrada válida seja fornecida — isso evita que o programa "quebre" ou
// entre em um estado inconsistente por causa de uma digitação errada.
int lerInteiro(const std::string& mensagem) {
    while (true) {
        std::cout << mensagem;
        std::string linha;
        std::getline(std::cin, linha);
        try {
            std::size_t posFinal;
            int valor = std::stoi(linha, &posFinal);
            // Garante que a linha inteira foi consumida como número (evita
            // aceitar entradas como "10abc" como se fossem "10").
            if (posFinal == linha.size()) {
                return valor;
            }
        } catch (...) {
            // std::stoi lança exceção se a string não começar com um
            // número válido; simplesmente cai para a mensagem de erro abaixo.
        }
        std::cout << "Entrada inválida. Por favor, digite um número inteiro.\n";
    }
}

// Exibe `mensagem` e lê um caractere que deve ser 'A' (Aquático) ou
// 'T' (Terrestre), aceitando tanto maiúsculas quanto minúsculas. Repete a
// pergunta até receber uma entrada válida.
char lerTipo(const std::string& mensagem) {
    while (true) {
        std::string linha = lerLinha(mensagem);
        if (!linha.empty()) {
            char c = static_cast<char>(std::toupper(static_cast<unsigned char>(linha[0])));
            if (c == 'A' || c == 'T') {
                return c;
            }
        }
        std::cout << "Entrada inválida. Digite 'A' para Aquático ou 'T' para Terrestre.\n";
    }
}

// Lê uma lista de nomes separados por vírgula (ex.: "Ana, Bruno, Carlos") e
// devolve um vetor de strings, já removendo espaços em branco extras no
// início/fim de cada nome. Uma linha vazia resulta em um vetor vazio (útil
// para representar "nenhum passageiro").
std::vector<std::string> lerListaNomes(const std::string& mensagem) {
    std::string linha = lerLinha(mensagem);
    std::vector<std::string> nomes;

    std::stringstream ss(linha);
    std::string nome;
    while (std::getline(ss, nome, ',')) {
        std::size_t inicio = nome.find_first_not_of(" \t");
        if (inicio == std::string::npos) continue; // campo vazio (ex.: vírgula duplicada): ignora
        std::size_t fim = nome.find_last_not_of(" \t");
        nomes.push_back(nome.substr(inicio, fim - inicio + 1));
    }
    return nomes;
}


//menu principal

void exibirMenu() {
    std::cout << "\n==================================================\n";
    std::cout << "        SISTEMA CONTROLADOR DE VIAGENS\n";
    std::cout << "==================================================\n";
    std::cout << " 1  - Cadastrar cidade\n";
    std::cout << " 2  - Cadastrar trajeto\n";
    std::cout << " 3  - Cadastrar transporte\n";
    std::cout << " 4  - Cadastrar passageiro\n";
    std::cout << " 5  - Iniciar viagem\n";
    std::cout << " 6  - Avancar horas (simular passagem do tempo)\n";
    std::cout << " 7  - Relatorio: posicao dos passageiros\n";
    std::cout << " 8  - Relatorio: posicao dos transportes\n";
    std::cout << " 9  - Relatorio: viagens em andamento\n";
    std::cout << "10  - Relatorio: cidades mais visitadas\n";
    std::cout << "11  - Listar cidades cadastradas\n";
    std::cout << "12  - Listar trajetos cadastrados\n";
    std::cout << "13  - Salvar dados agora\n";
    std::cout << "14  - Carregar dados agora (descarta alteracoes nao salvas)\n";
    std::cout << " 0  - Sair (salva os dados automaticamente)\n";
    std::cout << "==================================================\n";
    std::cout << "Escolha uma opcao: ";
}

int main() {
    // Cria o controlador (inicialmente vazio) e imediatamente tenta
    // carregar dados salvos em execuções anteriores. Se a pasta/arquivos
    // não existirem ainda (primeira execução), o sistema simplesmente
    // continua vazio, sem erro.
    ControladorDeTransito controlador;
    controlador.carregarDados(PASTA_DADOS);

    bool executando = true;
    while (executando) {
        exibirMenu();

        std::string opcaoTexto;
        std::getline(std::cin, opcaoTexto);

        int opcao;
        try {
            opcao = std::stoi(opcaoTexto);
        } catch (...) {
            std::cout << "Opcao invalida. Tente novamente.\n";
            continue;
        }

        switch (opcao) {
            case 1: {
                std::string nome = lerLinha("Nome da cidade: ");
                controlador.cadastrarCidade(nome);
                break;
            }
            case 2: {
                std::string origem = lerLinha("Cidade de origem: ");
                std::string destino = lerLinha("Cidade de destino: ");
                char tipo = lerTipo("Tipo do trajeto (A = Aquatico, T = Terrestre): ");
                int distancia = lerInteiro("Distancia em km: ");
                controlador.cadastrarTrajeto(origem, destino, tipo, distancia);
                break;
            }
            case 3: {
                std::string nome = lerLinha("Nome do transporte: ");
                char tipo = lerTipo("Tipo do transporte (A = Aquatico, T = Terrestre): ");
                int capacidade = lerInteiro("Capacidade de passageiros: ");
                int velocidade = lerInteiro("Velocidade (km/h): ");
                int distDescanso = lerInteiro("Distancia entre descansos (km, 0 = nunca descansa): ");
                int tempoDescanso = lerInteiro("Tempo de descanso (horas): ");
                std::string local = lerLinha("Cidade atual do transporte: ");
                controlador.cadastrarTransporte(nome, tipo, capacidade, velocidade,
                                                 distDescanso, tempoDescanso, local);
                break;
            }
            case 4: {
                std::string nome = lerLinha("Nome do passageiro: ");
                std::string local = lerLinha("Cidade atual do passageiro: ");
                controlador.cadastrarPassageiro(nome, local);
                break;
            }
            case 5: {
                std::string transporte = lerLinha("Nome do transporte a ser usado: ");
                std::vector<std::string> nomesPassageiros = lerListaNomes(
                    "Nomes dos passageiros, separados por virgula (ou deixe em branco): ");
                std::string origem = lerLinha("Cidade de origem: ");
                std::string destino = lerLinha("Cidade de destino: ");
                controlador.iniciarViagem(transporte, nomesPassageiros, origem, destino);
                break;
            }
            case 6: {
                int horas = lerInteiro("Quantas horas deseja avancar? ");
                controlador.avancarHoras(horas);
                break;
            }
            case 7:
                controlador.relatarPosicaoPassageiros();
                break;
            case 8:
                controlador.relatarPosicaoTransportes();
                break;
            case 9:
                controlador.relatarViagensEmAndamento();
                break;
            case 10:
                controlador.relatarCidadesMaisVisitadas();
                break;
            case 11:
                controlador.relatarCidades();
                break;
            case 12:
                controlador.relatarTrajetos();
                break;
            case 13:
                controlador.salvarDados(PASTA_DADOS);
                break;
            case 14:
                controlador.carregarDados(PASTA_DADOS);
                break;
            case 0:
                controlador.salvarDados(PASTA_DADOS);
                std::cout << "Dados salvos. Encerrando o sistema. Ate logo!\n";
                executando = false;
                break;
            default:
                std::cout << "Opcao invalida. Tente novamente.\n";
        }
    }
    return 0;
}
