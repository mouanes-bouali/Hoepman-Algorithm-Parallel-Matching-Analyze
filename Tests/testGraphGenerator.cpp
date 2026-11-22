// testGraphGenerator.cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../graphGenerator.cpp"

#include <set>
#include <vector>

// quick helpers I kept using
static int deg(const Graph& g, int v) {
    int d = 0;
    for (auto& e : g.edges) {
        if (e.u == v || e.v == v) d++;
    }
    return d;
}

static int countDeg(const Graph& g, int k) {
    int c = 0;
    for (size_t i = 0; i < g.vertices.size(); i++)
        if (deg(g, i) == k) c++;
    return c;
}

TEST_CASE("empty graph has right number of vertices", "[gen]") {
    graphGenerator gen;
    Graph g;
    gen.createEmptyGraph(g, 10);
    REQUIRE(g.vertices.size() == 10);
    REQUIRE(g.edges.size() == 0);
}

TEST_CASE("path graph", "[gen]") {
    graphGenerator gen;
    Graph g;
    gen.createPathGraph(g, 10);
    REQUIRE(g.edges.size() == 9);
    REQUIRE(countDeg(g, 1) == 2);
    REQUIRE(countDeg(g, 2) == 8);
    // no self loops
    for (auto& e : g.edges)
        REQUIRE(e.u != e.v);
}

TEST_CASE("circle graph", "[gen]") {
    graphGenerator gen;
    Graph g;
    gen.createCircleGraph(g, 10);
    REQUIRE(g.edges.size() == 10);
    REQUIRE(countDeg(g, 2) == 10);
    for (auto& e : g.edges)
        REQUIRE(e.u != e.v);
}

TEST_CASE("star graph", "[gen]") {
    graphGenerator gen;
    Graph g;
    gen.createStarGraph(g, 10);
    REQUIRE(g.edges.size() == 9);
    // center should have degree 9
    REQUIRE(deg(g, 0) == 9);
    REQUIRE(countDeg(g, 1) == 9);
}

TEST_CASE("complete graph", "[gen]") {
    graphGenerator gen;
    Graph g;
    gen.createCompleteGraph(g, 6);
    REQUIRE(g.edges.size() == 15);
    REQUIRE(countDeg(g, 5) == 6);
    // check no duplicate edges
    std::set<std::pair<int,int>> seen;
    for (auto& e : g.edges) {
        auto p = std::make_pair(std::min(e.u, e.v), std::max(e.u, e.v));
        REQUIRE(seen.insert(p).second);
    }
}

TEST_CASE("base graph random", "[gen]") {
    graphGenerator gen;
    Graph g;
    gen.createBaseGraph(g, 50, 100);
    REQUIRE(g.vertices.size() == 50);
    REQUIRE(g.edges.size() == 100);
    // spot check: no self loops, no dups, weights >= 0
    std::set<std::pair<int,int>> seen;
    for (auto& e : g.edges) {
        REQUIRE(e.u != e.v);
        REQUIRE(e.weight >= 0);
        auto p = std::make_pair(std::min(e.u, e.v), std::max(e.u, e.v));
        REQUIRE(seen.insert(p).second);
    }
    
}   
TEST_CASE("tree graph", "[gen]") {
    graphGenerator gen;
    Graph g;
    gen.createTreeGraph(g, 10);
    // n-1 edges, connected, no cycles
    REQUIRE(g.vertices.size() == 10);
    REQUIRE(g.edges.size() == 9);
    for (auto& e : g.edges)
        REQUIRE(e.u != e.v);
    // root (vertex 0) should have highest degree
    REQUIRE(deg(g, 0) >= 2);
}   

TEST_CASE("barabasi albert graph", "[gen]") {
    graphGenerator gen;
    Graph g;
    int n = 50, m = 3;
    gen.createBarabasiAlbertGraph(g, n, m);

    // seed clique: m(m-1)/2 edges, then (n-m)*m more
    int expected = m*(m-1)/2 + (n-m)*m;
    REQUIRE(g.vertices.size() == n);
    REQUIRE(g.edges.size() == expected);

    for (auto& e : g.edges)
        REQUIRE(e.u != e.v);

    // no dups
    std::set<std::pair<int,int>> seen;
    for (auto& e : g.edges) {
        auto p = std::make_pair(std::min(e.u, e.v), std::max(e.u, e.v));
        REQUIRE(seen.insert(p).second);
    }

    // should be connected (each new node links to existing ones)
    // quick BFS from 0
    std::vector<std::vector<int>> adj(n);
    for (auto& e : g.edges) { adj[e.u].push_back(e.v); adj[e.v].push_back(e.u); }
    std::vector<bool> vis(n, false);
    std::queue<int> q;
    q.push(0); vis[0] = true;
    int reached = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop(); reached++;
        for (int v : adj[u])
            if (!vis[v]) { vis[v] = true; q.push(v); }
    }
    REQUIRE(reached == n);
}   
