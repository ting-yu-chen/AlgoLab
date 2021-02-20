#include <CGAL/Exact_predicates_inexact_constructions_kernel.h> 

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm> 
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <tuple>

using namespace std; 

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Line_2 L; 
typedef K::Point_2 P;
typedef K::Segment_2 S;

void testcase(){
  int m, n ; cin >> m >> n ; 
  vector<P> path; 
  for (int i = 0 ; i < m ; i++){
    P p; cin >> p ; path.push_back(p);
  }
  vector<pair<P, P> > e0; 
  vector<pair<P, P> > e1; 
  vector<pair<P, P> > e2; 
  vector<tuple<bool, bool, bool>> signs; // left:1, right == 0 
  for(int i = 0 ; i < n ; i++){
    P p0, p1, p2, p3, p4, p5; 
    cin >> p0 >> p1 >> p2 >> p3 >> p4 >> p5; 
    P center = CGAL::centroid(p0, p2, p4); 
    //P center(xc, yc);
    e0.push_back({p0, p1});
    e1.push_back({p2, p3});
    e2.push_back({p4, p5});
    signs.push_back({CGAL::left_turn(p0, p1, center), 
          CGAL::left_turn(p2, p3, center), CGAL::left_turn(p4, p5, center)
    });
  }
  vector<vector<bool>> PinMap(m, vector<bool>(n, false));
  vector< vector<int> > mapToPath(n, vector<int>()) ; 
  map<int, int> pathIncluded; 
  for(int i = 0 ; i < m ; i++){
    pathIncluded.insert(pair<int, int>(i, 0));
    for(int j = 0 ; j < n ; j++){
      int s0 = 0; int s1 = 0; int s2 = 0; 
      if (get<0>(signs[j])){
        if (CGAL::left_turn(e0[j].first, e0[j].second, path[i])) s0 = 1; 
      }
      else{
        if (CGAL::right_turn(e0[j].first, e0[j].second, path[i])) s0 = 1; 
      }
      if (get<1>(signs[j])){
        if (CGAL::left_turn(e1[j].first, e1[j].second, path[i])) s1 = 1; 
      }
      else{
        if (CGAL::right_turn(e1[j].first, e1[j].second, path[i])) s1 = 1; 
      }
      
      if (get<2>(signs[j])){
        if (CGAL::left_turn(e2[j].first, e2[j].second, path[i])) s2 = 1; 
      }
      else{
        if (CGAL::right_turn(e2[j].first, e2[j].second, path[i])) s2 = 1; 
      }
         
      if (s0+s1+s2 == 3){
        PinMap[i][j] = true; 
        if (i-1 >= 0 && PinMap[i-1][j] == true)
           mapToPath[j].push_back(i-1); 
      }
      /*
      else{
        int col0 = CGAL::collinear(e0[j].first, e0[j].second, path[i]); 
        int col1 = CGAL::collinear(e1[j].first, e1[j].second, path[i]); 
        int col2 = CGAL::collinear(e2[j].first, e2[j].second, path[i]);
        if (col0+col1+col2+s0+s1+s2 == 3){
          PinMap[i][j] = true; 
          if (i-1 >= 0 && PinMap[i-1][j] == true)
             mapToPath[j].push_back(i-1); 
        }
      }*/
      //if(!mapToPath[j].empty())
      //  cout << j << "->" << mapToPath[j][mapToPath[j].size()-1]<<"\n";
    }
  }
  
  int mincost = n ; 
  int b = 0 ; int e = 0 ;
  int unique = 0; 
  while(b <= e && e < n){
    if (unique < m-1){
      //cout << b << " " << e << "not\n";
      for(int i = 0 ; i < (int)mapToPath[e].size(); i++){
        pathIncluded[mapToPath[e][i]]++; 
        if (pathIncluded[mapToPath[e][i]] == 1) unique++; 
      }
      e++;
    }
    else{
      //cout << b << " " << e << "good\n";
      mincost = min(mincost, e-b); 
      for(int i = 0 ; i < (int)mapToPath[b].size(); i++){
        pathIncluded[mapToPath[b][i]]--; 
        if (pathIncluded[mapToPath[b][i]] == 0) unique--; 
      }
      b++; 
    }
  }
  if (e >= n-1){
    while(b <= e && unique == m-1){
      mincost = min(mincost, e-b); 
      //out << b << " " << e <<" " << unique <<"good\n";
      for(int i = 0 ; i < (int)mapToPath[b].size(); i++){
        pathIncluded[mapToPath[b][i]]--; 
        if (pathIncluded[mapToPath[b][i]] == 0) unique--; 
      }
      b++; 
    }
  }
  cout << mincost <<"\n";
  
}
int main(){
    int t;
    cin >> t ;
    for (int i = 0 ; i < t; i++){
      testcase();
    }
    return 0;
}