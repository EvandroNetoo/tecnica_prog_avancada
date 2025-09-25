#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <optional>
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
class ArvoreBinariaVetor {
private:
    size_t get_i_esq(size_t indice) {
        return indice * 2 + 1;
    }

    size_t get_i_dir(size_t indice) {
        return indice * 2 + 2;
    }

    void trocar_rec(size_t origem, size_t destino) {
        optional<Valor> &no_origem = this->dados[origem];
        optional<Valor> &no_destino = this->dados[destino];

        if (!no_destino.has_value()) return;

        Valor &valor_no_destino = no_destino.value();
        if (no_origem.has_value()) {
            no_destino = no_origem.value();
        } else {
            no_destino.reset();
        }
        no_origem = valor_no_destino;

        trocar_rec(get_i_esq(origem), get_i_esq(destino));
        trocar_rec(get_i_dir(origem), get_i_dir(destino));
    }

    bool  inserir_rec(const Valor &valor, size_t indice) {
        if (indice >= this->capacidade) return false;

        optional<Valor> &no = this->dados[indice];

        if (!no.has_value()) {
            no = valor;
            return true;
        } else if (valor < no.value()) {
            return this->inserir_rec(valor, get_i_esq(indice));
        } else if (valor > no.value()) {
            return this->inserir_rec(valor, get_i_dir(indice));
        } else {
            if (!this->inserir_repetido) return false;
            return this->inserir_rec(valor, get_i_dir(indice));
        }
    }

    optional<Valor> *buscar_no_rec(const Valor &valor, size_t indice) {
        if (indice >= this->capacidade) return nullptr;

        optional<Valor> &no = this->dados[indice];

        if (!no.has_value()) return nullptr;
        if (valor == no.value()) return &no;
        if (valor < no.value()) return buscar_no_rec(valor, get_i_esq(indice));
        return buscar_no_rec(valor, get_i_dir(indice));
    }

    bool remover_rec(const Valor &valor, const size_t indice) {
        if (indice >= this->capacidade) return false;

        size_t i_esq = get_i_esq(indice), i_dir = get_i_dir(indice);
        optional<Valor> &no = this->dados[indice];
        optional<Valor> &no_esq = this->dados[i_esq];
        optional<Valor> &no_dir = this->dados[i_dir];

        if (!no.has_value()) {
            return false;
        }

        if (valor < no.value()) {
            return remover_rec(valor, i_esq);
        } else if (valor > no.value()) {
            return remover_rec(valor, i_dir);
        } else {
            // Caso 1: Nó folha
            if (!no_esq.has_value() && !no_dir.has_value()) {
                no.reset();
                return true;
            }
            // Caso 2: Nó com apenas um filho
            else if (!no_esq.has_value()) {
                no.reset();
                trocar_rec(indice, i_dir);
                return true;
            } else if (!no_dir.has_value()) {
                no.reset();
                trocar_rec(indice, i_esq);
                return true;
            }
            // Caso 3: Nó com dois filhos
            else {
                // Encontrar o sucessor (menor valor à direita)
                size_t i_menor_dir = get_i_dir(indice);
                while (this->dados[get_i_esq(i_menor_dir)].has_value()) {
                    i_menor_dir = get_i_esq(i_menor_dir);
                }

                optional<Valor> &no_menor_dir = this->dados[i_menor_dir];
                no = no_menor_dir.value();
                // Remover o sucessor
                return remover_rec(no_menor_dir.value(), get_i_dir(indice));
            }
        }
    }

    int altura_rec(const size_t indice) {
        if (indice >= this->capacidade) return 0;
        optional<Valor> &no = this->dados[indice];
        if (!no.has_value()) return 0;
        return 1 + max<int>(this->altura_rec(get_i_esq(indice)), this->altura_rec(get_i_dir(indice)));
    }


    // --- Percursos ---
    void pre_ordem_rec(size_t indice) {
        if (indice >= this->capacidade) return;

        optional<Valor> &no = this->dados[indice];
        if (!no.has_value()) return;
        cout << no.value() << endl;
        pre_ordem_rec(get_i_esq(indice));
        pre_ordem_rec(get_i_dir(indice));
    }

    void em_ordem_rec(size_t indice) {
        if (indice >= this->capacidade) return;

        optional<Valor> &no = this->dados[indice];
        if (!no.has_value()) return;
        em_ordem_rec(get_i_esq(indice));
        cout << no.value() << endl;
        em_ordem_rec(get_i_dir(indice));
    }

    void pos_ordem_rec(size_t indice) {
        if (indice >= this->capacidade) return;

        optional<Valor> &no = this->dados[indice];
        if (!no.has_value()) return;
        pos_ordem_rec(get_i_esq(indice));
        pos_ordem_rec(get_i_dir(indice));
        cout << no.value() << endl;
    }

