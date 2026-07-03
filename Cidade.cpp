#include "Cidade.h"

// O construtor apenas guarda o nome recebido como parâmetro e zera o
// contador interno de visitas, já que uma cidade recém-criada nunca foi
// visitada.
Cidade::Cidade(const std::string& nome) : nome(nome), visitas(0) {
}

std::string Cidade::getNome() const {
    return nome;
}

void Cidade::incrementarVisita() {
    // Simplesmente soma 1 ao contador. Quem decide QUANDO chamar este
    // método é a classe ControladorDeTransito (em finalizarCadeia), nunca a
    // própria Cidade — isso mantém a classe Cidade simples e focada apenas
    // em representar seus próprios dados (princípio da responsabilidade única).

    visitas++;
}

int Cidade::getVisitas() const {
    return visitas;
}
