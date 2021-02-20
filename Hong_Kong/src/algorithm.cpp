#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>
#include <queue>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
typedef Triangulation::Edge_iterator  Edge_iterator;
typedef Triangulation::Finite_faces_iterator Face_iterator;
typedef Triangulation::Face_handle Face_handle; 
using namespace std;

void testcase(){
    int n, m; double r; 
    cin >> n >> m >> r; 
    // trees 
    vector<K::Point_2> trees;
    for(int i = 0 ; i < n ; i++){
        double x, y ; 
        cin >> x >> y; 
        trees.push_back(K::Point_2(x, y));
    }
    // balloon 
    Triangulation t;
    t.insert(trees.begin(), trees.end());
    std::priority_queue<pair<K::FT, Face_handle>> pq;
    map<Face_handle, K::FT> maxBottleneck; 
    map<Face_handle, bool> visited; 

    for (Face_iterator fi = t.finite_faces_begin(); fi != t.finite_faces_end(); ++fi){
        visited.insert(make_pair(fi, false));
        maxBottleneck.insert(make_pair(fi, -1));
        for(int i = 0 ; i < 3 ; i ++){
            Face_handle fne = fi->neighbor(i);
            if (t.is_infinite(fne)){
                K::FT len = CGAL::squared_distance(fi->vertex((i+1)%3)->point(), fi->vertex((i+2)%3)->point());
                //cout << len << " " << fi->vertex((i+1)%3)->point() << " && " << fi->vertex((i+2)%3)->point() << " && " << fi->vertex((i+3)%3)->point() << " --- \n"; 
                pq.push({len, fi});
            }
        }
    }

    while (!pq.empty()){
        Face_handle curf = pq.top().second;
        K::FT cur_len = pq.top().first;
        pq.pop();
        if (!visited[curf]){
            maxBottleneck[curf] = cur_len; 
            visited[curf] = true; 
            for(int i = 0 ; i < 3 ; i++){
                Face_handle neighbor = curf->neighbor(i); 
                if (!t.is_infinite(neighbor)){
                    K::FT len = CGAL::squared_distance(curf->vertex((i+1)%3)->point(), curf->vertex((i+2)%3)->point());
                    pq.push({min(cur_len, len), neighbor});
                }
            }
        } 
    }

    vector<K::Point_2> balloons;
    vector<long> ballon_radius; 
    for(int i = 0 ; i < m ; i++){
        double x, y; double s ; 
        cin >> x >> y >> s; 
        K::Point_2 b(x, y); 
        K::Point_2 np = (t.nearest_vertex(b))->point();
        K::FT sr = s+r; 
        K::FT mindist = 2*sr; 
        K::FT mindist2 = mindist*mindist; 
        K::FT mindist2_half = sr*sr; 
        if(CGAL::squared_distance(b, np) >= mindist2){
            cout <<"y";
        }
        else if (CGAL::squared_distance(b, np) < mindist2_half) 
            cout << "n";
        else{
            Face_handle f = t.locate(b);
            //cout << mindist2 << " " << maxBottleneck[f] << " -- ";
            if (t.is_infinite(f))
                cout << "y"; 
            else if (maxBottleneck[f] >= mindist2) 
                cout << "y" ;  
            else cout << "n";
        }
    }
    cout << "\n";
    
}
int main(int argc, char const *argv[]) {
    ios_base::sync_with_stdio(false);
    int t ; cin >> t ; 
    for(int i = 0 ; i < t ;i++){
        testcase();
    }
}
