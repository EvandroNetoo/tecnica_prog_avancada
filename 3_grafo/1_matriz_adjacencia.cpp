#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <random>
#include <sstream>
#include <unordered_set>
#include <queue>


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

    bool grafo_e_conexo(bool transposto = false) {
        if (valores.empty()) return true;

        unordered_set<Valor> visitados;
        queue<Valor> a_visitar;

        a_visitar.push(valores[0]);
        visitados.insert(valores[0]);

        while (!a_visitar.empty()) {
            Valor valor = a_visitar.front();
            a_visitar.pop();

            size_t index = distance(valores.begin(), find(valores.begin(), valores.end(), valor));

            for (size_t i = 0; i < tamanho; i++)
                if (transposto ? matriz[i][index] : matriz[index][i]) {
                    Valor vizinho = valores[i];
                    if (!visitados.count(vizinho)) {
                        a_visitar.push(vizinho);
                        visitados.insert(vizinho);
                    }
                }

        }

        return visitados.size() == valores.size();
    }

    bool digrafo_e_conexo() {
        return grafo_e_conexo() && grafo_e_conexo(true);
    }

public:
    Grafo(bool direcionado = false) {
        this->direcionado = direcionado;
    }

    bool add_no(Valor valor) {
        for (const Valor &v : valores)
            if (v == valor)
                return false;


        this->tamanho++;
        this->valores.push_back(valor);

        matriz.resize(this->tamanho);

        for (vector<bool> &linha : matriz)
            linha.resize(this->tamanho);

        return true;
    }

    bool add_aresta(Valor no1, Valor no2) {
        if (no1 == no2)
            return false;


        auto it1 = find(valores.begin(), valores.end(), no1);
        auto it2 = find(valores.begin(), valores.end(), no2);

        if (it1 == valores.end() || it2 == valores.end())
            return false;  // Nó não existe


        size_t idx1 = distance(valores.begin(), it1);
        size_t idx2 = distance(valores.begin(), it2);

        if (matriz[idx1][idx2])
            return false;  // Aresta ja existe


        matriz[idx1][idx2] = true;
        if (!direcionado)
            matriz[idx2][idx1] = true;


        return true;
    }

    size_t qtd_arestas_completo() {
        if (this->direcionado)
            return this->tamanho * (this->tamanho - 1);

        return (this->tamanho * (this->tamanho - 1)) / 2;
    }

    static Grafo<Valor> a_partir_de_aleatorio(bool direcionado, size_t qtd_nos, unsigned int porcentagem_arestas_total = 100) {
        Grafo<Valor> grafo(direcionado);

        for (size_t i = 0; i < qtd_nos; i++)
            while (!grafo.add_no(static_cast<Valor>(rand())));

        vector<Valor> valores = grafo.get_valores();
        for (size_t i = 0; i < grafo.qtd_arestas_completo() * porcentagem_arestas_total / 100; i++)
            while (!grafo.add_aresta(random_choice(valores), random_choice(valores)));


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

    bool e_conexo() {
        return direcionado ? digrafo_e_conexo() : grafo_e_conexo();
    }
};


int main() {

    Grafo<int> grafo = Grafo<int>::a_partir_de_aleatorio(false, 5, 40);
    // Grafo<string> grafo = Grafo<string>::a_partir_de_arquivo_dot(".dot");

    grafo.print();
    grafo.exportar_para_dot(".dot");
    grafo.exportar_para_png(".png");

    cout << (grafo.e_conexo() ? "CONEXO" : "NÃO CONEXO") << endl;

    return 0;
}