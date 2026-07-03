#ifndef CIDADE_H
#define CIDADE_H

#include <string>

// ============================================================================
// Classe Cidade
// ----------------------------------------------------------------------------
// Representa uma cidade dentro do sistema de controle de viagens.
//
// Cada cidade possui:
//   - um "nome", que funciona como identificador único dentro do sistema
//     (não permitimos duas cidades com o mesmo nome, essa verificação é
//     feita em ControladorDeTransito::cadastrarCidade);
//   - um contador de "visitas", que representa quantas vezes essa cidade foi
//     o DESTINO FINAL de uma viagem concluída. Esse contador é usado depois
//     pelo relatório "cidades mais visitadas" (funcionalidade obrigatória do
//     trabalho).
//
// Observação de projeto: cidades intermediárias de uma viagem com conexão
// (ex.: A -> X -> Y -> B) NÃO têm suas visitas incrementadas, pois o
// enunciado deixa claro que os passageiros e o transporte só "chegam"
// efetivamente ao destino quando a ÚLTIMA viagem da cadeia é concluída.

class Cidade {
private:
    std::string nome;  // Nome da cidade (chave de identificação dentro do sistema)
    int visitas;        // Quantas vezes esta cidade foi destino final de uma viagem concluída

public:
    // Construtor: recebe o nome da cidade e inicializa o contador de
    // visitas em zero, pois toda cidade recém-cadastrada ainda não foi
    // visitada por ninguém.
    explicit Cidade(const std::string& nome);

    // Retorna o nome da cidade.
    std::string getNome() const;

    // Incrementa em 1 o contador de visitas. Deve ser chamado exatamente uma
    // vez, sempre que uma cadeia de viagens (uma viagem completa, direta ou
    // com conexão) termina tendo esta cidade como destino final.
    void incrementarVisita();

    // Retorna quantas vezes essa cidade foi visitada até o momento.
    int getVisitas() const;
};

#endif
