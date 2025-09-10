#include <cstdio>
#include <string>
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
#include <functional>
#include <cmath>
#include <optional>
#include <climits>

using namespace std;

template <typename Chave, typename Valor>
class Dict {
protected:
    struct Item {
        Chave chave;
        Valor valor;
        bool oculpado;
        bool deletado;
        Item() : chave(), valor(), oculpado(false), deletado(false) {}
    };

    vector<Item> tabela_hash;
    size_t tamanho;
    size_t capacidade;
    hash<Chave> funcao_hash;
    const float MAX_LOAD_FACTOR = 0.75;
    const float MIN_LOAD_FACTOR = 0.3;
    const size_t TAMANHO_MINIMO = 11;

    class Iterator {
        vector<Item> *tabela;
        size_t pos;
        size_t fim;

        void avancar() {
            while (pos < fim && ((*tabela)[pos].deletado || !(*tabela)[pos].oculpado)) {
                ++pos;
            }
        }

    public:
        Iterator(vector<Item> *tab, size_t position, size_t end)
            : tabela(tab), pos(position), fim(end) {
            avancar(); // pula deletados e vazios
        }

        pair<const Chave &, Valor &> operator*() const {
            return { (*tabela)[pos].chave, (*tabela)[pos].valor };
        }

        Iterator &operator++() {
            ++pos;
            avancar();
            return *this;
        }

        bool operator!=(const Iterator &outro) const {
            return pos != outro.pos || tabela != outro.tabela;
        }
    };

    size_t primo_mais_proximo(size_t n) {
        if (n & 1) {
            n -= 2;
        } else {
            n--;
        }

        size_t i, j;
        for (i = n; i >= 2; i -= 2) {
            if (i % 2 == 0)
                continue;
            for (j = 3; j <= sqrt(i); j += 2) {
                if (i % j == 0)
                    break;
            }
            if (j > sqrt(i))
                return i;
        }

        return 2;
    }


    
    size_t obter_index(const Chave &chave) {
        return this->funcao_hash(chave) % this->capacidade;
    }
    
    size_t rehash(const size_t &hash1, const size_t &capacidade, int tentativa) {
        size_t hash2 = 1 + (hash1 % (capacidade - 1));
        return (hash1 + tentativa * hash2) % capacidade;
    }

    void redimencionar() {
        size_t nova_capacidade = 0;

        float load = this->tamanho / (float)this->capacidade;

        if (load < this->MIN_LOAD_FACTOR && this->capacidade > this->TAMANHO_MINIMO) {
            nova_capacidade = this->capacidade / 2;
        } else if (load > this->MAX_LOAD_FACTOR) {
            nova_capacidade = this->capacidade * 2;
        }
        if (nova_capacidade != 0) {
            nova_capacidade = max(this->primo_mais_proximo(nova_capacidade), this->TAMANHO_MINIMO);

            vector<Item> nova_tabela_hash(nova_capacidade);

            for (const Item &item : this->tabela_hash) {
                if (item.oculpado && !item.deletado) {
                    size_t h1 = funcao_hash(item.chave) % nova_capacidade;
                    size_t index = h1;
                    int tentativa = 0;
                    while (nova_tabela_hash[index].oculpado) {
                        index = this->rehash(h1, nova_capacidade, ++tentativa);
                    }
                    nova_tabela_hash[index] = item;
                }
            }
            this->tabela_hash = move(nova_tabela_hash);
            this->capacidade = nova_capacidade;
        }
    }

    Item &obter_item_para_inserir(const Chave &chave) {
        size_t h1 = obter_index(chave);
        size_t index = h1;

        Item *item = &this->tabela_hash[index];

        int tentativa = 0;
        while (item->oculpado && !item->deletado && item->chave != chave) {
            index = this->rehash(h1, this->capacidade, ++tentativa);
            item = &this->tabela_hash[index];
            this->count_colisao++;
        }
        return *item;
    }

