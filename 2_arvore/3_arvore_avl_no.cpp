#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <utility>
#include <cmath>

using namespace std;


struct Aluno {
    string matricula;
    string cpf;
    string nome;
    float nota;
    int idade;
    string curso;
    string cidade;

    bool operator<(const Aluno &outro) const {
        return nome < outro.nome;
    }

    bool operator<=(const Aluno &outro) const {
        return nome <= outro.nome;
    }

    bool operator>(const Aluno &outro) const {
        return nome > outro.nome;
    }

    bool operator>=(const Aluno &outro) const {
        return nome >= outro.nome;
    }

    bool operator==(const Aluno &outro) const {
        return nome == outro.nome;
    }

};

ostream &operator<<(ostream &os, const Aluno &a) {
    os << a.nome;
    return os;
}

template <typename Valor>
class ArvoreBinariaNo {
private:
    struct No {
        Valor valor;
        int altura;
        No *esq;
        No *dir;

    };

    struct Retorno {
        bool inseriu;
        No *novo_filho;
    };

    No *rse(No *no) {
        No *filho_direito = no->dir;
        No *filho_esquerdo_da_direita = filho_direito->esq;

        filho_direito->esq = no;
        no->dir = filho_esquerdo_da_direita;

        no->bal = get_bal(no);
        filho_direito->bal = get_bal(filho_direito);

        return filho_direito;
    }

    No *rsd(No *no) {
        No *filho_esquerdo = no->esq;
        No *filho_direito_da_esquerda = filho_esquerdo->dir;

        filho_esquerdo->dir = no;
        no->esq = filho_direito_da_esquerda;

        no->bal = get_bal(no);
        filho_esquerdo->bal = get_bal(filho_esquerdo);

        return filho_esquerdo;
    }

    No *balancear_no(No *no, No *no_inserido) {
        int bal_no = get_bal(no), bal_no_inserido = get_bal(no_inserido);

        if (bal_no == 2 && bal_no_inserido >= 0) return rsd(no);

    }

    Retorno inserir_rec(const Valor &valor, No *&no) {
        if (no == nullptr) {
            no = new No{ valor, 0, nullptr, nullptr };
            return Retorno{ true, no };
        } else if (valor < no->valor) {
            Retorno retorno = this->inserir_rec(valor, no->esq);

            if (!retorno.inseriu) return Retorno{ false, no };  // não inseriu

            if (get_altura(no->esq) > get_altura(no->esq)) no->altura++;  // cresceu

            if (abs(get_bal(no)) <= 1) return Retorno{ true, no };  // está balanceado



        } else if (valor > no->valor) {
            return this->inserir_rec(valor, no->dir);
        } else {
            if (!this->inserir_repetido) return { false ,nullptr };
            return this->inserir_rec(valor, no->dir);
        }
    }

    No *buscar_no_rec(const Valor &valor, No *no) {
        if (no == nullptr) return nullptr;
        if (valor == no->valor) return no;
        if (valor < no->valor) return buscar_no_rec(valor, no->esq);
        return buscar_no_rec(valor, no->dir);
    }

    bool remover_rec(const Valor &valor, No *&no) {
        if (no == nullptr) {
            return false;
        }

        if (valor < no->valor) {
            return remover_rec(valor, no->esq);
        } else if (valor > no->valor) {
            return remover_rec(valor, no->dir);
        } else {
            // Caso 1: Nó folha
            if (no->esq == nullptr && no->dir == nullptr) {
                delete no;
                no = nullptr;
            }
            // Caso 2: Nó com apenas um filho
            else if (no->esq == nullptr) {
                No *temp = no;
                no = no->dir;
                delete temp;
            } else if (no->dir == nullptr) {
                No *temp = no;
                no = no->esq;
                delete temp;
            }
            // Caso 3: Nó com dois filhos
            else {
                // Encontrar o sucessor (menor valor à direita)
                No *menor_direita = no->dir;
                while (menor_direita->esq != nullptr) {
                    menor_direita = menor_direita->esq;
                }
                // Copiar o valor do sucessor
                no->valor = menor_direita->valor;
                // Remover o sucessor
                return remover_rec(menor_direita->valor, no->dir);
            }
            return true;
        }
    }

