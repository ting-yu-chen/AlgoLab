#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>

using namespace std;

bool validate(int num_box, const vector<int> &strength, const vector<int> &weight){
    int wi = 0 ;
    int si = 0; 
    bool valid = true; 
    int sn = strength.size(); 
    int wn = weight.size(); 
    int curbox = 0; 
    while(wi < wn && si < sn){
        if (curbox < num_box){
            if (strength[si] >= weight[wi]){
                curbox++; 
                wi++; 
            }
            else{
                valid = false;
                break;  
            }
        }
        else{
            si++; 
            curbox = 0; 
        }
    }
    if (wi < wn) valid = false; 
    return valid; 
}

void testcase(){
    int n, m ; cin >> n >> m ; 
    vector<int> strength(n) ;
    vector<int> weight(m);
    for(int i = 0 ; i < n; i++){
        int s; cin >> s; 
        strength[i] = s; 
    }
    for(int i = 0 ; i < m; i++){
        int w; cin >> w; 
        weight[i] = w; 
    }
    sort(strength.begin(), strength.end());
    sort(weight.begin(), weight.end());
    reverse(strength.begin(), strength.end());
    reverse(weight.begin(), weight.end());
    int l = 1;
    int r = m+1; 
    int mid = (l+r)/2; 
    //binary search to find min box; 
    int minbox = m+2; 
    while(l < r){
        bool valid = validate(mid, strength, weight);
        if (valid){
            r = mid-1; 
            minbox = min(mid, minbox);
        }
        else l = mid+1; 
        mid = (l+r)/2; 
    }
    // try samller value 
    int curbox = minbox; 
    while(minbox > 1){
        curbox -- ; 
        bool valid = validate(curbox, strength, weight);
        if (valid) minbox = curbox;
        else break; 
    }
    if (minbox > m+1) cout << "impossible\n";
    else cout << minbox*3-1 << "\n";
}
int main(int argc, char const *argv[]) {
    ios_base::sync_with_stdio(false);
    int t; cin >> t; 
    for(int i = 0 ; i < t ; i++){
        testcase();
    }
}