    size_t tamanho;
    size_t capacidade;
    bool inserir_repetido;
public:
    vector<optional<Valor>> dados;
    ArvoreBinariaVetor(size_t capacidade, bool inserir_repetido = true) {
        this->inserir_repetido = inserir_repetido;
        this->tamanho = 0;
        this->capacidade = capacidade;
        this->dados.resize(capacidade);
    }

    ~ArvoreBinariaVetor() {
    }


    bool inserir(const Valor &valor) {
        bool inseriu = this->inserir_rec(valor, 0);
        if (inseriu) {
            this->tamanho++;
        }
        return inseriu;
    }

    const Valor &buscar_valor(const Valor &valor) {
        optional<Valor> *no = this->buscar_no_rec(valor, 0);
        if (no == nullptr || !no->has_value()) {
            throw out_of_range("Valor não encontrado");
        }
        return no->value();
    }

    optional<Valor> *buscar_no(const Valor &valor) {
        return this->buscar_no_rec(valor, 0);
    }

    bool remover(const Valor &valor) {
        bool removeu = this->remover_rec(valor, 0);
        if (removeu) {
            this->tamanho--;
        }
        return removeu;
    }

    size_t get_tamanho() {
        return this->tamanho;
    }

    int altura() {
        return altura_rec(0) - 1;
    }

    // --- Percursos ---
    void pre_ordem() {
        this->pre_ordem_rec(0);
    }

    void em_ordem() {
        this->em_ordem_rec(0);
    }

    void pos_ordem() {
        this->pos_ordem_rec(0);
    }
    //secondary function
    void desenha_arvore_horiz(size_t indice, int depth, char *path, bool direita, int espacoEntreNiveis = 6)
    {
        // stopping condition
        if (indice >= this->capacidade || !this->dados[indice].has_value())
            return;

        // increase spacing
        depth++;

        // start with direita no
        desenha_arvore_horiz(get_i_dir(indice), depth, path, true, espacoEntreNiveis);

        // set | draw map
        path[depth - 2] = 0;

        if (direita)
            path[depth - 2] = 1;

        size_t i_esq = get_i_esq(indice);
        if (i_esq < this->capacidade && this->dados[i_esq].has_value())
            path[depth - 1] = 1;

        // print root after spacing
        printf("\n");

        for (int i = 0; i < depth - 1; i++)
        {
            if (i == depth - 2)
                printf("+");
            else if (path[i])
                printf("|");
            else
                printf(" ");

            for (int j = 1; j < espacoEntreNiveis; j++)
                if (i < depth - 2)
                    printf(" ");
                else
                    printf("-");
        }

        cout << this->dados[indice].value() << "\n";

        if (depth == 1)
            cout << "(raiz)\n";

        // vertical espacors below
        for (int i = 0; i < depth; i++)
        {
            if (path[i])
                printf("|");
            else
                printf(" ");

            for (int j = 1; j < espacoEntreNiveis; j++)
                printf(" ");
        }

        // go to esquerda no
        desenha_arvore_horiz(get_i_esq(indice), depth, path, false);
    }

    //primary function
    void draw_arvore_hor()
    {
        // should check if we don't exceed this somehow..
        char path[255] = {};

        //initial depth is 0
        desenha_arvore_horiz(0, 0, path, false);
    }
};



void carregar_lista_de_alunos(ArvoreBinariaVetor<Aluno> &arvore_alunos, const string &nome_arquivo) {
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

        if (!arvore_alunos.inserir(novo_aluno)) return;
    }
}


int main() {
    ArvoreBinariaVetor<int> a(pow(2, 5));

    a.inserir(4);
    a.inserir(3);
    a.inserir(7);
    a.inserir(6);
    a.inserir(10);
    // a.inserir(9);
    a.inserir(12);
    a.inserir(11);

    a.remover(7);

    a.draw_arvore_hor();
    cout << "[";
    for (size_t i = 0; i < a.dados.size(); ++i) {
        if (i > 0) cout << ", ";
        if (a.dados[i].has_value())
            cout << a.dados[i].value();
        else
            cout << "None";
    }
    cout << "]" << endl;


    // clock_t inicio, fim, total = 0;
    // ArvoreBinariaVetor<Aluno> arvore_alunos(pow(2, 20));

    // inicio = clock();
    // carregar_lista_de_alunos(arvore_alunos, "../alunos_completos.csv");
    // fim = clock();
    // total += fim - inicio;
    // printf("Inserir alunos: %f segundos\n", double(total) / CLOCKS_PER_SEC);
    // printf("Altura árvore: %d\n", arvore_alunos.altura());
    // printf("Tamanho árvore: %zu\n", arvore_alunos.get_tamanho());

    return 0;
}
