#include <cstdio>
#include <string>
#include <ios>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <functional>

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
    size_t capacidade;
    hash<Chave> funcao_hash;

    static constexpr size_t TAMANHO_MINIMO = 11;

    Item *criar_tabela(size_t tamanho) {
        return new Item[tamanho];
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

    size_t get_index(const Chave &chave) {
        return this->funcao_hash(chave) % this->capacidade;
    }

    Item &get_item(const Chave &chave) {
        Item &item = this->tabela_hash[get_index(chave)];
        return item;
    }
    
public:
    Dict() {
        tabela_hash = criar_tabela(TAMANHO_MINIMO);
        capacidade = TAMANHO_MINIMO;
    }

    Valor &operator[](const Chave &chave) {
        return this->get_item(chave).valor;
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


int main() {
    Dict<string, int> dict;
    dict["asdf"] = 3;

    for (auto [chave, valor] : dict) {
        cout << chave << " -> " << valor << "\n";
    }

    return 0;
}