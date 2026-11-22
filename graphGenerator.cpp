#include "graph.hpp"
#include <cstddef>
#include <queue>
#include <random>
#include <set>
#include <vector>
class graphGenerator {
public:
    graphGenerator()=default;
  void createEmptyGraph(Graph& graph,int numberOfVertices){
    for (size_t u=0;  u<numberOfVertices; u++) {
        graph.vertices.push_back(u);
    }
  }

  void createPathGraph(Graph& graph,int numberOfVertices ){
        
    intVertices(graph, numberOfVertices);
    
    for (size_t i=1;  i<numberOfVertices; i=i+2) {
        Edge e,e2;
        e.u=i-1;
        e.v=i;
        e.weight=i;
         if(i+1< numberOfVertices){
        e2.u=i;
        e2.v=i+1;
        e2.weight=i+1;
        graph.edges.push_back(e2);
    }

        graph.edges.push_back(e);
        
    }

  }

  void createStarGraph(Graph& graph, int numberOfVertices){
      genVertice.seed(1);
      genWeight.seed(1);
      intVertices(graph,  numberOfVertices);
      for (int v =1;v< numberOfVertices; v++) {
         Edge e= createEdge(0,  v, randomVerticeWeight(50));
         graph.edges.push_back(e);
      }
  }


   void createCircleGraph(Graph& graph,int numberOfVertices ){
        
    intVertices(graph, numberOfVertices);
    
    for (int i=1;  i<numberOfVertices; i=i+2) {
        Edge e,e2;

        e.u=i-1;
        e.v=i;
        e.weight=i;
        if(i+1< numberOfVertices){
        e2.u=i;
        e2.v=i+1;
        e.weight=i+1;
        graph.edges.push_back(e2);
    }

        graph.edges.push_back(e);
       
    }
     if (numberOfVertices >= 3) {
        Edge c;
        c.u = numberOfVertices - 1;
        c.v = 0;
        c.weight = numberOfVertices;           
        graph.edges.push_back(c);
    }
  }

  void createCompleteGraph(Graph& graph, int numberOfVertices){
    intVertices(graph, numberOfVertices);
       genVertice.seed(1);
      genWeight.seed(1);
      for (int u= 0; u<numberOfVertices; u++) {
          for (int v=u+1; v<numberOfVertices;v++ ) {
            Edge e=createEdge(u,  v, randomVerticeWeight(50));
            graph.edges.push_back(e);
          
          }
      
      }
  }
  void createTreeGraph(Graph& graph,int numberOfVertices){
    intVertices(graph,  numberOfVertices);
    genWeight.seed(1);
    int numberofchildren=2;
    int numberOfTreeEdges=0;
    std::queue<Vertice> queue; // to save the children that will become parents in the next loop
    queue.push(0);
    int verticeId=1; // saves the current not linked vertice
    while (numberOfTreeEdges<numberOfVertices-1) {
      Vertice parent =queue.front();
      queue.pop();
        int childrenForThisParent = numberofchildren - verticeId + 1;
      for (int u=verticeId ; u<=numberofchildren;u++ ) {
          if (u >= numberOfVertices) break;
          Edge e= createEdge(u, parent , randomVerticeWeight(400));
          graph.edges.push_back(e);   
          queue.push(u);

          numberOfTreeEdges++;
          verticeId++;
      
      }
      numberofchildren=numberofchildren*2;

    }
  }
 void createBarabasiAlbertGraph(Graph& graph, int numberOfVertices, int m) {
    intVertices(graph, numberOfVertices);
    genWeight.seed(1);

    std::vector<int> degrees(numberOfVertices, 0);

    // seed: clique of size m
    for (int i = 0; i < m; i++)
        for (int j = i + 1; j < m; j++) {
            graph.edges.push_back(createEdge(i, j, randomVerticeWeight(400)));
            degrees[i]++;
            degrees[j]++;
        }

    for (int newV = m; newV < numberOfVertices; newV++) {
        int chosen[m];
        int count = 0;

        while (count < m) {
            int totalDeg = 0;
            for (int v = 0; v < newV; v++) totalDeg += degrees[v];

            std::uniform_int_distribution<int> dist(0, totalDeg - 1);
            int r = dist(genWeight);

            int running = 0, target = 0;
            for (int v = 0; v < newV; v++) {
                running += degrees[v];
                if (running > r) { target = v; break; }
            }

            bool dup = false;
            for (int i = 0; i < count; i++)
                if (chosen[i] == target) { dup = true; break; }

            if (!dup) {
                chosen[count++] = target;
                graph.edges.push_back(createEdge(newV, target, randomVerticeWeight(400)));
                degrees[target]++;
                degrees[newV]++;
            }
        }
    }
}   

  void createBaseGraph(Graph& graph, int numberOfVertices,int numberOfEdges){
    intVertices(graph, numberOfVertices);
    int currentnumberOfEdges=0;
    genVertice.seed(1);
    genVertice.seed(1);
   
    while (currentnumberOfEdges<numberOfEdges) {
        for (Vertice u : graph.vertices) {
            Vertice v;
            do {
             v=randomVertice(numberOfVertices);
            }while (u==v);
            Edge e=createEdge(u, v, randomVerticeWeight(maxWeight));
            if (checkEdgeExist(graph, e.u, e.v)) {
              continue;
            }
            graph.edges.push_back(e);
            currentnumberOfEdges++;
            if (currentnumberOfEdges==numberOfEdges) {
                    break;
            }
        }

    }



  }
  void createCompleteBipartiteGraph(Graph& graph, int partASize, int PartBSize) {
    intVertices(graph, partASize + PartBSize);

    for (int u = 0; u < partASize; ++u) {
        for (int v = partASize; v < partASize + PartBSize; ++v) {
            graph.edges.push_back(createEdge(u, v, randomVerticeWeight(maxWeight)));
        }
    }
}


  
private:

     std::mt19937 genVertice{42};
     std::mt19937 genWeight{42};
    int maxWeight=400;
Edge createEdge(Vertice u, Vertice v,int weight){
    Edge e;
    e.v=v;
    e.u=u;
    e.weight=weight;
    return e;
  }
    void intVertices(Graph& graph,int numberOfVertices){
        
         for (size_t u=0;  u<numberOfVertices; u++) {
        graph.vertices.push_back(u);

        
    }
  }
  bool checkEdgeExist(Graph& graph,Vertice u , Vertice v){
    for (Edge e : graph.edges) {
         if ((e.u == u && e.v == v) || (e.u == v && e.v == u)) {
                return true;
          }
    }
    return false;
  }

     int randomVertice(int max) {
    return static_cast<int>(static_cast<double>(genVertice()) / 4294967296.0 * max);
}   

 int randomVerticeWeight(int max) {
    return static_cast<int>(static_cast<double>(genWeight
        ()) / 4294967296.0 * max);
}   
void saveGraph(const char*& filePath,Graph graph ){

}
std::vector<Graph> loadALLGraphs(const char*& folderPath,Graph graph){


}



};