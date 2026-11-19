/*
* @brief


*/
#include <omp.h>
#include "graph.hpp"
#include <algorithm>
#include <utility>
#include <vector>
class hoepmanAlgorithm {
public:
  hoepmanAlgorithm(Graph graph) : _graph(graph) {
    _parallelGraph=graph;
    candidates.resize(graph.vertices.size(), -1);
    matches.resize(graph.vertices.size(), -1);
    currentHighestAdjacency.resize(graph.vertices.size(), 0);
    _adjacencyList.resize(graph.vertices.size());



    _parallelCandidates.resize(graph.vertices.size(), -1);
    _parallelMatches.resize(graph.vertices.size(), -1);
    currentHighestParallelAdjacency.resize(graph.vertices.size(), 0);
    _parallelAdjacencyList.resize(graph.vertices.size());
    makeDeacendingAdjacecnyList(_adjacencyList);
    makeDeacendingAdjacecnyList(_parallelAdjacencyList);
  }
  Graph StartSequentialAlgorithm() {
    bool matchingFound = true;

    // we loop over the vertices to find matches, if we found we loop again,
    // otherwise we stop.
    while (matchingFound) {
      matchingFound = false; // so we check if a matching found in a loop
      for (Vertice u : _graph.vertices) {
        if (matches[u] == -1) {

          // vertice v = find highest weight edge in adj
          // send a request to v by u

          candidates[u] = getBestMatch(u);
        }
      }

      // we loop again so we check if 2 edges requested each others, which means
      // they match
      for (Vertice u : _graph.vertices) {
        if (matches[u] != 1 && candidates[u] != -1) {
          Vertice v = candidates[u];
          if (candidates[v] == u) {
            matches[u] = 1;
            matches[v] = 1;
            matchingFound = true;
          }
        }
      }
    }
    Graph outGraph = _graph;
    outGraph.edges.clear();

    for (Vertice u : _graph.vertices) {
      if (matches[u] == 1 && u < candidates[u]) {
        for (const auto &neighbor : _adjacencyList[u]) {
          if (neighbor.first == candidates[u]) {
            outGraph.edges.push_back({u, candidates[u], neighbor.second});
            break;
          }
        }
      }
    }

    return outGraph;
  }
 Graph StartParallelAlgorithm(int numberOfThreads) {
    bool matchingFound = true;

    // we loop over the vertices to find matches, if we found we loop again,
    // otherwise we stop.
    while (matchingFound) {
      matchingFound = false; // so we check if a matching found in a loop
      #pragma omp parallel for num_threads(numberOfThreads) schedule(dynamic, 256)
      for (Vertice u : _parallelGraph.vertices) {
        if (_parallelMatches[u] == -1) {

          // vertice v = find highest weight edge in adj
          // send a request to v by u

          _parallelCandidates[u] = getParallelBestMatch(u);
        }
      }

      // we loop again so we check if 2 edges requested each others, which means
      // they match
      #pragma omp parallel for num_threads(numberOfThreads) schedule(dynamic, 256) reduction(||:matchingFound)
      for (Vertice u : _parallelGraph.vertices) {
        if (_parallelMatches[u] != 1 && _parallelCandidates[u] != -1) {
          Vertice v = _parallelCandidates[u];
          if (_parallelCandidates[v] == u) {
            _parallelMatches[u] = 1;
          
            matchingFound = true;
          }
        }
      }
    }
    Graph outGraph = _graph;
    outGraph.edges.clear();

    for (Vertice u : _parallelGraph.vertices) {
      if (_parallelMatches[u] == 1 && u < _parallelCandidates[u]) {
        for (const auto &neighbor : _parallelAdjacencyList[u]) {
          if (neighbor.first == _parallelCandidates[u]) {
            outGraph.edges.push_back({u, _parallelCandidates[u], neighbor.second});
            break;
          }
        }
      }
    }

    return outGraph;
  }
private:
  Graph _graph;
  Graph _parallelGraph;
  // local variables of helper vectors

  // candidates saves the vertice with best match for vertice u
  std::vector<Vertice> candidates;

  // matches saves the matched vertices and their current relation, so we can
  // mkae the matched edges and also to make we dont change a matched one
  std::vector<Vertice> matches;
  std::vector<std::vector<std::pair<Vertice, int>>> _adjacencyList;
  std::vector<Vertice> currentHighestAdjacency;



  // same data of sequential
  std::vector<Vertice> _parallelCandidates;
  std::vector<Vertice> _parallelMatches;
  std::vector<std::vector<std::pair<Vertice, int>>> _parallelAdjacencyList;
  std::vector<Vertice> currentHighestParallelAdjacency;

  // creating adjacancy list for the vertices, sorted in a deacending way, and
  // marks if an adjacany vertex rejected the request
  void makeDeacendingAdjacecnyList(std::vector<std::vector<std::pair<Vertice, int>>>& adjacencyList ) {
    for (Edge e : _graph.edges) {
      adjacencyList[e.u].push_back({e.v, e.weight});
      adjacencyList[e.v].push_back({e.u, e.weight});
    }
    for (Vertice u : _graph.vertices) {
      std::sort(adjacencyList[u].begin(), adjacencyList[u].end(),
                [](const std::pair<Vertice, int> &a,
                   const std::pair<Vertice, int> &b) {
                    if (a.second!=b.second)  return a.second >b.second;     //sorting by weight highest to lowest 
                       return a.first < b.first;  // if equal we just return a ordering, as it doesnt matter
                    
                  
                           
                });
    }
  }

  

  Vertice getBestMatch(Vertice u) {
    if (_adjacencyList[u].size() < 0) {
      return -1;
    }
    int id = currentHighestAdjacency[u]; // we get postion of the last potentiel
                                         // match
    while (id < _adjacencyList[u].size() &&
           matches[_adjacencyList[u][id].first] != -1) {
      id++;
    }
    currentHighestAdjacency[u] = id;

    if (currentHighestAdjacency[u] < _adjacencyList[u].size()) {
      return _adjacencyList[u][currentHighestAdjacency[u]].first;
    } else {
      return -1;
    }
  } 
  
  
  
  
  Vertice getParallelBestMatch(Vertice u) {
    if (_parallelAdjacencyList[u].size() < 0) {
      return -1;
    }
    int id = currentHighestParallelAdjacency[u]; // we get postion of the last potentiel
                                         // match
    while (id < _parallelAdjacencyList[u].size() &&
           _parallelMatches[_parallelAdjacencyList[u][id].first] != -1) {
      id++;
    }
    currentHighestParallelAdjacency[u] = id;

    if (currentHighestParallelAdjacency[u] < _parallelAdjacencyList[u].size()) {
      return _parallelAdjacencyList[u][currentHighestParallelAdjacency[u]].first;
    } else {
      return -1;
    }
  }
};