    Item &obter_item_existente(const Chave &chave) {
        size_t h1 = obter_index(chave);
        size_t index = h1;

        int tentativa = 0;
        while (true) {
            Item &item = this->tabela_hash[index];

            if (item.oculpado) {
                if (!item.deletado && item.chave == chave) {
                    return item;
                }
            } else {
                throw out_of_range("Chave não encontrada");
            }

            index = this->rehash(h1, this->capacidade, ++tentativa);
        }
    }

public:
    Dict() {
        this->capacidade = TAMANHO_MINIMO;
        this->tabela_hash.resize(this->capacidade);
        this->tamanho = 0;
    }

    size_t count_colisao = 0;
    void inserir(const Chave &chave, const Valor &valor) {
        Item &item = this->obter_item_para_inserir(chave);
        if (!item.oculpado || item.deletado) {
            item.chave = chave;
            item.oculpado = true;
            item.deletado = false;
            this->tamanho++;
        }
        item.valor = valor;
        this->redimencionar();
    }

    const Valor& em(const Chave &chave) {
        Item &item = this->obter_item_existente(chave);
        return item.valor;
    }

    void remover(const Chave &chave) {
        Item &item = this->obter_item_existente(chave);
        item.deletado = true;
        this->tamanho--;
        this->redimencionar();
    }

    optional<Valor> obter(const Chave &chave, optional<Valor> valor_default = nullopt) {
        try {
            return this->em(chave);
        }
        catch (const out_of_range &) {
            return valor_default;
        }
    }

    size_t obter_tamanho() {
        return this->tamanho;
    }

    size_t obter_capacidade() {
        return this->capacidade;
    }

    Iterator begin() { return Iterator(&tabela_hash, 0, tabela_hash.size()); }
    Iterator end() { return Iterator(&tabela_hash, tabela_hash.size(), tabela_hash.size()); }
};



template <typename Valor>
class Set : protected Dict<Valor, bool> {
protected:
    // Iterator do Set
    class Iterator {
        using DictIterator = typename Dict<Valor, bool>::Iterator;
        DictIterator it;

    public:
        Iterator(const DictIterator &dict_it) : it(dict_it) {}

        const Valor &operator*() const { return (*it).first; } // retorna só a chave (valor do Set)
        Iterator &operator++() { ++it; return *this; }
        bool operator!=(const Iterator &outro) const { return it != outro.it; }
    };

public:
    Set() {}

    void adicionar(const Valor &valor) {
        this->inserir(valor, true);
    }

    bool contem(const Valor &valor) {
        try {
            return this->em(valor);
        }
        catch (const out_of_range &) {
            return false;
        }
    }

    void remover(const Valor &valor) {
        try {
            Dict<Valor, bool>::remover(valor);
        }
        catch (const out_of_range &) {}
    }


    Iterator begin() { return Iterator(Dict<Valor, bool>::begin()); }
    Iterator end() { return Iterator(Dict<Valor, bool>::end()); }
};

struct Aluno {
    string matricula;
    string cpf;
    string nome;
    float nota;
    int idade;
    string curso;
    string cidade;
};

namespace std {
    template <>
    struct hash<Aluno> {
        size_t operator()(const Aluno &aluno) const noexcept {
            hash<string> hash_str;
            return hash_str(aluno.nome);
        }
    };
}

void carregar_lista_de_alunos(Dict<string, Aluno *> &dict_alunos, const string &nome_arquivo) {
    ifstream arquivo(nome_arquivo);
    string linha;

    getline(arquivo, linha);  // ignora cabeçalho


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

        Aluno *novo_aluno = new Aluno{ colunas[0], colunas[1], colunas[2], stof(colunas[3]), stoi(colunas[4]), colunas[5], colunas[6] };
        string nome = colunas[2];

        dict_alunos.inserir(nome, novo_aluno);

    }
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

int main() {
    clock_t inicio, fim, total = 0;
    Dict<string, Aluno *> dict_alunos;

    inicio = clock();
    carregar_lista_de_alunos(dict_alunos, "../alunos_completos.csv");
    fim = clock();
    printf("Inserir alunos: %f segundos\n", double(fim - inicio) / CLOCKS_PER_SEC);
    total += fim - inicio;

    printf("Tamanho: %zu\nCapacidade: %zu\n", dict_alunos.obter_tamanho(), dict_alunos.obter_capacidade());

    cout << "Colisões: " << dict_alunos.count_colisao << endl;

    imprime_aluno(dict_alunos.em("José Porto Pardo"));
    return 0;
}
