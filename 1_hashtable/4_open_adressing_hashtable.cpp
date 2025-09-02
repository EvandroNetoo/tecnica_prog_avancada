#include <cstdio>
#include <string>
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <functional>
#include <cmath>
#include <climits>

using namespace std;

template <typename Chave, typename Valor>
class Dict {
private:
    struct Item {
        Chave chave;
        Valor valor;
        bool deletado;
        Item() : chave(), valor(), deletado(true) {}
    };

    Item *tabela_hash;
    size_t tamanho;
    size_t capacidade;
    hash<Chave> funcao_hash;
    const float MAX_LOAD_FACTOR = 0.75;
    const float MIN_LOAD_FACTOR = 0.3;
    const size_t TAMANHO_MINIMO = 11;

    void criar_tabela_hash() {
        this->tabela_hash = new Item[this->capacidade];
    }

    class Iterator {
        Item *atual;
        Item *fim;

        void avancar() {
            while (atual < fim && atual->deletado) {
                ++atual;
            }
        }

    public:
        Iterator(Item *pos, Item *end) : atual(pos), fim(end) {
            avancar(); // pula deletados
        }

        tuple<Chave, Valor> operator*() const {
            return { atual->chave, atual->valor };
        }

        Iterator &operator++() {
            ++atual;
            avancar();
            return *this;
        }

        bool operator!=(const Iterator &outro) const {
            return atual != outro.atual;
        }


    };

    size_t obter_index(const Chave &chave) {
        return this->funcao_hash(chave) % this->capacidade;
    }

    Item &obter_item_para_inserir(const Chave &chave) {
        size_t index = obter_index(chave);
        size_t index_inicial = index;

        Item *primeiro_deletado = nullptr;

        while (true) {
            Item &item = this->tabela_hash[index];

            if (item.deletado) {
                return item;
            } else if (item.chave == chave) {
                return item; // chave já existe
            }

            index = (index + 1) % this->capacidade;
            if (index == index_inicial) {
                if (primeiro_deletado) return *primeiro_deletado;
                throw out_of_range("Tabela cheia");
            }
        }
    }

    Item &obter_item_existente(const Chave &chave) {
        size_t index = obter_index(chave);
        size_t index_inicial = index;

        while (true) {
            Item &item = this->tabela_hash[index];

            if (!item.deletado && item.chave == chave) {
                return item; // achou
            }

            index = (index + 1) % this->capacidade;
            if (index == index_inicial) {
                throw out_of_range("Chave não encontrada");
            }
        }
    }

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


    void rehash() {
        bool redimensionar = false;
        size_t antiga_capacidade = 0;

        float load = static_cast<float>(this->tamanho) / static_cast<float>(this->capacidade);

        if (load < this->MIN_LOAD_FACTOR && this->capacidade > this->TAMANHO_MINIMO) {
            antiga_capacidade = this->capacidade;
            this->capacidade /= 2;
            redimensionar = true;
        } else if (load > this->MAX_LOAD_FACTOR) {
            antiga_capacidade = this->capacidade;
            this->capacidade *= 2;
            redimensionar = true;
        }

        if (redimensionar) {
            this->capacidade = this->primo_mais_proximo(this->capacidade);

            Item *antiga_tabela_hash = this->tabela_hash;
            this->criar_tabela_hash();

            for (size_t i = 0; i < antiga_capacidade; i++) {
                Item &item = antiga_tabela_hash[i];
                if (!item.deletado) { // <=== CORREÇÃO
                    this->inserir(item.chave, item.valor);
                }
            }

            delete[] antiga_tabela_hash; // <=== CORREÇÃO
        }
    }

public:
    Dict() {
        this->capacidade = TAMANHO_MINIMO;
        this->criar_tabela_hash();
    }

    void inserir(const Chave chave, const Valor valor) {
        Item &item = this->obter_item_para_inserir(chave);
        item.chave = chave;
        item.valor = valor;
        item.deletado = false;
        this->tamanho++;
        rehash();
    }

