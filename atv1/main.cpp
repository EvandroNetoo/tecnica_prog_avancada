#include <cstdlib>
#include <cstdio>
#include <string>
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

struct Aluno {
    Aluno *ante;
    Aluno *prox;
    string matricula;
    string cpf;
    string nome;
    float nota;
    int idade;
    string curso;
    string cidade;
};

struct Alunos {
    Aluno *inicio;
    Aluno *fim;
    int tam;
};

bool inserir_ordenado(Alunos *alunos, Aluno *novo_aluno) {
    if (alunos->inicio == NULL && alunos->fim == NULL) {
        alunos->inicio = novo_aluno;
        alunos->fim = novo_aluno;
        alunos->tam++;
        return true;
    }
    Aluno *atual = alunos->inicio, *atual2 = NULL;
    while (atual != NULL ) {
        if (atual->matricula == novo_aluno->matricula || atual->cpf == novo_aluno->cpf ) {
            return false;
        }
        if (atual2 == NULL && novo_aluno->nome < atual->nome) {
            atual2 = atual;
        }
        atual = atual->prox;
    }
    atual = atual2;
    
    if (novo_aluno->nome <= alunos->inicio->nome) {
        alunos->inicio->ante = novo_aluno;
        novo_aluno->prox = alunos->inicio;
        alunos->inicio = novo_aluno;
    } else if (novo_aluno->nome >= alunos->fim->nome) {
        alunos->fim->prox = novo_aluno;
        novo_aluno->ante = alunos->fim;
        alunos->fim = novo_aluno;
    } else {
        atual->ante->prox = novo_aluno;
        novo_aluno->ante = atual->ante;
        
        atual->ante = novo_aluno;
        novo_aluno->prox = atual;
    }
    alunos->tam++;
    return true;
}

bool inserir_simples(Alunos *alunos, Aluno *novo_aluno) {
    if (alunos->inicio == NULL && alunos->fim == NULL) {
        alunos->inicio = novo_aluno;
        alunos->fim = novo_aluno;
    } else {
        alunos->fim->prox = novo_aluno;
        novo_aluno->ante = alunos->fim;
        alunos->fim = novo_aluno;
    }
    alunos->tam++;
    return true;
}

bool verificar_aluno_por_matricula(const Aluno *aluno, const string& matricula) {
    return aluno->matricula == matricula;
}

bool verificar_aluno_por_cpf(const Aluno *aluno, const string& cpf) {
    return aluno->cpf == cpf;
}

Aluno *pesquisar(Alunos *alunos, const string& busca, bool (*funcao_verificadora)(const Aluno*, const string&)) {
    Aluno *atual = alunos->inicio;
    while (atual != NULL) {
        if (funcao_verificadora(atual, busca)) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

void remover_aluno_por_referencia(Alunos *alunos, Aluno *aluno) {
    if (aluno == NULL) return;
    if (aluno == alunos->inicio && aluno == alunos->fim) {
        alunos->inicio = NULL;
        alunos->fim = NULL;
    } else if (aluno == alunos->inicio) {
        alunos->inicio = aluno->prox;
        alunos->inicio->ante = NULL;
    } else if (aluno == alunos->fim) {
        alunos->fim = aluno->ante;
        alunos->fim->prox = NULL;
    } else {
        aluno->ante->prox = aluno->prox;
        aluno->prox->ante = aluno->ante;
    }
    delete aluno;
    alunos->tam--;
}

void imprime_aluno(Aluno *aluno) {
    cout << aluno->matricula << " - " << aluno->cpf << " - " << aluno->nome << " - " 
    << aluno->nota << " - " << aluno->idade << " - " << aluno->curso << " - " 
    << aluno->cidade << endl;
}

void imprime_alunos(Alunos *alunos) {
    Aluno *atual = alunos->inicio;

    printf("Alunos:\n");

    while (atual != NULL) {
        imprime_aluno(atual);
        atual = atual->prox;
    } 
}

void carregar_lista_de_alunos(Alunos *alunos, const string nome_arquivo) {
    ifstream file(nome_arquivo);

    string linha;

    getline(file, linha);

    while (getline(file, linha)) {
        vector<string> colunas;

        size_t inicio = 0;
        size_t fim;

        while ((fim = linha.find(',', inicio)) != string::npos) {
            colunas.emplace_back(linha.substr(inicio, fim - inicio));
            inicio = fim + 1;
        }
        colunas.emplace_back(linha.substr(inicio));

        Aluno *novo_aluno = new Aluno{NULL, NULL, colunas[0], colunas[1], colunas[2], stof(colunas[3]), stoi(colunas[4]), colunas[5], colunas[6]};
        cout << novo_aluno->matricula << endl;
        // inserir_simples(alunos, novo_aluno);
        inserir_ordenado(alunos, novo_aluno);
    }
}

int menu() {
    cout
    << "MENU:" << endl
    << "1 - Buscar aluno por matrícula" << endl
    << "2 - Buscar aluno por CPF" << endl
    << "0 - Sair" << endl
    << "Opção: ";

    int opcao;
    cin >> opcao;
    return opcao;
}

void buscar(Alunos *alunos, bool (*funcao_verificadora)(const Aluno*, const string&)) {
    string busca;
    cout << "Busca: ";
    cin >> busca;
    Aluno *aluno = pesquisar(alunos, busca, funcao_verificadora);

    if (aluno == NULL) {
        cout << "Nenhum aluno encontrado." << endl;
        return;
    }

    cout << "Dados: " << endl;
    imprime_aluno(aluno);

    cout << "Digite 1 para remover esse aluno, qualquer coisa para continuar: ";
    string quer_remover_aluno;
    cin >> quer_remover_aluno;
    if (quer_remover_aluno == "1") {
        remover_aluno_por_referencia(alunos, aluno);
    }
}

int main() {
    Alunos *alunos = new Alunos{NULL, NULL, 0};
    carregar_lista_de_alunos(alunos, "../alunos.csv");
    imprime_alunos(alunos);
    int opcao;

    do {
        opcao = menu();

        switch (opcao) {

        case 1:
            buscar(alunos, verificar_aluno_por_matricula);
            break;
        case 2:
            buscar(alunos, verificar_aluno_por_cpf);
            break;

        case 0:
            break;
        default:
            cout << "Opção inválida." << endl;
            break;
        }

    } while (opcao != 0);
    
    return 0;
}