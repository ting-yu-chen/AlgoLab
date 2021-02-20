#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>
#include <iostream>
// BGL includes
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
// Graph Type with nested interior edge properties for Cost Flow Algorithms
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
  boost::no_property, boost::property<boost::edge_weight_t, int> >      weighted_graph;
typedef boost::property_map<weighted_graph, boost::edge_weight_t>::type weight_map;
typedef boost::graph_traits<weighted_graph>::edge_descriptor            edge_desc;
typedef boost::graph_traits<weighted_graph>::vertex_descriptor          vertex_desc;

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>> graph;

using namespace std;

class edge_adder {
  graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}

  void add_edge(int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const auto e = boost::add_edge(from, to, G).first;
    const auto rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

bool isMaxflowValid(const vector<tuple<int, int, int>> &edges, int maxlen, const int a, const int s, const int c, const int d){
    graph G(a+s*c+2);
    edge_adder adder(G);
    const int v_source = a+s*c; 
    const int v_target = a+s*c+1; 
    for(int i = 0 ; i < a ; i++)
        adder.add_edge(v_source, i, 1);
    
    for(int i = 0 ; i < s ; i++){
         for(int j = 0 ; j < c; j++){
            adder.add_edge(a+i+j*s, v_target, 1);
         }
    }
        
    for(int i = 0 ; i < edges.size(); i++){
        int len = get<0>(edges[i]);
        int ai = get<1>(edges[i]);
        int si = get<2>(edges[i]);
        for(int j = 0 ; j < c; j++){
            if (len <= maxlen-d*(j+1)){
                adder.add_edge(ai, si+a+s*j, 1);
            }
        }
    }
    int flow = boost::push_relabel_max_flow(G, v_source, v_target);
    if (flow == a) return true; 
    return false; 
}

void testcase(){
    int n, m , a, s, c, d; 
    cin >> n >> m >> a >> s >> c >> d; 
    weighted_graph G(n);
    weight_map weights = boost::get(boost::edge_weight, G);
    edge_desc e;
    for(int i = 0 ; i < m; i++){
        char w; int x, y, z; cin >> w >> x >> y >> z ; 
        if (w == 'S'){
            e = boost::add_edge(x, y, G).first; weights[e]=z; 
        }
        else {
            e = boost::add_edge(x, y, G).first; weights[e]=z; 
            e = boost::add_edge(y, x, G).first; weights[e]=z; 
        }
    }
    vector<int> agents ; 
    for(int i = 0 ; i < a ; i++){
        int ai ; cin >> ai ; 
        agents.push_back(ai);
    }
    vector<int> shelters ; 
    for(int i = 0 ; i < s ; i++){
        int si ; cin >> si ; 
        shelters.push_back(si);
    }
    vector<vector<int>> dist_maps(a, vector<int>(n));
    const int maxINT = numeric_limits<int>::max();
    set<int> minTimes ; 
    vector<tuple<int, int, int>> edges; 
    for(int i = 0 ; i < a ; i++){
        boost::dijkstra_shortest_paths(G, agents[i],
         boost::distance_map(boost::make_iterator_property_map(
            dist_maps[i].begin(), boost::get(boost::vertex_index, G))));
        for(int j = 0 ; j < s ; j++){
            int s_idx = shelters[j];
            if (dist_maps[i][s_idx] != maxINT){
               minTimes.insert(dist_maps[i][s_idx]+c*d);
               edges.push_back({dist_maps[i][s_idx], i, j});
            }
        }
    }
    vector<int> minTime_Vec(minTimes.begin(), minTimes.end());
    sort(minTime_Vec.begin(), minTime_Vec.end());
    // binary search in minTimes 
    long l = d; //int r = minTime_Vec[minTime_Vec.size()-1];
    long r = maxINT; 
    long mid = (l+r)/2; 
    long minlen = maxINT; 
    long bestIdx = r; 
    while(l < r){
        long curlen = mid;
        bool res = isMaxflowValid(edges, curlen, a, s, c, d);
        if (res){
            if (curlen <= minlen){
                minlen = min(minlen, curlen);
                bestIdx = min(bestIdx, mid);
            }
            r = mid; 
        }
        else l = mid+1; 
        mid = (l+r)/2; 
    }

    cout << minlen << "\n";
}
int main(int argc, char const *argv[]) {
    ios_base::sync_with_stdio(false);
    int t; cin >> t; 
    for(int i = 0 ; i < t ; i++)
        testcase();
}
