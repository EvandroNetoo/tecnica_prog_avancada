// Tamanho da tabela hash: 100
// Inicializar tabela: 0.000003 segundos4ttt
// Inserir alunos 513.161091 segundos
// Total 513.161094 segundos


// Tamanho da tabela hash: 1000000
// Inicializar tabela: 0.009534 segundos
// Inserir alunos 1.300364 segundos
// Total 1.309898 segundos
// Apenas remover os comentários

#include <cstdlib>
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>

#define TAM 100
// #define TAM 1000000

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

struct TabelaHashAlunos {
    Alunos tabela[TAM];
    int tam;
};

void inicializar_tabela(TabelaHashAlunos *tabela_hash) {
    for (int i = 0; i < TAM; i++) {
        tabela_hash->tabela[i].inicio = NULL;
        tabela_hash->tabela[i].fim = NULL;
        tabela_hash->tabela[i].tam = 0;
    }
    tabela_hash->tam = 0;
}

int funcao_hash(const Aluno *const aluno) {
    // return stoi(aluno->cpf.substr(0, 3) + aluno->cpf.substr(4, 3) + aluno->cpf.substr(8, 3));
    return stoi(aluno->cpf.substr(12, 2));
}

bool inserir(TabelaHashAlunos *tabela_hash_alunos, Aluno *novo_aluno) {
    int index = funcao_hash(novo_aluno) % TAM;

    Alunos *alunos = &tabela_hash_alunos->tabela[index];

    Aluno *atual = alunos->inicio, *atual2 = NULL;
    while (atual != NULL) {
        if (atual->cpf == novo_aluno->cpf) {
            return false;
        }
        if (atual2 == NULL && novo_aluno->nome < atual->nome) {
            atual2 = atual;
        }
        atual = atual->prox;
    }
    atual = atual2;

    if (alunos->inicio == NULL && alunos->fim == NULL) {
        alunos->inicio = novo_aluno;
        alunos->fim = novo_aluno;
    } else if (novo_aluno->nome <= alunos->inicio->nome) {
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
    tabela_hash_alunos->tam++;
    return true;
}

Aluno *pesquisar(TabelaHashAlunos *tabela_hash_alunos, const string &busca, bool (*funcao_verificadora)(const Aluno *, const string &)) {
    for (int i = 0; i < TAM; i++) {
        Alunos *alunos = &tabela_hash_alunos->tabela[i];
        Aluno *atual = alunos->inicio;
        while (atual != NULL) {
            if (funcao_verificadora(atual, busca)) {
                return atual;
            }
            atual = atual->prox;
        }
    }

    return NULL;
}

bool verificar_aluno_por_matricula(const Aluno *aluno, const string &matricula) {
    return aluno->matricula == matricula;
}

bool verificar_aluno_por_cpf(const Aluno *aluno, const string &cpf) {
    return aluno->cpf == cpf;
}

void remover_aluno_por_referencia(TabelaHashAlunos *tabela_hash_alunos, Aluno *aluno) {
    Alunos *alunos = &tabela_hash_alunos->tabela[funcao_hash(aluno) % TAM];

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
    printf(
        "%s - %s - %s - %.2f - %d - %s - %s\n",
        aluno->matricula.c_str(),
        aluno->cpf.c_str(),
        aluno->nome.c_str(),
        aluno->nota,
        aluno->idade,
        aluno->curso.c_str(),
        aluno->cidade.c_str()
    );
}

void imprime_alunos(Alunos *alunos) {
    Aluno *atual = alunos->inicio;

    while (atual != NULL) {
        imprime_aluno(atual);
        atual = atual->prox;
    }
}

void imprime_tabela_hash_alunos(TabelaHashAlunos *tabela_hash_alunos) {
    printf("Alunos:\n");
    for (int i = 0; i < TAM; i++) {
        Alunos *alunos = &tabela_hash_alunos->tabela[i];
        imprime_alunos(alunos);
    }
}

void carregar_lista_de_alunos(TabelaHashAlunos *tabela_hash_alunos, const string &nome_arquivo) {
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

        Aluno *novo_aluno = new Aluno{ NULL, NULL, colunas[0], colunas[1], colunas[2], stof(colunas[3]), stoi(colunas[4]), colunas[5], colunas[6] };

        inserir(tabela_hash_alunos, novo_aluno);
    }
}

int menu() {
    printf("MENU:\n");
    printf("1 - Buscar aluno por matrícula\n");
    printf("2 - Buscar aluno por CPF\n");
    printf("0 - Sair\n");
    printf("Opção: ");

    int opcao;
    scanf("%d", &opcao);
    return opcao;
}

void buscar(TabelaHashAlunos *tabela_hash_alunos, const char *bucar_por, bool (*funcao_verificadora)(const Aluno *, const string &)) {
    string busca;
    printf("Busca por %s: ", bucar_por);
    cin >> busca;
    Aluno *aluno = pesquisar(tabela_hash_alunos, busca, funcao_verificadora);

    if (aluno == NULL) {
        cout << "Nenhum aluno encontrado." << endl;
        return;
    }

    printf("Dados:\n");
    imprime_aluno(aluno);

    cout << "Digite 1 para remover esse aluno, qualquer coisa para continuar: ";
    string quer_remover_aluno;
    cin >> quer_remover_aluno;
    if (quer_remover_aluno == "1") {
        remover_aluno_por_referencia(tabela_hash_alunos, aluno);
    }
}

int main() {
    clock_t inicio, fim, total = 0;

    printf("Tamanho da tabela hash: %d\n", TAM);

    inicio = clock();
    TabelaHashAlunos *tabela_hash_alunos = new TabelaHashAlunos;
    inicializar_tabela(tabela_hash_alunos);
    fim = clock();
    printf("Inicializar tabela: %f segundos\n", double(fim - inicio) / CLOCKS_PER_SEC);
    total += fim - inicio;

    inicio = clock();
    carregar_lista_de_alunos(tabela_hash_alunos, "../alunos_completos.csv");
    fim = clock();
    printf("Inserir alunos %f segundos\n", double(fim - inicio) / CLOCKS_PER_SEC);
    total += fim - inicio;

    printf("Total %f segundos\n", double(total) / CLOCKS_PER_SEC);
    printf("Total alunos: %d\n", tabela_hash_alunos->tam);

    int opcao;

    do {
        opcao = menu();

        switch (opcao) {

        case 1:
            buscar(tabela_hash_alunos, "matrícula", verificar_aluno_por_matricula);
            break;
        case 2:
            buscar(tabela_hash_alunos, "CPF", verificar_aluno_por_cpf);
            break;
        case 0:
            break;
        default:
            printf("Opção inválida.\n");
            break;
        }

    }
    while (opcao != 0);

    return 0;
}