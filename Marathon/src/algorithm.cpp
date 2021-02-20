#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

// Graph Type with nested interior edge properties for Cost Flow Algorithms
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph; // new! weightmap corresponds to costs

typedef boost::graph_traits<graph>::edge_descriptor             edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator           out_edge_it; // Iterator


typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
  boost::no_property, boost::property<boost::edge_weight_t, int> >      weighted_graph;
typedef boost::property_map<weighted_graph, boost::edge_weight_t>::type weight_map;
typedef boost::graph_traits<weighted_graph>::edge_descriptor            w_edge_desc;
typedef boost::graph_traits<weighted_graph>::vertex_descriptor          vertex_desc;


using namespace std;

class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}
  void add_edge(int from, int to, long capacity, long cost) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    auto w_map = boost::get(boost::edge_weight, G); // new!
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;   // new assign cost
    w_map[rev_e] = -cost;   // new negative cost
  }
};


void testcase(){
    int n,  m, s, f; 
    cin >> n >> m >> s >> f; 
    graph G(n+2);
    weighted_graph w_G(n);
    weight_map weights = boost::get(boost::edge_weight, w_G);

    w_edge_desc e;
    edge_adder adder(G); 
    const int v_source = n;
    const int v_target = n+1; 
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    auto rc_map = boost::get(boost::edge_residual_capacity, G);
    int maxSupply = 0; 
    vector<vector<int>> edge_cap(n, vector<int>(n, 0));
    vector<vector<int>> edge_len(n, vector<int>(n, 0));
    for(int i = 0 ; i < m ; i++){
        int a, b, c, d;
        cin >> a >> b >> c >> d; 
        int i1, i2; 
        if (a < b){
            i1 = a; i2= b; 
        }
        else{
            i1 = b; i2 = a ;
        }
        
        if (edge_cap[i1][i2] == 0){
            edge_cap[i1][i2] = c; 
            edge_len[i1][i2] = d; 
        }
        else if (d < edge_len[i1][i2]){
            edge_cap[i1][i2] = c; 
            edge_len[i1][i2] = d; 
        }
        else if (d == edge_len[i1][i2]){
            edge_cap[i1][i2] = edge_cap[i1][i2]+c; 
        }
    }

    for(int i = 0; i < n ; i++){
        for(int j = i; j < n; j++){
            if (edge_len[i][j] > 0){
                e = boost::add_edge(i, j, w_G).first; 
                weights[e]=edge_len[i][j];
            }
        }
    }
    
    std::vector<int>         dist_map(n);
  std::vector<vertex_desc> pred_map(n);

  boost::dijkstra_shortest_paths(w_G, s,
    boost::distance_map(boost::make_iterator_property_map(
      dist_map.begin(), boost::get(boost::vertex_index, w_G)))
    .predecessor_map(boost::make_iterator_property_map(
      pred_map.begin(), boost::get(boost::vertex_index, w_G))));

    int maxCap = 100000; 
    int cur = f;
    while (s != cur) {
        if (cur < pred_map[cur])
            maxCap = min(maxCap,edge_cap[cur][pred_map[cur]]);
        else 
            maxCap = min(maxCap,edge_cap[pred_map[cur]][cur]);
        cur = pred_map[cur];
        
    }

    int shortest_length = dist_map[f]; 

    for(int i = 0; i < n ; i++){
        for(int j = i; j < n; j++){
            if (edge_len[i][j] > 0){
                if (dist_map[i]+ edge_len[i][j] == dist_map[j]){
                    adder.add_edge(i, j, edge_cap[i][j], edge_len[i][j]);
                }
                if (dist_map[j]+ edge_len[i][j] == dist_map[i]){
                    adder.add_edge(j, i, edge_cap[i][j], edge_len[i][j]);
                }
            }
        }
    }
    adder.add_edge(v_source, s, m*10000, 0);
    adder.add_edge(f, v_target, m*10000, 0);
    
    boost::successive_shortest_path_nonnegative_weights(G, v_source, v_target);
    int cost2 = boost::find_flow_cost(G);
    int flow = 0;
    out_edge_it eit, eend;
    for(boost::tie(eit, eend) = boost::out_edges(boost::vertex(v_source,G), G); eit != eend; ++eit) {
        flow += c_map[*eit] - rc_map[*eit];     
    }
    cout << flow << "\n";

}
int main(int argc, char const *argv[]) {
    ios_base::sync_with_stdio(false);
    int t ; cin >> t ; 
    for(int i = 0 ; i < t ; i++){
        testcase();
    }
}
