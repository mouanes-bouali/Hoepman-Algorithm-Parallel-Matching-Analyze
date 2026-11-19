#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../greedyMatching.cpp" // Make sure this matches your actual file name!

// Helper function to calculate the total weight of the matching
long long calculateTotalWeight(const Graph& matching) {
    long long total = 0;
    for (const auto& e : matching.edges) {
        total += e.weight;
    }
    return total;
}

// Helper function to check that no vertex is matched more than once
bool isValidMatching(const Graph& matching) {
    // Assuming Vertice is an int (0 to N-1)
    std::vector<bool> matched(matching.vertices.size(), false);
    
    for (const auto& e : matching.edges) {
        // If the vertex is already marked as true, we matched it twice!
        if (matched[e.u] || matched[e.v]) {
            return false; 
        }
        matched[e.u] = true;
        matched[e.v] = true;
    }
    return true;
}

TEST_CASE("Greedy Matching - Core Logic", "[greedy]") {
    
    SECTION("Empty Graph") {
        // 3 vertices, but no edges
        Graph g = {{0, 1, 2}, {}}; 
        GreedyMatching greedy(g);
        Graph result = greedy.findMaximalWeightedMatching();
        
        REQUIRE(result.edges.empty());
        REQUIRE(calculateTotalWeight(result) == 0);
    }

    SECTION("Single Edge") {
        // Vertex 0 and 1, connected by weight 10
        Graph g = {{0, 1}, {{0, 1, 10}}};
        GreedyMatching greedy(g);
        Graph result = greedy.findMaximalWeightedMatching();
        
        REQUIRE(result.edges.size() == 1);
        REQUIRE(calculateTotalWeight(result) == 10);
        REQUIRE(isValidMatching(result));
    }

    SECTION("Simple Triangle (Pick the max)") {
        // Vertices: 0, 1, 2
        // Edges: (0-1, weight 10), (1-2, weight 5), (0-2, weight 1)
        // Expected: Greedy picks (0-1, weight 10) and stops because 0 and 1 are matched.
        Graph g = {{0, 1, 2}, {
            {0, 1, 10}, 
            {1, 2, 5}, 
            {0, 2, 1}
        }};
        
        GreedyMatching greedy(g);
        Graph result = greedy.findMaximalWeightedMatching();
        
        REQUIRE(result.edges.size() == 1);
        REQUIRE(result.edges[0].weight == 10);
        REQUIRE(calculateTotalWeight(result) == 10);
        REQUIRE(isValidMatching(result));
    }

    SECTION("Two Independent Edges") {
        // Vertices: 0, 1, 2, 3
        // Edges: (0-1, 10) and (2-3, 8)
        // Expected: Greedy should pick both because they don't share vertices.
        Graph g = {{0, 1, 2, 3}, {
            {0, 1, 10}, 
            {2, 3, 8}
        }};
        
        GreedyMatching greedy(g);
        Graph result = greedy.findMaximalWeightedMatching();
        
        REQUIRE(result.edges.size() == 2);
        REQUIRE(calculateTotalWeight(result) == 18);
        REQUIRE(isValidMatching(result));
    }

    SECTION("The Greedy Trap (Sub-optimal locally, sub-optimal globally)") {
        // Vertices: 0, 1, 2, 3
        // Edges: (0-1, 10), (0-2, 9), (1-3, 9), (2-3, 1)
        // The absolute best matching is (0-2, 9) + (1-3, 9) = 18.
        // But Greedy grabs the 10 first: (0-1). Then it is forced to take (2-3, 1). Total = 11.
        
        Graph g = {{0, 1, 2, 3}, {
            {0, 1, 10}, 
            {0, 2, 9}, 
            {1, 3, 9}, 
            {2, 3, 1}
        }};
        
        GreedyMatching greedy(g);
        Graph result = greedy.findMaximalWeightedMatching();
        
        // We expect exactly 2 edges
        REQUIRE(result.edges.size() == 2);
        // We expect Greedy to fail the optimal test and get 11 instead of 18
        REQUIRE(calculateTotalWeight(result) == 11);
        REQUIRE(isValidMatching(result));
    }
}