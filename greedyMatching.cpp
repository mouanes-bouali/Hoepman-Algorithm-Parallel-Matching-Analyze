#include "graph.hpp"
#include <algorithm>
#include <cstddef>
#include <queue>
#include <vector>
class GreedyMatching {
public:
// we assign the graph we will implment the algorithm  in the constructor
 GreedyMatching(Graph graph) : _graph(graph) {
    // assign the size of the used vectors with vertices total.
    _matched.resize(graph.vertices.size(),false);

  }

 // the main function of the algorithm that starts it, taking as a parameter a graph and returns its matching graph 
  Graph findMaximalWeightedMatching() {
// we make a new copy of the graph
    auto inGraph=_graph;
    Graph outGraph = inGraph;
    // clear the edges as we will insert the matching ones only
    outGraph.edges.clear();
    // assign the sorted edges into a queue
    auto sortededges = sortEdgesDescending(inGraph.edges);
        while (!sortededges.empty()) {
            auto e= sortededges.front();
            sortededges.pop();
            if (_matched[e.u]==false && _matched[e.v]== false) {
                outGraph.edges.push_back(e);
                _matched[e.u]=true;
                _matched[e.v]=true;
            }
        }

    return outGraph;
  };

private:
  Graph _graph;
  
  std::vector<bool>_matched;



  /**
  * @brief the fonction sorts the edges of from highest to lowest  O(n)

  */
  std::queue<Edge> sortEdgesDescending(std::vector<Edge> inEdges) {
    bool finishedsorting = false; // a bool to flag when the sorting finishes
    std::queue<Edge> sortedEdges ;
   
    std::sort(inEdges.begin(),inEdges.end(),[](const Edge& a,const Edge& b){
            return a.weight>b.weight;
    });

    for (const auto& e : inEdges)
        sortedEdges.push(e);
    return sortedEdges;
  }


  void removeVertice(std::vector<Vertice>& vertices, Vertice u){
    vertices.erase(vertices.begin()+ u);
  }
  void removeEdge(std::vector<Edge>& edges,Edge edge){
     int n=0;
     for (Edge ee : edges) {
        if (edge.u == ee.u && edge.v==ee.v){
                break;
        };
        n++;
     };       
    edges.erase(edges.begin()+n);
  }
};