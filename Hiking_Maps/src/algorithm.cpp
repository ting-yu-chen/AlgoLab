#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 P; 

using namespace std;

void testcase(){
    int m, n; cin >> m >> n ; 
    vector<P> path; 
    for(int i = 0 ; i < m ; i++){
        int x, y; 
        cin >> x >> y; 
        path.push_back(P(x,y));
    }
    vector<vector<pair<P, P>>> map_edges(n, vector<pair<P, P>>()); 
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < 3 ; j++){
            int x1, y1, x2, y2; 
            cin >> x1 >> y1 >> x2 >> y2 ; 
            P p1(x1, y1); P p2(x2, y2); 
            map_edges[i].push_back({p1, p2});
        }
        P center = CGAL::centroid(map_edges[i][0].first, map_edges[i][1].first, map_edges[i][2].first); 
        for(int j = 0 ; j < 3 ; j++){
            if (!CGAL::left_turn(map_edges[i][j].first, map_edges[i][j].second, center)){
                swap(map_edges[i][j].first, map_edges[i][j].second);
            }
        }
    }
    // iterate all points in all map mn -> 10^6 -> acceptable 
    vector<vector<bool>> MapIncludesPath(n, vector<bool>(m, false)); 
    for(int i = 0 ; i < m ; i++){
        for(int j = 0 ; j < n ; j++){
            bool inMap = true; 
            for(int k = 0 ; k < 3 ; k++){
                if (CGAL::right_turn(map_edges[j][k].first, map_edges[j][k].second, path[i])){
                    inMap = false; 
                    break; 
                }
            }
            MapIncludesPath[j][i] = inMap; 
        }
    }

    vector<vector<int>> MapIncludesLeg(n, vector<int>());
    
    for(int j = 0 ; j < n ; j++){
        for(int i = 0 ; i < m-1 ; i++){
            if ( MapIncludesPath[j][i] &&  MapIncludesPath[j][i+1]){
                MapIncludesLeg[j].push_back(i);
            }
        }
    }
    
    // sliding window the map 
    int l = 0; int r = 0; 
    vector<int> covered(m-1, 0);
    int uncovered = m-1; 
    int minLen = n; 
    while(l <= r && r <= n-1){
        if (uncovered > 0){
            for(int i = 0 ; i < MapIncludesLeg[r].size(); i++){
                covered[ MapIncludesLeg[r][i]] ++ ; 
                if (covered[ MapIncludesLeg[r][i]] == 1)
                    uncovered -- ; 
            }
            r++ ; 
        } 
        else{
            minLen = min(minLen, r-l);
            for(int i = 0 ; i < MapIncludesLeg[l].size(); i++){
                covered[ MapIncludesLeg[l][i]] -- ; 
                if (covered[ MapIncludesLeg[l][i]] == 0)
                    uncovered ++ ; 
            }
            l++; 
        }
    }
    while (uncovered == 0){
        minLen = min(minLen, r-l);
        for(int i = 0 ; i < MapIncludesLeg[l].size(); i++){
            covered[ MapIncludesLeg[l][i]] -- ; 
            if (covered[ MapIncludesLeg[l][i]] == 0)
                uncovered ++ ; 
        }
        l++; 
    } 
    cout << minLen << "\n";
}
int main(int argc, char const *argv[]) {
    ios_base::sync_with_stdio(false);
    int t; cin >> t; 
    for(int i = 0 ; i < t ; i++){
        testcase();
    }
}

