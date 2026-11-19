
#include <vector>
using Vertice=int ;
struct Edge {
Vertice u;
Vertice v;
int weight;
};
struct Graph{
std::vector<Vertice> vertices;
std::vector<Edge> edges;
};