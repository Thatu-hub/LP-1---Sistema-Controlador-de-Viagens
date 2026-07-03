#include "Viagem.h"
#include <sstream>

Viagem::Viagem(int id, Transporte* transporte, std::vector<Passageiro*> passageiros,
               Cidade* origem, Cidade* destino, Cidade* origemGeral, Cidade* destinoGeral,
               int distanciaTotal)
    : id(id), transporte(transporte), passageiros(std::move(passageiros)),
      origem(origem), destino(destino), origemGeral(origemGeral), destinoGeral(destinoGeral),
      distanciaTotal(distanciaTotal),
      distanciaPercorrida(0),               // ninguém andou nada ainda
      distanciaDesdeUltimoDescanso(0),      // nenhum km percorrido desde o "último" descanso (que ainda não houve)
      horasDescansoRestante(0),             // não começa descansando
      estado(EstadoViagem::NAO_INICIADA),   // só passa a EM_ANDAMENTO quando iniciarViagem() for chamado
      proxima(nullptr) {                    // por padrão, é o último trecho da cadeia (o controlador liga depois, se houver mais trechos)
}

void Viagem::iniciarViagem() {
    estado = EstadoViagem::EM_ANDAMENTO;

    // A partir de agora, tanto o transporte quanto cada passageiro sabem
    // que estão "ocupados" nesta viagem — isso é o que impede, por exemplo,
    // que o mesmo transporte seja escalado para duas viagens ao mesmo tempo

    transporte->setViagemAtual(this);
    for (Passageiro* p : passageiros) {
        p->setViagemAtual(this);
    }
}

void Viagem::avancarUmaHora() {

    // Só faz sentido simular o avanço de um trecho que está, de fato, em
    // andamento. Trechos NAO_INICIADA (aguardando a vez) ou já CONCLUIDA
    // simplesmente não fazem nada aqui

    if (estado != EstadoViagem::EM_ANDAMENTO) {
        return;
    }

    // Caso 1: o transporte está em um período de descanso obrigatório.
    // Nessa hora, ele não percorre nenhuma distância — apenas "gasta" uma
    // hora do tempo de descanso que ainda falta

    if (horasDescansoRestante > 0) {
        horasDescansoRestante--;
        return;
    }

    // Caso 2: o transporte está livre para se mover. Avança a distância
    // percorrida de acordo com a velocidade cadastrada para o transporte
    // (quantos quilômetros ele anda em uma hora)

    int velocidade = transporte->getVelocidade();
    distanciaPercorrida += velocidade;
    distanciaDesdeUltimoDescanso += velocidade;

    // Verifica se, com esse avanço, o transporte já alcançou (ou
    // ultrapassou) a distância total do trecho. Usamos ">=" porque a
    // velocidade pode não "encaixar" perfeitamente na distância restante
    // (ex.: faltam 30 km e a velocidade é 50 km/h) — nesse caso,
    // consideramos que o trecho terminou nesta mesma hora

    if (distanciaPercorrida >= distanciaTotal) {
        distanciaPercorrida = distanciaTotal; // "trava" no valor exato, evitando km "extras" no relatório
        estado = EstadoViagem::CONCLUIDA;
        return; // chegou: não faz sentido também iniciar um descanso agora
    }

    // Caso 3: o trecho ainda não terminou. Verifica se, desde a última
    // parada de descanso, o transporte já percorreu quilômetros suficientes
    // para ser obrigado a descansar novamente.
    // (distanciaEntreDescansos == 0 é tratado como "nunca precisa descansar".)

    int distanciaEntreDescansos = transporte->getDistanciaEntreDescansos();
    if (distanciaEntreDescansos > 0 && distanciaDesdeUltimoDescanso >= distanciaEntreDescansos) {
        horasDescansoRestante = transporte->getTempoDeDescanso();
        distanciaDesdeUltimoDescanso = 0; // reinicia a contagem para o próximo descanso
    }
}

std::string Viagem::relatarEstado() const {
    std::ostringstream oss;
    oss << "Trecho #" << id << " [" << transporte->getNome() << "]: "
        << origem->getNome() << " -> " << destino->getNome();

    switch (estado) {
        case EstadoViagem::NAO_INICIADA:
            oss << " (aguardando trecho anterior da cadeia terminar)";
            break;
        case EstadoViagem::EM_ANDAMENTO:
            oss << " (" << distanciaPercorrida << "/" << distanciaTotal << " km percorridos";
            if (horasDescansoRestante > 0) {
                oss << ", descansando por mais " << horasDescansoRestante << "h";
            }
            oss << ")";
            break;
        case EstadoViagem::CONCLUIDA:
            oss << " (trecho concluído)";
            break;
    }

    if (origem != origemGeral || destino != destinoGeral) {
        oss << " [parte da viagem " << origemGeral->getNome() << " -> " << destinoGeral->getNome() << "]";
    }

    return oss.str();
}

bool Viagem::isEmAndamento() const { return estado == EstadoViagem::EM_ANDAMENTO; }
bool Viagem::isConcluida() const   { return estado == EstadoViagem::CONCLUIDA; }
bool Viagem::isNaoIniciada() const { return estado == EstadoViagem::NAO_INICIADA; }

int Viagem::getId() const { return id; }
Transporte* Viagem::getTransporte() const { return transporte; }
const std::vector<Passageiro*>& Viagem::getPassageiros() const { return passageiros; }
Cidade* Viagem::getOrigem() const { return origem; }
Cidade* Viagem::getDestino() const { return destino; }
Cidade* Viagem::getOrigemGeral() const { return origemGeral; }
Cidade* Viagem::getDestinoGeral() const { return destinoGeral; }
int Viagem::getDistanciaTotal() const { return distanciaTotal; }
int Viagem::getDistanciaPercorrida() const { return distanciaPercorrida; }
int Viagem::getHorasDescansoRestante() const { return horasDescansoRestante; }
int Viagem::getDistanciaDesdeUltimoDescanso() const { return distanciaDesdeUltimoDescanso; }
EstadoViagem Viagem::getEstado() const { return estado; }

Viagem* Viagem::getProxima() const { return proxima; }
void Viagem::setProxima(Viagem* p) { proxima = p; }

void Viagem::restaurarEstado(EstadoViagem estadoSalvo, int distPercorrida,
                              int horasDescanso, int distDesdeDescanso) {
    // Usado apenas ao recarregar dados do disco: sobrescreve diretamente o
    // progresso da simulação com os valores que estavam salvos, sem passar
    // pelas regras de avancarUmaHora() (que não se aplicam aqui, pois não
    // estamos simulando tempo, estamos restaurando um "retrato" salvo).
    estado = estadoSalvo;
    distanciaPercorrida = distPercorrida;
    horasDescansoRestante = horasDescanso;
    distanciaDesdeUltimoDescanso = distDesdeDescanso;
}
