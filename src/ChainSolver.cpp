#include "ChainSolver.h"
#include "Anchor.h"
using namespace std;
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>




vector <PointLineSweep> buildPti(const vector<Anchor>& anchors){

    vector<PointLineSweep> pti;
    pti.reserve(2*anchors.size());
    for(int i=0;i<anchors.size();i++){

        //begin
        pti.push_back(

            {anchors[i].getXbegin(),
            anchors[i].getYbegin(),
            true,
            i
        });
        
        //end
        pti.push_back({
            anchors[i].getXend(),
            anchors[i].getYend(),
            false,
            i
        });
    }

    sort(pti.begin()+1, pti.end()-1,
    [](const PointLineSweep& a, const PointLineSweep& b){
        if (a.x != b.x) return a.x < b.x;
        return a.isBegin < b.isBegin; 
        // false (end) prima di true (begin)
    });

    return pti;
}

void printChainRec(int id, const std::vector<Anchor>& anchors) {

    if (id == -1) return;
    if (id == 0) return;  // stoppa alla fittizia iniziale

    const Anchor& a = anchors[id];

    if (a.getPrec() != -1) {
        printChainRec(a.getPrec(), anchors);
    }

    
        std::cout
            << a.getXbegin() << " "
            << a.getXend()   << " "
            << a.getYbegin() << " "
            << a.getYend()   << " "
            << a.getWeight() << "\n";
    
}

std::vector<int> compression(std::vector<Anchor>& anchors){

    std::vector<int> ys;
    for(const auto& a : anchors){
        ys.push_back(a.getYend());
    }
    std::sort(ys.begin(), ys.end());
    ys.erase(std::unique(ys.begin(), ys.end()), ys.end());

    return ys;
}

////////////
std::vector<Anchor> ancoreGrafico(std::vector<Anchor>& anchors){
    
    std::sort(anchors.begin(), anchors.end(),
          [](const Anchor& a, const Anchor& b) {
              return a.getXbegin() < b.getXbegin();
          });

    return std::vector<Anchor>(anchors.begin()+500500, anchors.begin() + 501000);
}
///////////

void solve(std::vector<Anchor>& anchors){
    
    //anchors= ancoreGrafico(anchors);

    auto ys = compression(anchors);
    auto getY = [&](int y){
        return (int)(upper_bound(ys.begin(), ys.end(), y) - ys.begin());
    };

    Fenwick fw(ys.size());  
    #ifndef NDEBUG
    cerr << "Compressione completata. Numero di y unici: " << ys.size() << endl;
    #endif
    
    std::vector <PointLineSweep> pti= buildPti(anchors);
    
    int n_anchors=anchors.size();
    //sweep line
    int n_pti=pti.size();
    
    
    #ifndef NDEBUG
    cerr << "numero ancore:" << n_anchors << endl;
    cerr << "numero pti:" << n_pti << endl;
    cerr << "ys: ";
    for(auto y : ys) cerr << y << " ";
    cerr << endl;
    #endif


    int c=1;
    int idPrec,priority_last=-1;
    auto start = chrono::high_resolution_clock::now();
    for(int i=0;i< n_pti; i++){

        int idcurr = pti[i].id;

        #ifndef NDEBUG
        cerr << "evento i=" << i 
         << " x=" << pti[i].x 
         << " y=" << pti[i].y
         << " isBegin=" << pti[i].isBegin 
         << " id=" << idcurr << endl;

        if(i==n_pti*c/100){
            c++;
            cerr << "Progress: " << (i*100)/n_pti << "%\n";
        }
        #endif
        if(pti[i].isBegin){

            
            FenwickNode res = fw.query(getY(pti[i].y));
            idPrec = res.bestId;

            if(idPrec == -1){
                anchors[idcurr].setPrec(-1);
                anchors[idcurr].setScore(anchors[idcurr].getWeight());
            }
            else if(idPrec == 0){
                anchors[idcurr].setPrec(0);
                anchors[idcurr].setScore(0);
            }
            else if(idcurr == n_anchors-1){
                anchors[idcurr].setPrec(idPrec);
                anchors[idcurr].setScore(anchors[idPrec].getScore());
            }
            else{

                anchors[idcurr].setPrec(idPrec);
                int gap=
                    (anchors[idcurr].getXbegin() - anchors[idPrec].getXend()) + 
                    (anchors[idcurr].getYbegin() - anchors[idPrec].getYend());
                int score=anchors[idPrec].getScore() - gap;
                anchors[idcurr].setScore(score);
            }

        }
        else if(!pti[i].isBegin){
            
            //per l ultima ancora ricalcolo la stessa priority del suo predecessore
            if(idcurr == n_anchors-1){
                cout << "idPrec: " << idPrec << endl;
                int gap=
                    (anchors[n_anchors-1].getXbegin() - anchors[idPrec].getXend()) + 
                    (anchors[n_anchors-1].getYbegin() - anchors[idPrec].getYend());
                priority_last = anchors[idPrec].getScore() - gap;
                cout << "priority_last: " << priority_last << endl;
                fw.update(getY(pti[i].y), priority_last, idcurr);
            }

            int gap=
                    (anchors[n_anchors-1].getXbegin() - anchors[idcurr].getXend()) + 
                    (anchors[n_anchors-1].getYbegin() - anchors[idcurr].getYend());
            int priority = anchors[idcurr].getScore() - gap;
            fw.update(getY(pti[i].y), priority, idcurr);
        }


    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cerr << "Tempo di esecuzione: " << duration << " ms\n";
    

    #ifndef NDEBUG
    for(int i = 0; i < anchors.size(); i++){
    cerr << "ancora " << i 
         << " score=" << anchors[i].getScore()
         << " prec=" << anchors[i].getPrec() << endl;
}
    #endif

    for (int i=0; i < anchors.size(); i++) {
        
        cerr << anchors[i].getXbegin() << " "
             << anchors[i].getYbegin() << " "
             << anchors[i].getXend()   << " "
             << anchors[i].getYend()   << " "
             << anchors[i].getWeight() << "\n"
             ;
}

    int lastId = anchors.size() - 1;
    cout << "x_begin y_begin x_end y_end weight\n";
    printChainRec(lastId, anchors);
    cout << "priority_last: " << priority_last << endl;
    cout << "Score totale: " << anchors[lastId].getScore() << endl;
    cerr << "Catena stampata. Score totale: " << anchors[lastId].getScore() << endl;
    

    

}