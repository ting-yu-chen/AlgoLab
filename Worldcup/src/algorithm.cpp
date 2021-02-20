#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>



using namespace std;
// choose input type (input coefficients must fit)
typedef int IT;
// choose exact type for solver (CGAL::Gmpz or CGAL::Gmpq)
typedef CGAL::Gmpz ET;
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
// program and solution types
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;
typedef K::Point_2 P; 
void testcase(){
    int n, m , c; 
    cin >> n >> m >> c; 
    // LP:
    // var: number of litter from n to m , n*m var 
    // constr: exact demand (2m)
    // constr: alcol upper limit (m)
    // 2m constr <= 40 
    
    Program lp (CGAL::SMALLER, false, 0, false, 0); 
    vector<int> supply; 
    vector<int> alcohol; 
    vector<P> warehouse; 
    for(int i = 0 ; i < n ; i++){
        // warehouse 
        long x, y; int s, a; 
        cin >> x >> y >> s >> a; 
        warehouse.push_back(P(x,y));
        supply.push_back(s);
        alcohol.push_back(a); 
        for(int j = 0 ; j < m ; j++){
            // var = i*n+j
            lp.set_a(i*n+j, j, 1); 
            lp.set_a(i*n+j, j+m, -1); 
            lp.set_a(i*n+j, j+m*2, a); 
        }
    }
    vector<P> stadium; 
    for(int j = 0 ; j < m ; j++){
        // stadium 
        long x, y; int d, u; 
        cin >> x >> y >> d >> u; 
        stadium.push_back(P(x,y));
        lp.set_b(j, d);
        lp.set_b(j+m, -d);
        lp.set_b(j+m*2, u*100);
    }
    vector<vector<int>> revs(n, vector<int>(m)); 
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < m ; j++){
            int rev; cin >> rev; 
            revs[i][j] = rev;
            /*
            lp.set_c(i*n+j, -rev);
            lp.set_l(i*n+j, true, 0);
            lp.set_a(i*n+j, m*3+i, 1);*/
        }
        //lp.set_b(m*3+i, supply[i]);
    }

    vector<vector<int>> warehouseInOut(n, vector<int>(c, 0));
    vector<vector<int>> stadiumInOut(m, vector<int>(c, 0));;
    for(int i = 0 ; i < c ; i++){
        long x, y; K::FT r; 
        cin >> x >> y >> r; 
        P center(x,y);
        K::FT r2 = r*r; 
        for(int j = 0 ; j < n; j++){
            K::FT dw = CGAL::squared_distance(warehouse[j],center);
            if (dw < r2) warehouseInOut[j][i] = 1; 
            else warehouseInOut[j][i] = -1; 
        }

        for(int j = 0 ; j < m; j++){
            K::FT ds = CGAL::squared_distance(stadium[j],center);
            if (ds < r2) stadiumInOut[j][i] = 1; 
            else stadiumInOut[j][i] = -1; 
        }
    }

    vector<vector<int>> passedContour(n, vector<int>(m, 0));
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < m ; j++){
            for(int k = 0 ; k < c; k++){
                if (warehouseInOut[i][k]*stadiumInOut[j][k]==-1) 
                    passedContour[i][j]++; 
            }
        }
    }

    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < m ; j++){
            lp.set_c(i*n+j, -revs[i][j]*100+passedContour[i][j]);
            lp.set_l(i*n+j, true, 0);
            lp.set_a(i*n+j, m*3+i, 1);
        }
        lp.set_b(m*3+i, supply[i]);
    }


    Solution sol = CGAL::solve_linear_program(lp, ET());
    if (sol.is_infeasible() || sol.is_unbounded())
        cout << "RIOT!\n";
    else cout << fixed << setprecision(0) << floor(CGAL::to_double(-sol.objective_value())/100.0) << "\n";
}
int main(int argc, char const *argv[]) {
    ios_base::sync_with_stdio(false);
    int t; cin >> t; 
    for(int i = 0 ; i < t; i++){
        testcase();
    }
}
