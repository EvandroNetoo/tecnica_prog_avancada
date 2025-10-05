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
                    if (visitados.find(vizinho) == visitados.end()) {
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
        this->matriz = vector<vector<bool>>();
        this->valores = vector<Valor>();
        this->tamanho = 0;
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

    static Grafo<string> a_partir_de_arquivo_dot(const string &arquivo) { // IA fez esse método
        Grafo<string> grafo(false);

        ifstream in(arquivo);
        if (!in.is_open())
            return grafo;

        // Read whole file
        ostringstream buffer;
        buffer << in.rdbuf();
        string content = buffer.str();
        in.close();

        // detect direcionado
        bool direcionado = content.find("digraph") != string::npos;
        grafo = Grafo<string>(direcionado);

        // remove block comments /* ... */
        size_t s = 0;
        while ((s = content.find("/*", s)) != string::npos) {
            size_t e = content.find("*/", s + 2);
            if (e == string::npos) {
                content.erase(s);
                break;
            }
            content.erase(s, e - s + 2);
        }

        // remove line comments // ...\n
        size_t pos = 0;
        while ((pos = content.find("//", pos)) != string::npos) {
            size_t e = content.find('\n', pos + 2);
            if (e == string::npos) {
                content.erase(pos);
                break;
            }
            content.erase(pos, e - pos);
        }

        // normalize newlines to spaces
        for (char &c : content) if (c == '\n' || c == '\r') c = ' ';

        auto trim = [&](string s) {
            // trim both ends
            const char *ws = " \t\n\r";
            size_t start = s.find_first_not_of(ws);
            if (start == string::npos) return string();
            size_t end = s.find_last_not_of(ws);
            return s.substr(start, end - start + 1);
            };

        auto strip_quotes = [&](string s) {
            s = trim(s);
            if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\'')))
                return s.substr(1, s.size() - 2);
            return s;
            };

        auto clean_token = [&](string tok) {
            tok = trim(tok);
            // remove attributes between [ ]
            size_t br = tok.find('[');
            if (br != string::npos) tok = tok.substr(0, br);
            // if contains comma-separated nodes, take first after split
            // but we'll handle commas elsewhere; remove trailing commas
            while (!tok.empty() && (tok.back() == ',' || tok.back() == ';')) tok.pop_back();
            // take first whitespace-separated piece (node names usually one token)
            size_t sp = tok.find_first_of(" \t");
            if (sp != string::npos) tok = tok.substr(0, sp);
            tok = strip_quotes(tok);
            return trim(tok);
            };

        // split by semicolon
        size_t start = 0;
        while (start < content.size()) {
            size_t semi = content.find(';', start);
            string stmt;
            if (semi == string::npos) {
                stmt = content.substr(start);
                start = content.size();
            } else {
                stmt = content.substr(start, semi - start);
                start = semi + 1;
            }

            stmt = trim(stmt);
            if (stmt.empty()) continue;

            // skip the header like "graph G {" or "digraph G {"
            // remove trailing/opening braces
            if (stmt.find("graph") == 0 || stmt.find("digraph") == 0)
                continue;
            if (stmt == "{" || stmt == "}") continue;

            // check for edge operators
            size_t arrow = stmt.find("->");
            size_t dash = stmt.find("--");

            if (arrow != string::npos || dash != string::npos) {
                size_t op_pos = (arrow != string::npos) ? arrow : dash;
                string left = stmt.substr(0, op_pos);
                string right = stmt.substr(op_pos + ((arrow != string::npos) ? 2 : 2));

                // right may contain further edges like "A -> B -> C" -> handle chain
                vector<string> parts;
                // split on arrow or dash occurrences to get sequence
                string chain = stmt;
                // unify operator to '>' for splitting
                for (size_t i = 0; i + 1 < chain.size(); ++i) {
                    if (chain[i] == '-' && chain[i + 1] == '>') { chain[i] = '>'; chain[i + 1] = '>'; }
                }

                // We will parse simply by replacing "->" and "--" with a single delimiter and splitting
                string working = stmt;
                // replace "->" with '>'
                size_t p = 0;
                while ((p = working.find("->", p)) != string::npos) { working.replace(p, 2, ">"); p += 1; }
                p = 0;
                while ((p = working.find("--", p)) != string::npos) { working.replace(p, 2, ">"); p += 1; }

                // now split by '>' to get nodes in chain
                vector<string> nodes;
                size_t cur = 0;
                while (cur < working.size()) {
                    size_t nxt = working.find('>', cur);
                    string piece;
                    if (nxt == string::npos) { piece = working.substr(cur); cur = working.size(); } else { piece = working.substr(cur, nxt - cur); cur = nxt + 1; }
                    piece = trim(piece);
                    if (!piece.empty()) {
                        // piece may still contain attributes; clean
                        nodes.push_back(clean_token(piece));
                    }
                }

                // add nodes and edges between consecutive nodes
                for (size_t i = 0; i < nodes.size(); ++i) {
                    if (nodes[i].empty()) continue;
                    grafo.add_no(nodes[i]);
                    if (i + 1 < nodes.size() && !nodes[i + 1].empty()) {
                        grafo.add_no(nodes[i + 1]);
                        grafo.add_aresta(nodes[i], nodes[i + 1]);
                    }
                }

            } else {
                // statement is likely a node declaration or multiple nodes separated by commas
                // split by comma
                size_t cur = 0;
                while (cur < stmt.size()) {
                    size_t nxt = stmt.find(',', cur);
                    string piece;
                    if (nxt == string::npos) { piece = stmt.substr(cur); cur = stmt.size(); } else { piece = stmt.substr(cur, nxt - cur); cur = nxt + 1; }
                    piece = clean_token(piece);
                    if (!piece.empty()) grafo.add_no(piece);
                }
            }
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

    bool e_conexo() {
        return direcionado ? digrafo_e_conexo() : grafo_e_conexo();
    }
};


int main() {

    // Grafo<int> grafo = Grafo<int>::a_partir_de_aleatorio(true, 5, 40);
    Grafo<string> grafo = Grafo<string>::a_partir_de_arquivo_dot(".dot");
    grafo.print();
    // grafo.exportar_para_dot(".dot");
    grafo.exportar_para_png(".png");

    cout << (grafo.e_conexo() ? "CONEXO" : "NÃO CONEXO") << endl;

    return 0;
}