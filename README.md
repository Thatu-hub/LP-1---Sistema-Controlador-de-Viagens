# Sistema Controlador de Viagens

Trabalho da disciplina de Linguagem de Programação 1, desenvolvido em C++.

O sistema simula viagens entre cidades, permitindo o cadastro de cidades, trajetos, transportes e passageiros, além da execução de viagens com ou sem conexões entre cidades. O sistema também conta com persistência de dados em arquivos de texto.

---

## **main.cpp**

O arquivo main.cpp é o ponto de entrada do sistema.

Ele é responsável apenas por:
- Exibir o menu em linha de comando (CLI)
- Ler entradas do usuário
- Repassar os dados para o ControladorDeTransito

Toda a lógica do sistema (validações, cálculos e simulação de viagens) está centralizada na classe ControladorDeTransito.

---

## **Estrutura do projeto**

sistema_viagens/

├── main.cpp  
├── Cidade.h / Cidade.cpp  
├── Trajeto.h / Trajeto.cpp  
├── Transporte.h / Transporte.cpp  
├── Passageiro.h / Passageiro.cpp  
├── Viagem.h / Viagem.cpp  
├── ControladorDeTransito.h / ControladorDeTransito.cpp  

## **Como compilar**

É necessário ter o compilador g++ com suporte a C++17.

Dentro da pasta do projeto, execute:

g++ -std=c++17 -Wall -Wextra -O2 -o viagens \
main.cpp Cidade.cpp Trajeto.cpp Transporte.cpp Passageiro.cpp Viagem.cpp ControladorDeTransito.cpp

---

## **Como executar**

./viagens

Ao iniciar, o sistema tenta carregar automaticamente os dados da pasta "dados".  
Se a pasta não existir, o sistema inicia vazio.

---

## **Funcionalidades**

- Cadastro de cidades
- Cadastro de trajetos (A = aquático, T = terrestre)
- Cadastro de transportes
- Cadastro de passageiros
- Início de viagens entre cidades
- Viagens com ou sem conexão entre cidades
- Simulação de tempo por horas
- Relatórios do sistema
- Persistência de dados em arquivos de texto

---

## **Viagens com conexão**

Quando não existe trajeto direto entre duas cidades, o sistema cria automaticamente uma sequência de viagens intermediárias.

Exemplo: A → X → Y → B

Cada trecho é executado separadamente, e o transporte só chega ao destino final quando o último trecho da cadeia é concluído.

---

## **Persistência de dados**

Os dados são salvos na pasta:

dados/

Arquivos:
- cidades.txt
- trajetos.txt
- transportes.txt
- passageiros.txt
- viagens.txt

O sistema também salva o estado das viagens em andamento, permitindo continuar a simulação após reiniciar o programa.
