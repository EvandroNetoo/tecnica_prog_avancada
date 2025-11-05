#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <random>
#include <sstream>
#include <queue>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <tuple>
#include <ctime>
#include <algorithm>

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

    unordered_map<Valor, unordered_map<Valor, float>> grafo;

    bool direcionado;
    size_t tamanho;

    string gerar_dot() {
        ostringstream dot;

        const string TAB = "    ";
        const string aresta = direcionado ? " -> " : " -- ";
        const string tipo_grafo = direcionado ? "digraph" : "graph";

        dot << tipo_grafo << " G {" << endl;

        // Adicionar nós
        for (auto &[valor, vizinhos] : this->grafo)
            dot << TAB << valor << ";" << endl;


        // Adicionar arestas
        set<pair<Valor, Valor>> ja_impressos;
        for (auto &[valor, vizinhos] : this->grafo)
            for (auto &[vizinho, peso] : vizinhos) {
                if (ja_impressos.count({ valor, vizinho })) continue;
                dot << TAB << valor << aresta << vizinho;
                dot << " [label=" << this->grafo[valor][vizinho] << ",weight=" << this->grafo[valor][vizinho] << "]";
                dot << ";" << endl;
                ja_impressos.insert({ valor, vizinho });
                if (!this->direcionado) ja_impressos.insert({ vizinho, valor });
            }

        dot << "}" << endl;

        return dot.str();
    }

    bool grafo_e_conexo(bool transposto = false) {
        vector<Valor> valores = this->get_valores();
        if (valores.empty()) return true;

        unordered_set<Valor> visitados;
        queue<Valor> a_visitar;

        a_visitar.push(valores[0]);
        visitados.insert(valores[0]);

        while (!a_visitar.empty()) {
            Valor valor = a_visitar.front();
            a_visitar.pop();


            for (Valor possivel_vizinho : valores)
                if (transposto ? this->existe_aresta(possivel_vizinho, valor) : this->existe_aresta(valor, possivel_vizinho)) {
                    if (visitados.find(possivel_vizinho) == visitados.end()) {
                        a_visitar.push(possivel_vizinho);
                        visitados.insert(possivel_vizinho);
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
        this->grafo.clear();
        this->tamanho = 0;
    }

    bool add_no(Valor valor) {
        if (grafo.find(valor) != grafo.end())
            return false;

        this->grafo[valor] = unordered_map<Valor, float>();

        this->tamanho++;

        return true;
    }

    bool add_aresta(Valor no1, Valor no2, float peso) {
        if (no1 == no2)
            return false;

        if (this->grafo.find(no1) == this->grafo.end())
            this->add_no(no1);

        if (this->grafo.find(no2) == this->grafo.end())
            this->add_no(no2);

        this->grafo[no1][no2] = peso;
        if (!direcionado)
            this->grafo[no2][no1] = peso;

        return true;
    }

    bool existe_aresta(Valor no1, Valor no2) {
        if (this->grafo.find(no1) == this->grafo.end()) {
            return false;
        }
        if (this->grafo[no1].find(no2) == this->grafo[no1].end()) {
            return false;
        }
        return true;
    }

    size_t qtd_arestas_completo() {
        size_t quantidade = this->tamanho * (this->tamanho - 1);
        return this->direcionado ? quantidade : quantidade / 2;
    }

    static Grafo<Valor> a_partir_de_aleatorio(bool direcionado, size_t qtd_nos, unsigned int porcentagem_arestas_total = 100) {
        static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
        std::uniform_int_distribution<int> peso_dist(1, 10);

        Grafo<Valor> grafo(direcionado);

        for (size_t i = 0; i < qtd_nos; i++) {
            Valor valor;
            do {
                valor = static_cast<Valor>(rng());
            }
            while (!grafo.add_no(valor));
        }

        size_t num_arestas = grafo.qtd_arestas_completo() * porcentagem_arestas_total / 100;
        auto valores = grafo.get_valores();
        std::uniform_int_distribution<size_t> idx_dist(0, valores.size() - 1);

        for (size_t i = 0; i < num_arestas; i++) {
            Valor no1, no2;
            do {
                no1 = valores[idx_dist(rng)];
                no2 = valores[idx_dist(rng)];
            }
            while (no1 == no2 || grafo.existe_aresta(no1, no2));

            grafo.add_aresta(no1, no2, static_cast<float>(peso_dist(rng)));
        }

        return grafo;
    }

    static Grafo<string> a_partir_de_arquivo_dot(const string &arquivo) {
        Grafo<string> grafo(false);

        ifstream in(arquivo);
        if (!in.is_open())
            return grafo;
    }

    void print() {
        cout << (direcionado ? "Dígrafo" : "Grafo") << " | nós: " << grafo.size();

        size_t total_arestas = 0;
        for (const auto &kv : grafo) total_arestas += kv.second.size();
        cout << " | arestas: " << (direcionado ? total_arestas : total_arestas / 2) << endl;

        // Lista de nós
        cout << "Nós: ";
        {
            bool first = true;
            for (const auto &kv : grafo) {
                if (!first) cout << ", ";
                cout << kv.first;
                first = false;
            }
            if (grafo.empty()) cout << "(vazio)";
            cout << endl;
        }

        // Lista de adjacências com pesos
        for (const auto &[u, adj] : grafo) {
            cout << u << " -> ";
            if (adj.empty()) {
                cout << "{}" << endl;
                continue;
            }
            cout << "{";
            bool first = true;
            for (const auto &[v, peso] : adj) {
                if (!first) cout << ", ";
                cout << v << ": " << peso;
                first = false;
            }
            cout << "}" << endl;
        }
    }

    vector<Valor> get_valores() {
        vector<Valor> valores;
        for (auto &[k, v] : this->grafo)
            valores.push_back(k);
        return valores;
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


    Grafo<Valor> arvore_geradora_minima_prim() {
        Grafo<Valor> arvore_geradora_minima;
        vector<Valor> valores = this->get_valores();
        if (valores.empty() || !this->e_conexo()) return arvore_geradora_minima;

        unordered_set<Valor> visitados;
        queue<Valor> a_visitar;

        visitados.insert(valores[0]);
        arvore_geradora_minima.add_no(valores[0]);

        while (visitados.size() != valores.size()) {
            tuple<Valor, Valor, float> a_adicionar = { Valor(), Valor(), numeric_limits<float>::infinity() };
            for (Valor valor : visitados)
                for (auto &[vizinho, peso] : this->grafo[valor])
                    if (visitados.find(vizinho) == visitados.end() && peso < get<2>(a_adicionar))
                        a_adicionar = make_tuple(valor, vizinho, peso);

            arvore_geradora_minima.add_aresta(get<0>(a_adicionar), get<1>(a_adicionar), get<2>(a_adicionar));
            visitados.insert(get<1>(a_adicionar));
        }

        return arvore_geradora_minima;
    }

    Grafo<Valor> arvore_geradora_minima_kruskel() {
        Grafo<Valor> arvore_geradora_minima;
        vector<Valor> valores = this->get_valores();
        if (valores.empty() || !this->e_conexo()) return arvore_geradora_minima;

        unordered_map<Valor, int> subgrafos;
        vector<tuple<int, Valor, Valor>> arestas;

        int subgrafo_atual = 0;
        for (auto &[vertice, vizinhos] : this->grafo) {
            subgrafos[vertice] = ++subgrafo_atual;
            for (auto &[vizinho, peso] : vizinhos)
                arestas.push_back(make_tuple(peso, vertice, vizinho));
        }

        sort(
            arestas.begin(),
            arestas.end(),
            [](const auto &a, const auto &b) {
                return get<0>(a) < get<0>(b);
            }
        );


        for (const auto &[peso, v1, v2] : arestas) {
            if (subgrafos[v1] != subgrafos[v2]) {
                arvore_geradora_minima.add_aresta(v1, v2, peso);
                int novo_subgrafo = ++subgrafo_atual;
                int subgrafo_v1 = subgrafos[v1];
                int subgrafo_v2 = subgrafos[v2];

                for (auto &[vertice, subgrafo] : subgrafos) {
                    if (subgrafo == subgrafo_v1 || subgrafo == subgrafo_v2)
                        subgrafos[vertice] = novo_subgrafo;
                }
            }
        }

        return arvore_geradora_minima;
    }
};


int main() {

    Grafo<int> grafo = Grafo<int>::a_partir_de_aleatorio(false, 5, 70);
    grafo.print();
    grafo.exportar_para_dot("grafo.dot");
    grafo.exportar_para_png("grafo.png");

    cout << (grafo.e_conexo() ? "CONEXO" : "NÃO CONEXO") << endl;

    grafo.arvore_geradora_minima_prim().exportar_para_png("prim.png");
    grafo.arvore_geradora_minima_kruskel().exportar_para_png("kruskel.png");

    return 0;
}