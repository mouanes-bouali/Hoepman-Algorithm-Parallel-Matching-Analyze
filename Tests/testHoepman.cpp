#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../hoepmanAlgortihm.cpp"   

#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include <utility>
#include <vector>

// ------------------------------------------------------------------
// Helpers
// ------------------------------------------------------------------

static long long totalWeight(const Graph& g) {
    long long t = 0;
    for (const auto& e : g.edges) t += e.weight;
    return t;
}

static bool isValidMatching(const Graph& g) {
    std::vector<char> used(g.vertices.size(), 0);
    for (const auto& e : g.edges) {
        if (e.u < 0 || e.v < 0)                              return false;
        if (e.u >= (int)used.size() || e.v >= (int)used.size()) return false;
        if (used[e.u] || used[e.v])                          return false;
        used[e.u] = 1;
        used[e.v] = 1;
    }
    return true;
}

// Returns the matching's edges as an undirected set for comparison.
static std::set<std::pair<Vertice, Vertice>> edgeSet(const Graph& g) {
    std::set<std::pair<Vertice, Vertice>> s;
    for (const auto& e : g.edges)
        s.insert({std::min(e.u, e.v), std::max(e.u, e.v)});
    return s;
}

static int weightBetween(const Graph& g, Vertice u, Vertice v) {
    for (const auto& e : g.edges)
        if ((e.u == u && e.v == v) || (e.u == v && e.v == u))
            return e.weight;
    return -1;
}

// Build a random simple graph with n vertices and about m edges.
static Graph randomGraph(int n, int m, int maxW, std::mt19937& rng) {
    Graph g;
    g.vertices.resize(n);
    std::iota(g.vertices.begin(), g.vertices.end(), 0);

    std::set<std::pair<int, int>> seen;
    std::uniform_int_distribution<int> pickV(0, n - 1);
    std::uniform_int_distribution<int> pickW(1, maxW);

    int guard = 0;
    while ((int)seen.size() < m && guard < m * 100) {
        ++guard;
        int u = pickV(rng), v = pickV(rng);
        if (u == v) continue;
        auto p = std::minmax(u, v);
        if (seen.count(p)) continue;
        seen.insert(p);
        g.edges.push_back({u, v, pickW(rng)});
    }
    return g;
}

// ------------------------------------------------------------------
// Test 1: Small deterministic cases.
// For every graph we assert:
//   (a) both algorithms return a valid matching,
//   (b) they return exactly the same edge set,
//   (c) the total weight matches the expected value.
// ------------------------------------------------------------------
TEST_CASE("Hoepman: small deterministic graphs", "[hoepman]") {

    auto run = [](const Graph& g) {
        hoepmanAlgorithm seqAlgo(g);
        hoepmanAlgorithm parAlgo(g);

        Graph seq = seqAlgo.StartSequentialAlgorithm();
        Graph par = parAlgo.StartParallelAlgorithm(4);

        REQUIRE(isValidMatching(seq));
        REQUIRE(isValidMatching(par));
        REQUIRE(edgeSet(seq) == edgeSet(par));
        REQUIRE(totalWeight(seq) == totalWeight(par));

        return std::make_pair(seq, par);
    };

    SECTION("empty graph (isolated vertices)") {
        Graph g = {{0, 1, 2}, {}};
        auto r = run(g);
        REQUIRE(r.first.edges.empty());
        REQUIRE(r.second.edges.empty());
    }

    SECTION("single edge") {
        Graph g = {{0, 1}, {{0, 1, 10}}};
        auto r = run(g);
        REQUIRE(r.first.edges.size() == 1);
        REQUIRE(totalWeight(r.first) == 10);
    }

    SECTION("triangle: heaviest edge wins") {
        // 0-1 (10), 1-2 (5), 0-2 (1)  ->  match (0,1)
        Graph g = {{0, 1, 2}, {{0, 1, 10}, {1, 2, 5}, {0, 2, 1}}};
        auto r = run(g);
        REQUIRE(r.first.edges.size() == 1);
        REQUIRE(weightBetween(g, r.first.edges[0].u, r.first.edges[0].v) == 10);
    }

    SECTION("two independent edges") {
        Graph g = {{0, 1, 2, 3}, {{0, 1, 10}, {2, 3, 8}}};
        auto r = run(g);
        REQUIRE(r.first.edges.size() == 2);
        REQUIRE(totalWeight(r.first) == 18);
    }

    SECTION("path: heavier edge wins") {
        // 0-1 (10), 1-2 (20)  ->  match (1,2)
        Graph g = {{0, 1, 2}, {{0, 1, 10}, {1, 2, 20}}};
        auto r = run(g);
        REQUIRE(r.first.edges.size() == 1);
        REQUIRE(totalWeight(r.first) == 20);
    }

    SECTION("greedy trap") {
        // 0-1 (10), 0-2 (9), 1-3 (9), 2-3 (1)  ->  (0,1) + (2,3) = 11
        Graph g = {{0, 1, 2, 3},
                   {{0, 1, 10}, {0, 2, 9}, {1, 3, 9}, {2, 3, 1}}};
        auto r = run(g);
        REQUIRE(r.first.edges.size() == 2);
        REQUIRE(totalWeight(r.first) == 11);
    }

    SECTION("isolated vertex + one edge") {
        // 0 isolated, 1-2 (7)
        Graph g = {{0, 1, 2}, {{1, 2, 7}}};
        auto r = run(g);
        REQUIRE(r.first.edges.size() == 1);
        REQUIRE(totalWeight(r.first) == 7);
    }
}

// ------------------------------------------------------------------
// Test 2: Parallel is deterministic and matches the sequential result
//         regardless of the number of threads used.
// ------------------------------------------------------------------
TEST_CASE("Hoepman: parallel matches sequential across thread counts",
          "[hoepman][parallel]") {

    std::mt19937 rng(12345);

    for (int trial = 0; trial < 25; ++trial) {
        Graph g = randomGraph(/*n=*/40, /*m=*/80, /*maxW=*/100, rng);

        // Reference: run sequential once on a fresh object.
        hoepmanAlgorithm seqAlgo(g);
        Graph seq = seqAlgo.StartSequentialAlgorithm();
        REQUIRE(isValidMatching(seq));
        auto refEdges = edgeSet(seq);

        for (int t : {1, 2, 4, 8}) {
            hoepmanAlgorithm parAlgo(g);
            Graph par = parAlgo.StartParallelAlgorithm(t);

            INFO("trial=" << trial << " threads=" << t);
            REQUIRE(isValidMatching(par));
            REQUIRE(edgeSet(par) == refEdges);
            REQUIRE(totalWeight(par) == totalWeight(seq));
        }
    }
}

// ------------------------------------------------------------------
// Test 3: Stress: validity on larger random graphs.
//         Only checks that both outputs are *valid* matchings and
//         equal each other. Does not assert a specific total weight.
// ------------------------------------------------------------------
TEST_CASE("Hoepman: validity on larger random graphs",
          "[hoepman][stress]") {

    std::mt19937 rng(999);

    for (int trial = 0; trial < 10; ++trial) {
        int n = 500;
        int m = 2000;
        Graph g = randomGraph(n, m, 1000, rng);

        hoepmanAlgorithm seqAlgo(g);
        hoepmanAlgorithm parAlgo(g);

        Graph seq = seqAlgo.StartSequentialAlgorithm();
        Graph par = parAlgo.StartParallelAlgorithm(4);

        INFO("trial=" << trial);
        REQUIRE(isValidMatching(seq));
        REQUIRE(isValidMatching(par));
        REQUIRE(edgeSet(seq) == edgeSet(par));
    }
}