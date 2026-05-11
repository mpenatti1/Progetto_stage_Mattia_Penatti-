#include "ChainSolver.h"
#include "Anchor.h"
using namespace std;
#include <algorithm>
#include <iostream>
#include <fstream>





vector <PointLineSweep> buildPti(const vector<Anchor>& anchors){

    vector<PointLineSweep> pti;

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
            << a.getYbegin() << " "
            << a.getXend()   << " "
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

void solve(std::vector<Anchor>& anchors){
    
    
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
            int idPrec = res.bestId;

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
            
            int gap=
                    (anchors[n_anchors-1].getXbegin() - anchors[idcurr].getXend()) + 
                    (anchors[n_anchors-1].getYbegin() - anchors[idcurr].getYend());
            int priority = anchors[idcurr].getScore() - gap;
            fw.update(getY(pti[i].y), priority, idcurr);
        }


    }
    
    int bestId = -1;
    int bestScore = std::numeric_limits<int>::min();

    #ifndef NDEBUG
    for(int i = 0; i < anchors.size(); i++){
    cerr << "ancora " << i 
         << " score=" << anchors[i].getScore()
         << " prec=" << anchors[i].getPrec() << endl;
}
    #endif

    
    int lastId = anchors.size() - 1;
    cout << "x_begin y_begin x_end y_end weight\n";
    printChainRec(lastId, anchors);
    cout << "Score totale: " << anchors[lastId].getScore() << endl;
    cerr << "Catena stampata. Score totale: " << anchors[lastId].getScore() << endl;
    

    

}