    Valor em(const Chave &chave) {
        Item &item = this->obter_item_existente(chave);
        return item.valor;
    }

    void remover(const Chave &chave) {
        Item &item = this->obter_item_existente(chave);
        item.deletado = true;
        this->tamanho--;
        rehash();
    }

    Valor obter(const Chave &chave, const Valor valor_default) {
        try {
            return this->em(chave);
        }
        catch (const out_of_range &e) {
            return valor_default;
        }
    }


    Iterator begin() { return Iterator(tabela_hash, tabela_hash + capacidade); }
    Iterator end() { return Iterator(tabela_hash + capacidade, tabela_hash + capacidade); }
};



template <typename Chave>
class Set : private Dict<Chave, bool> {
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


template<typename K, typename V>
size_t count_items(Dict<K, V> &d) {
    size_t c = 0;
    for (auto it = d.begin(); it != d.end(); ++it) ++c;
    return c;
}

int main() {
    Dict<string, int> d;
    const int TOTAL = 1000000;        // quantidade total a inserir
    const int REMOVER_AT = 1000000;   // remover essa quantidade depois

    cout << ">>> Inserindo " << TOTAL << " itens (testando rehash-up)..." << endl;
    for (int i = 0; i < TOTAL; ++i) {
        string k = "k" + to_string(i);
        d.inserir(k, i);

        // checagens periódicas de integridade
        if ((i + 1) % 25 == 0) {
            cout << "  Inseridos: " << (i + 1) << "  |  iters encontrados: " << count_items(d) << endl;
            // garante que todos os já inseridos retornam o valor correto
            for (int j = 0; j <= i; ++j) {
                string kj = "k" + to_string(j);
                int val = d.obter(kj, INT_MIN);
                if (val != j) {
                    cerr << "ERRO: chave " << kj << " devolveu " << val << " (esperado " << j << ")" << endl;
                    return 1;
                }
            }
        }
    }

    cout << "Verificação pós-inserção OK. total via iterator = " << count_items(d) << endl;

    // sobrescreve um valor para testar atualização durante/apos rehash
    cout << ">>> Testando sobrescrita: k50 -> 5000" << endl;
    d.inserir("k50", 5000);
    if (d.obter("k50", -1) != 5000) {
        cerr << "ERRO: sobrescrita falhou para k50" << endl;
        return 1;
    }

    cout << ">>> Removendo " << REMOVER_AT << " itens (testando rehash-down)..." << endl;
    for (int i = 0; i < REMOVER_AT; ++i) {
        string k = "k" + to_string(i);
        try {
            d.remover(k);
        }
        catch (const out_of_range &e) {
            cerr << "Exceção ao remover " << k << ": " << e.what() << endl;
            return 1;
        }

        if ((i + 1) % 25 == 0) {
            cout << "  Removidos: " << (i + 1) << "  |  iters encontrados: " << count_items(d) << endl;
            // validações: removidos não existem; demais existem
            for (int j = 0; j <= i; ++j) {
                string kj = "k" + to_string(j);
                int got = d.obter(kj, INT_MIN);
                if (got != INT_MIN) {
                    cerr << "ERRO: chave removida " << kj << " ainda retorna " << got << endl;
                    return 1;
                }
            }
            for (int j = i + 1; j < TOTAL; ++j) {
                string kj = "k" + to_string(j);
                int got = d.obter(kj, INT_MIN);
                if (got != j && !(kj == "k50" && got == 5000)) { // k50 foi sobrescrita
                    cerr << "ERRO: chave restante " << kj << " devolveu " << got << " (esperado " << j << ")" << endl;
                    return 1;
                }
            }
        }
    }

    cout << "Verificação pós-remoção OK. itens finais via iterator = " << count_items(d) << endl;

    cout << "Itens restantes (iterando):" << endl;
    for (auto it = d.begin(); it != d.end(); ++it) {
        auto [k, v] = *it;
        cout << "  " << k << " -> " << v << endl;
    }

    cout << "\n>>> Teste de rehash concluído com sucesso (integridade mantida)." << endl;
    return 0;
}