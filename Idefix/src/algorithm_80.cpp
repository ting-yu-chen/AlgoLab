#include <iostream>
#include <string>
#include <cmath>
#include <algorithm> 
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <limits>  

using namespace std; 

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Triangulation;
typedef Triangulation::Finite_faces_iterator Face_iterator;
typedef Triangulation::Finite_edges_iterator Edge_iterator;


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> graph; // Use vecS for the VertexList! Choosing setS for the OutEdgeList disallows parallel edges
typedef boost::graph_traits<graph>::vertex_descriptor    vertex_desc;    // Vertex Descriptor: with vecS vertex list, this is really just an int in the range [0, num_vertices(G)).  
typedef boost::graph_traits<graph>::edge_iterator    edge_it;    // to iterate over all edges

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
  boost::no_property, boost::property<boost::edge_weight_t, int> >      weighted_graph;
typedef boost::property_map<weighted_graph, boost::edge_weight_t>::type weight_map;
typedef boost::graph_traits<weighted_graph>::edge_descriptor            edge_desc;
typedef boost::graph_traits<graph>::vertex_descriptor    vertex_desc_w;    // Vertex Descriptor: with vecS vertex list, this is really just an int in the range [0, num_vertices(G)).  
typedef boost::graph_traits<graph>::edge_iterator    edge_it_w;    // to iterate over all edges


int computeK(const K::FT len, const int n, const vector<tuple<K::FT, int, int>> &edges, const vector<vector<K::FT>> &treeNbones){
    graph G(n);
    for(int i = 0 ; i < edges.size(); i++){
        K::FT l = get<0>(edges[i]); 
        if (l <= len){
          int v1_idx = get<1>(edges[i]);
          int v2_idx = get<2>(edges[i]);
          boost::add_edge(v1_idx, v2_idx, G);
        }
        else break; 
    }
    std::vector<int> component_map(n);
    int ncc = boost::connected_components(G, boost::make_iterator_property_map(component_map.begin(), boost::get(boost::vertex_index, G))); 
    vector<int> bonesCount(ncc, 0);
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < treeNbones[i].size(); j++){
            if (treeNbones[i][j] <= len){
                bonesCount[component_map[i]]++; 
                //bestlen = max(bestlen, treeNbones[i][j]); 
            }
        }
    }
    int maxK = 0 ; 
    int bestcomp = 0; 
    for(int i = 0 ; i < ncc; i++){
        if (bonesCount[i] > maxK){
            bestcomp = i; 
            maxK = bonesCount[i];
        }
    }
    return maxK; 
}
void testcase(){
  int n, m, k; K::FT s ; 
  cin >> n >> m >> s >> k ; 
  map< K::Point_2, int > treeInfo; 
  vector<K::Point_2> tree;
  tree.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    int x, y;
    cin >> x >> y;
    K::Point_2 p(x, y);
    tree.push_back(p);
    treeInfo.insert(pair<K::Point_2, int>(p, i)); 
  }
 // construct triangulation
  Triangulation t;
  t.insert(tree.begin(), tree.end());
  vector<K::Point_2> bones;
  bones.reserve(n);
  for (std::size_t i = 0; i < m; ++i) {
    int x, y; cin >> x >> y;
    K::Point_2 bone(x, y);
    bones.push_back(bone);
  }
  // build a graph for connected components 
    graph G(n);
    weighted_graph wG(n); 
    weight_map weights = boost::get(boost::edge_weight, wG);
    edge_desc ed;
  
  vector<tuple<K::FT, int, int>> edges; 
  vector<K::FT> edgelens; 
  for (Edge_iterator ei = t.finite_edges_begin(); ei != t.finite_edges_end(); ++ei){
    K::Segment_2 e = t.segment(ei); 
    K::FT edgelen = e.squared_length() ; 
    edgelens.push_back(edgelen);
    int v1_idx = treeInfo[e.source()];
    int v2_idx = treeInfo[e.target()];
    edges.push_back({edgelen, v1_idx, v2_idx});
    ed = boost::add_edge(v1_idx, v2_idx, wG).first; weights[ed]=CGAL::to_double(edgelen);
    if (edgelen <= s ){
      boost::add_edge(v1_idx, v2_idx, G);
    } 
  }
 
  std::vector<int> component_map(n);
  int ncc = boost::connected_components(G, boost::make_iterator_property_map(component_map.begin(), boost::get(boost::vertex_index, G))); 
  // compute the bones in each connected componets 
  vector<int> bonesCount(ncc, 0 );
  vector<vector<K::FT>> treeNbones(n, vector<K::FT>());
  for(int i = 0 ; i < m ; i++){
    K::Point_2 bone = bones[i]; 
    Triangulation::Vertex_handle vh = t.nearest_vertex(bone); 
    K::Point_2 nearest_tree = vh->point();
    K::FT dist =CGAL::squared_distance(nearest_tree, bone);
    dist = dist*4; 
    edgelens.push_back(dist);
    treeNbones[treeInfo[nearest_tree]].push_back(dist);
    if (dist <= s){
      int componentIdx = component_map[treeInfo[nearest_tree]]; 
      bonesCount[componentIdx] ++ ; 
    }
  }

  int maxbones = 0 ; 
  for(int i = 0 ; i < ncc; i++){
    maxbones = max(maxbones, bonesCount[i]);
  }
  set<K::FT> edgeset(edgelens.begin(), edgelens.end() );
  edgelens.assign(edgeset.begin(), edgeset.end() );
  sort(edgelens.begin(), edgelens.end()); 
  sort(edges.begin(), edges.end()); 

  int l = 0 ; int r = edgelens.size()-1 ; 
  int mid = (l+r)/2;
  K::FT bestR = edgelens[r]; 
  int bestIdx = r ; 
  while(l < r){
    K::FT curR = edgelens[mid];
    int curK = computeK(curR, n, edges, treeNbones);
    //cout << curR << " " << curK << "---\n";
    if (curK < k ) l = mid+1; 
    else{
        if (curR <= bestR){
          bestR = min(curR, bestR);
          bestIdx = min(mid, bestIdx);
        }
        r = mid-1; 
    }
    mid = (l+r)/2; 
  }
  while(true && bestIdx > 0){
    bestIdx -- ; 
    int curK = computeK(edgelens[bestIdx], n, edges, treeNbones);
    if (curK >= k) bestR = edgelens[bestIdx]; 
    else break; 
  }
   cout << fixed << std::setprecision(0)<< maxbones << " " << (long)CGAL::to_double(bestR) << "\n";
}
int main(){
    int t;
    cin >> t ;
    for (int i = 0 ; i < t; i++){
      testcase();
    }
    return 0;
}
