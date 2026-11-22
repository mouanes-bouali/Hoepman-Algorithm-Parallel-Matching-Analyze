#include "graph.hpp"
#include "graphGenerator.cpp"
#include "hoepmanAlgortihm.cpp"
#include <cstdio>
#include <iostream>
class graphAnalyzer {
public:
    graphAnalyzer(){
        generator= graphGenerator();
    };
    void parallelAlgortihmRuner(){
        Graph g;
        int V=10000;
        int E=V*10;
       generator.createBaseGraph(g,V,E);

       auto h= hoepmanAlgorithm(g);
     auto results=   h.StartAnalysingParallelAlgorithm( 7,100);
    printf("%.4f", results.executionTimeMs);   
    printf("\n");   
        printf("%lld", results.usedMemoryBytes);   
        printf("\n"); 


    
    }
    void sequenctialAlgorithmRunner(){
         Graph g;
        int V=100000;
        int E=V*2;
       generator.createBaseGraph(g,V,E);

       auto h= hoepmanAlgorithm(g);
     auto results=   h.StartAnalysingSequentialAlgorithm();
    printf("%.4f n", results.executionTimeMs);   
    printf("%lld", results.usedMemoryBytes);   
    }

private:
    graphGenerator generator;
};
int main() {
    graphAnalyzer a;
    a.parallelAlgortihmRuner();
    a.sequenctialAlgorithmRunner();
    return 0;
}   