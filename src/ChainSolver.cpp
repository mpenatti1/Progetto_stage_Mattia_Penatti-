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

    const Anchor& a = anchors[id];

    // prima vai indietro nella catena
    if (a.getPrec() != -1) {
        printChainRec(a.getPrec(), anchors);
    }

    // poi stampa il nodo corrente
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
    cerr << "Compressione completata. Numero di y unici: " << ys.size() << endl;
    
    std::vector <PointLineSweep> pti= buildPti(anchors);
    
    int n_anchors=anchors.size();
    //sweep line
    int n_pti=pti.size();
    
    
    
    cerr << "numero ancore:" << n_anchors << endl;
    cerr << "numero pti:" << n_pti << endl;
    #
    int c=1;
    cerr << "ys: ";
    for(auto y : ys) cerr << y << " ";
    cerr << endl;
    for(int i=0;i< n_pti; i++){

        int idcurr = pti[i].id;
        cerr << "evento i=" << i 
         << " x=" << pti[i].x 
         << " y=" << pti[i].y
         << " isBegin=" << pti[i].isBegin 
         << " id=" << idcurr << endl;
        if(i==n_pti*c/100){
            c++;
            cerr << "Progress: " << (i*100)/n_pti << "%\n";
        }

        if(pti[i].isBegin){

            
            FenwickNode res = fw.query(getY(pti[i].y));
            int idPrec = res.bestId;
            if(idPrec == -1){
                anchors[idcurr].setPrec(-1);
                anchors[idcurr].setScore(0);
            }
            else if(idPrec == 0){
                anchors[idcurr].setPrec(0);
                anchors[idcurr].setScore(0);
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

            fw.update(getY(pti[i].y), anchors[idcurr].getScore(), idcurr);
        }


    }
    
    int bestId = -1;
    int bestScore = std::numeric_limits<int>::min();
    for(int i = 0; i < anchors.size(); i++){
    cerr << "ancora " << i 
         << " score=" << anchors[i].getScore()
         << " prec=" << anchors[i].getPrec() << endl;
}


    for (int i = 0; i < anchors.size(); i++) {
        if (anchors[i].getScore() > bestScore) {
            bestScore = anchors[i].getScore();
            bestId = i;
        }
    }
    cout << "x_begin y_begin x_end y_end weight\n";
    printChainRec(bestId, anchors);
    //cout << "Score totale: " << anchors.back().getScore() << endl;

    

    

}