    int get_altura(const No *no) {
        return no ? no->altura : -1;
    }

    int get_bal(const No *no) {
        return no ? get_altura(no->esq) - get_altura(no->dir) : 0;
    }

    void destruir(No *no) {
        if (no == nullptr) return;
        destruir(no->esq);
        destruir(no->dir);
        delete no;
    }

    // --- Percursos ---
    void pre_ordem_rec(No *no) {
        if (no == nullptr) return;
        cout << no->valor << endl;
        pre_ordem_rec(no->esq);
        pre_ordem_rec(no->dir);
    }

    void em_ordem_rec(No *no) {
        if (no == nullptr) return;
        em_ordem_rec(no->esq);
        cout << no->valor << endl;
        em_ordem_rec(no->dir);
    }

    void pos_ordem_rec(No *no) {
        if (no == nullptr) return;
        pos_ordem_rec(no->esq);
        pos_ordem_rec(no->dir);
        cout << no->valor << endl;
    }

    No *raiz;
    size_t tamanho;
    bool inserir_repetido;
public:
    ArvoreBinariaNo(bool inserir_repetido = true) {
        this->inserir_repetido = inserir_repetido;
        this->raiz = nullptr;
        this->tamanho = 0;
    }

    ~ArvoreBinariaNo() {
        destruir(raiz);
    }


    bool inserir(const Valor &valor) {
        bool inseriu = this->inserir_rec(valor, this->raiz);
        if (inseriu) {
            this->tamanho++;
        }
        return inseriu;
    }

    const Valor &buscar_valor(const Valor &valor) {
        No *no = this->buscar_no_rec(valor, this->raiz);
        if (no == nullptr) {
            throw out_of_range("Valor não encontrado");
        }
        return no->valor;
    }

    const No *buscar_no(const Valor &valor) {
        return this->buscar_no_rec(valor, this->raiz);
    }

    bool remover(const Valor &valor) {
        bool removeu = this->remover_rec(valor, this->raiz);
        if (removeu) {
            this->tamanho--;
        }
        return removeu;
    }

    size_t get_tamanho() {
        return this->tamanho;
    }

    int altura() {
        return get_altura(this->raiz) - 1;
    }

    // --- Percursos ---
    void pre_ordem() {
        this->pre_ordem_rec(this->raiz);
    }

    void em_ordem() {
        this->em_ordem_rec(this->raiz);
    }

    void pos_ordem() {
        this->pos_ordem_rec(this->raiz);
    }
};



void carregar_lista_de_alunos(ArvoreBinariaNo<Aluno> &arvore_alunos, const string &nome_arquivo) {
    ifstream arquivo(nome_arquivo);
    string linha;

    getline(arquivo, linha);


    string colunas[7];
    while (getline(arquivo, linha)) {

        size_t inicio = 0;
        size_t fim;


        int i = 0;
        while ((fim = linha.find(',', inicio)) != string::npos) {
            colunas[i] = linha.substr(inicio, fim - inicio);
            inicio = fim + 1;
            i++;
        }
        colunas[i] = linha.substr(inicio);

        Aluno novo_aluno = Aluno{ colunas[0], colunas[1], colunas[2], stof(colunas[3]), stoi(colunas[4]), colunas[5], colunas[6] };
        arvore_alunos.inserir(novo_aluno);
    }
}


int main() {
    clock_t inicio, fim, total = 0;
    ArvoreBinariaNo<Aluno> arvore_alunos(false);

    inicio = clock();
    carregar_lista_de_alunos(arvore_alunos, "../alunos_completos.csv");
    fim = clock();
    total += fim - inicio;
    printf("Inserir alunos: %f segundos\n", double(total) / CLOCKS_PER_SEC);
    printf("Altura árvore: %d\n", arvore_alunos.altura());
    printf("Tamanho árvore: %zu\n", arvore_alunos.get_tamanho());

    return 0;
}
