#include "ChainSolver.h"
#include "Anchor.h"
#include "KDpoint.h"
#include "KDtree.h"
#include "KDnode.h"
using namespace std;
#include <algorithm>
#include <iostream>


vector<KDpoint*> ChainSolver::buildkdPoints(const vector<Anchor>& anchors){

    vector<KDpoint*> kdpoints;
    
    for (int i=0;i<anchors.size();i++){

        KDpoint * kd =new KDpoint(
                
                anchors[i].getXend(),
                anchors[i].getYend(),
                i
                
        );
        kdpoints.push_back(kd); //vettore kdpoints per costruzione albero
        
    }

    return kdpoints;
}

vector <KDnode*> ChainSolver::buildkdNodes(const vector<KDpoint*>& kdpoints){
    
    vector<KDnode*> kdnodes;

    for(int i=0;i<kdpoints.size();i++){

        KDnode* node = new KDnode(kdpoints[i]);
        kdnodes.push_back(node);
    }

    return kdnodes;
} 

vector <PointLineSweep> ChainSolver::buildPti(const vector<Anchor>& anchors){

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
    sort(pti.begin(), pti.end(),
    [](const PointLineSweep& a, const PointLineSweep& b){
        if (a.x != b.x) return a.x < b.x;
        return a.isBegin < b.isBegin; 
        // false (end) prima di true (begin)
    });

    return pti;
}

void ChainSolver::printChainRec(Anchor & a, std::vector<Anchor> & anchors) {
    if(a.getPrec() == -1){
        cout << a.getPrec() << " ";  // o niente se vuoi ignorare -1
        return;
    }
    printChainRec(anchors.at(a.getPrec()),anchors); // vai al precedente
    cout << a.getPrec() << " ";
}



void ChainSolver::solve(std::vector<Anchor>& anchors){
    
    std::vector<KDpoint*> kdpoints = buildkdPoints(anchors);

    std::vector<KDnode*> kdnodes  = buildkdNodes(kdpoints);

    KDtree tree(kdnodes);
    
    std::vector <PointLineSweep> pti= buildPti(anchors);
    
    //sweep line
    int n_pti=pti.size();
    
    for(int i=0;i< n_pti; i++){

        int idcurr = pti[i].id;

        if(pti[i].isBegin){
            
            KDpoint* p = tree.rmq(pti[i].x,pti[i].y);


            if(p != nullptr && p->getId() != idcurr) {
                int idPrec = p->getId();

                if (idPrec <0 || idPrec >= anchors.size() || idcurr < 0 || idcurr >= anchors.size()) {
                    cerr << "Error: idPrec or idcurr out of bounds. idPrec: " << idPrec << ", idcurr: " << idcurr << endl;
                    continue; // Skip this iteration to avoid out-of-bounds access
                }

                Anchor &prev = anchors[idPrec];
                anchors[idcurr].setPrec(idPrec);
                
                //gapcost
                int gap = ((anchors[idcurr].getXbegin()-anchors[idPrec].getXend())+(anchors[idcurr].getYbegin()-anchors[idPrec].getYend()));
                anchors[idcurr].setScore(prev.getScore()-gap);
            }
            else {

                anchors[idcurr].setPrec(-1);
                anchors[idcurr].setScore(0);
            }
            
        }
        else if(!pti[i].isBegin){
    
            if (idcurr < 0 || idcurr >= anchors.size()) {
                cerr << "Error: idcurr out of bounds. idcurr: " << idcurr << endl;
                continue; // Skip this iteration to avoid out-of-bounds access
            }
            if(kdpoints[idcurr] && kdnodes[idcurr]) {
                kdpoints[idcurr]->setGc(pti[n_pti-1].x, pti[n_pti-1].y);
                kdpoints[idcurr]->setPriority(anchors[idcurr].getScore());
                kdnodes[idcurr]->activate();
            }
            else {
                cerr << "errore: non esistono kdpoint o kdnodes per l'indice " << idcurr << endl;
            }
            
        }


    }

    cout << "valori precedent:\n";
    for (int i = 0; i < anchors.size(); i++) {
        cout << i << " -> " << anchors[i].getPrec() << endl;
    }
    
    cout << "catena più lunga :\n ";
    printChainRec(anchors.back(), anchors);
    cout << "score : ";
    printf("%d\n",anchors.back().getScore());

    //distruggo kdpoints
    for (auto p : kdpoints)
    delete p;

}