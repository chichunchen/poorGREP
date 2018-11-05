#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <stack>
#include <utility>
#include <memory>

using namespace std;

class Digraph {
public:
    explicit Digraph(size_t V) : V(V), adj(vector<vector<int>>(V, vector<int>())) {
    }

    void addEdge(int v, int w) {
        adj[v].push_back(w);
    }

    vector<vector<int>> &getAdj() {
        return adj;
    }

    size_t getV() {
        return this->V;
    }

private:
    const size_t V;
    vector<vector<int>> adj;
};

class DirectedDFS {
public:
    DirectedDFS(Digraph &G, int s) : marked(vector<bool>(static_cast<unsigned long>(G.getV()))) {
        dfs(G, s);
    }

    DirectedDFS(Digraph &G, vector<int> &vec) : marked(vector<bool>(static_cast<unsigned long>(G.getV()))) {
        for (int e : vec) dfs(G, e);
    }

    bool visited(int v) {
        return marked[v];
    }

private:
    vector<bool> marked;

    void dfs(Digraph &G, int v) {
        marked[v] = true;
        for (int e : G.getAdj()[v]) {
            if (!marked[e]) dfs(G, e);
        }
    }
};

class NFA {
public:
    explicit NFA(string regex) : M(regex.length()) {
        re = move(regex);
        buildEpsilonTransitionDigraph();
    }

    bool recognizes(string txt) {
        vector<int> pc;
        DirectedDFS dfs(*G, 0);

        // state reachable from start by epsilon-transitions
        for (int v = 0; v < G->getV(); v++) {
            if (dfs.visited(v)) pc.push_back(v);
        }

        // state reachable after scanning past txt[i]
        for (char ch : txt) {
            vector<int> match;
            for (int v : pc) {
                if (v == M) continue;
                if ((re[v] == ch) || re[v] == '.') match.push_back(v + 1);
            }

            // follow epsilon-transitions
            dfs = DirectedDFS(*G, match);
            pc.clear();
            for (int v = 0; v < G->getV(); v++) {
                if (dfs.visited(v)) pc.push_back(v);
            }
        }

        for (int v : pc)
            if (v == M) return true;
        return false;
    }

private:
    string re;                  // match transitions
    unique_ptr<Digraph> G;      // epsilon transition of digraph
    size_t M;                   // number of states

    void buildEpsilonTransitionDigraph() {
        G = make_unique<Digraph>(Digraph(M + 1));
        stack<int> ops;
        for (int i = 0; i < M; i++) {
            int lp = i;

            if (re[i] == '(' || re[i] == '|') {
                ops.push(i);
            } else if (re[i] == ')') {
                int _or = ops.top();
                ops.pop();
                if (re[_or] == '|') {
                    lp = ops.top();
                    ops.pop();
                    G->addEdge(lp, _or + 1);
                    G->addEdge(_or, i);
                } else lp = _or;
            }

            // closure
            if ((i < (M - 1)) && (re[i + 1] == '*')) {
                G->addEdge(lp, i + 1);
                G->addEdge(i + 1, lp);
            }

            // meta-symbols
            if (re[i] == '(' || re[i] == '*' || re[i] == ')')
                G->addEdge(i, i + 1);
        }
    }
};

void unit_test() {
    NFA nfa("((A*B|AC)D)");
    assert(nfa.recognizes("AAAABD"));
    assert(!nfa.recognizes("BCD"));
}

int main(int argc, const char *argv[]) {
    // unit_test();
    string re = "(.*" + string(argv[1]) + ".*)";
    NFA nfa(re);
    for (string line; getline(cin, line);) {
        if (nfa.recognizes(line)) {
            cout << line << endl;
        }
    }
    return 0;
}