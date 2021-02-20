#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>
#include <queue>
using namespace std;

void testcase(){
    int n ; cin >> n ; 
    vector<int> bomb; 
    for(int i = 0 ; i < n ; i++){
        int ti; cin >> ti; 
        bomb.push_back(ti);
    }
    // update bomb time to min(self, parent -1)
    for(int i = 1; i < n ; i+=2){
        int parent = (i-1)/2; 
        bomb[i] = min(bomb[i], bomb[parent]-1);
        bomb[i+1] = min(bomb[i+1], bomb[parent]-1);
    }
    std::priority_queue<int, std::vector<int>, std::greater<int>> pq; 
    for(int i = 0; i < n ; i++){
        pq.push(bomb[i]);
    }
    int time = 1 ; 
    while(!pq.empty()){
        int cur = pq.top(); 
        if (cur >= time){
            pq.pop();
            time++; 
        }
        else break; 
    }
    if (pq.empty()) cout << "yes\n";
    else cout <<"no\n"; 
}
int main(int argc, char const *argv[]) {
    ios_base::sync_with_stdio(false);
    int t; cin >> t; 
    for (int i = 0 ; i < t ; i++){
        testcase();
    }
}
