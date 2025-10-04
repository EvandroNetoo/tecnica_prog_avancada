#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <random>
#include <sstream>


using namespace std;


template <typename T>
T random_choice(const vector<T> &v) {
    static random_device rd;
    static mt19937 gen(rd()); // Mersenne Twister
    uniform_int_distribution<> dist(0, v.size() - 1);
    return v[dist(gen)];
}

template <typename Valor>
class Grafo {
private:
    vector<vector<bool>> matriz;
    vector<Valor> valores;
    bool direcionado;
    size_t tamanho;


    string gerar_dot() {
        ostringstream dot;

        const string TAB = "    ";
        const string aresta = direcionado ? " -> " : " -- ";
        const string tipo_grafo = direcionado ? "digraph" : "graph";

        dot << tipo_grafo << " G {" << endl;

        // Adicionar nós
        for (size_t i = 0; i < tamanho; i++)
            dot << TAB << valores[i] << ";" << endl;


        // Adicionar arestas
        for (size_t i = 0; i < tamanho; i++)
            for (size_t j = 0; j < tamanho; j++)
                if (matriz[i][j])
                    if (direcionado || i < j)
                        dot << TAB << valores[i] << aresta << valores[j] << ";" << endl;

        dot << "}" << endl;

        return dot.str();
    }
    
public:
    Grafo(bool direcionado = false) {
        this->direcionado = direcionado;
        this->matriz = vector<vector<bool>>();
        this->valores = vector<Valor>();
        this->tamanho = 0;
    }

    bool add_no(Valor valor) {
        for (const Valor &v : valores) {
            if (v == valor) {
                return false;
            }
        }

        this->tamanho++;
        this->valores.push_back(valor);

        matriz.resize(this->tamanho);

        for (vector<bool> &linha : matriz) {
            linha.resize(this->tamanho);
        }

        return true;
    }

    bool add_aresta(Valor no1, Valor no2) {
        if (no1 == no2) {
            return false;
        }

        auto it1 = find(valores.begin(), valores.end(), no1);
        auto it2 = find(valores.begin(), valores.end(), no2);

        if (it1 == valores.end() || it2 == valores.end()) {
            return false;  // Nó não existe
        }

        size_t idx1 = distance(valores.begin(), it1);
        size_t idx2 = distance(valores.begin(), it2);

        if (matriz[idx1][idx2]) {
            return false;  // Aresta ja existe
        }

        matriz[idx1][idx2] = true;
        if (!direcionado) {
            matriz[idx2][idx1] = true;
        }

        return true;
    }

    size_t qtd_arestas_completo() {
        if (this->direcionado) {
            return this->tamanho * (this->tamanho - 1);
        }
        return (this->tamanho * (this->tamanho - 1)) / 2;
    }

    static Grafo gerar_aleatorio(bool direcionado, size_t qtd_nos, unsigned int porcentagem_arestas_total = 100) {
        Grafo<Valor> grafo(direcionado);

        for (size_t i = 0; i < qtd_nos; i++) {
            while (!grafo.add_no(static_cast<Valor>(rand())));
        }

        vector<Valor> valores = grafo.get_valores();
        for (size_t i = 0; i < grafo.qtd_arestas_completo() * porcentagem_arestas_total / 100; i++) {
            while (!grafo.add_aresta(random_choice(valores), random_choice(valores)));
        }

        return grafo;
    }

    void print() {
        cout << "Valores dos nós: ";
        for (size_t i = 0; i < valores.size(); i++) {
            cout << valores[i];
            if (i < valores.size() - 1) cout << " ";
        }
        cout << endl;

        cout << "Matriz de adjacência:" << endl;
        for (size_t i = 0; i < tamanho; i++) {
            for (size_t j = 0; j < tamanho; j++) {
                if (i < matriz.size() && j < matriz[i].size()) {
                    cout << (matriz[i][j] ? "1" : "0") << " ";
                } else {
                    cout << "0 ";
                }
            }
            cout << endl;
        }
    }

    vector<Valor> get_valores() {
        return this->valores;
    }

    void exportar_para_dot(const string &nome_arquivo) {
        ofstream arquivo(nome_arquivo);
        if (!arquivo.is_open()) {
            cerr << "Erro ao abrir arquivo: " << nome_arquivo << endl;
            return;
        }

        arquivo << gerar_dot();
        arquivo.close();
    }

    void exportar_para_png(const string &arquivo_png) {
        string comando = "dot -Tpng -o " + arquivo_png;
        FILE *pipe = popen(comando.c_str(), "w");
        if (!pipe) {
            cerr << "Erro ao executar Graphviz.\n";
            return;
        }

        string conteudo_dot = gerar_dot();
        fwrite(conteudo_dot.c_str(), 1, conteudo_dot.size(), pipe);
        pclose(pipe);
    }
};


int main() {

    Grafo<int> grafo = Grafo<int>::gerar_aleatorio(true, 20, 20);
    grafo.print();
    grafo.exportar_para_dot(".dot");
    grafo.exportar_para_png(".png");

    return 0;
}