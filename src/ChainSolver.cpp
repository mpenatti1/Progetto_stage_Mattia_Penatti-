#include "ChainSolver.h"
#include "Anchor.h"
#include "KDpoint.h"
#include "KDtree.h"
#include "KDnode.h"
using namespace std;
#include <algorithm>
#include <iostream>
#include <fstream>


vector<KDpoint*> buildkdPoints(const vector<Anchor>& anchors){

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

vector <KDnode*> buildkdNodes(const vector<KDpoint*>& kdpoints){
    
    vector<KDnode*> kdnodes;

    for(int i=0;i<kdpoints.size();i++){

        KDnode* node = new KDnode(kdpoints[i]);
        kdnodes.push_back(node);
    }

    return kdnodes;
} 

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

void printChainRec(Anchor & a, std::vector<Anchor> & anchors) {
    if(a.getPrec() == -1) {
        cout << a.getXbegin() << " " << a.getYbegin() << " " << a.getXend() << " " << a.getYend() << " " << a.getWeight() << endl;  // o niente se vuoi ignorare -1
        return;
    }
    printChainRec(anchors.at(a.getPrec()),anchors); // vai al precedente
    cout << a.getXbegin() << " " << a.getYbegin() << " " << a.getXend() << " " << a.getYend() << " " << a.getWeight() << endl;
    
}



void solve(std::vector<Anchor>& anchors){
    
    

    std::vector<KDpoint*> kdpoints = buildkdPoints(anchors);

    std::vector<KDnode*> kdnodes  = buildkdNodes(kdpoints);

    KDtree tree(kdnodes);
    
    std::vector <PointLineSweep> pti= buildPti(anchors);
    
    int n_anchors=anchors.size();
    //sweep line
    int n_pti=pti.size();
    
    for(int i=0;i< n_pti; i++){

        int idcurr = pti[i].id;

        if(pti[i].isBegin){
            
            #ifndef NDEBUG
            cerr << "\nBEGIN"<< endl;
            cerr << "Processing point: (" << pti[i].x << ", " << pti[i].y << ") - id: " << idcurr << endl;
            #endif

            KDpoint* p = tree.rmq(pti[i].x,pti[i].y ); 

            if(p != nullptr) {

                int idPrec = p->getId();
            
                if (idPrec <0 || idPrec >= anchors.size() || idcurr < 0 || idcurr >= anchors.size()) {
                    cerr << "Error: idPrec or idcurr out of bounds. idPrec: " << idPrec << ", idcurr: " << idcurr << endl;
                    continue; // Skip this iteration to avoid out-of-bounds access
                }

                Anchor &prev = anchors[idPrec];
                anchors[idcurr].setPrec(idPrec);
                
                cerr << "Predecessor anchor id: " << idPrec << " with coordinates (" << prev.getXbegin() << ", " << prev.getYbegin() << ", " << prev.getXend() << ", " << prev.getYend() << ") and score: " << prev.getScore() << endl;

                //gapcost
                if(idPrec == 0){
                    anchors[idcurr].setScore(0);
                    #ifndef NDEBUG
                    cerr << "sono entrato" << endl;
                    cerr << "Best point found: (" << p->getX() << ", " << p->getY() << ") - id: " << idPrec << endl;
                    cerr << "peso di "<< anchors[idcurr].getId() << " : " << anchors[idcurr].getWeight() << endl;
                    cerr << "score di "<< anchors[idcurr].getId() << " : " << anchors[idcurr].getScore() << endl;
                    #endif
                    continue;
                }
                

                int gap = ((anchors[idcurr].getXbegin()-anchors[idPrec].getXend())+(anchors[idcurr].getYbegin()-anchors[idPrec].getYend()));
                
                #ifndef NDEBUG
                cerr << "Best point found: (" << p->getX() << ", " << p->getY() << ") - id: " << idPrec << endl;
                cerr << "peso di "<< anchors[idcurr].getId() << " : " << anchors[idcurr].getWeight() << endl;
                cerr << "score predecessore : " << prev.getScore() << endl;
                cerr << "gap cost : " << gap << endl;
                #endif

                anchors[idcurr].setScore(prev.getScore()-gap);

                #ifndef NDEBUG
                cerr << "Updated anchor score: " << anchors[idcurr].getScore() << endl;
                #endif
            }
            else {
                #ifndef NDEBUG  
                
                cerr << "No valid predecessor found for anchor id: " << idcurr << ". Setting prec to -1 and score to 0." << endl;
                #endif
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
            //////////
            #ifndef NDEBUG
            cerr << "\nEND"<< endl;
            cerr << "Processing point: (" << pti[i].x << ", " << pti[i].y << ") - id: " << idcurr << endl;
            cerr << "priority : " << kdpoints[idcurr]->getPriority() << endl;
            #endif
            
        }


    }

    #ifndef NDEBUG
    tree.printAlbero();
    cerr << "valori precedent:\n";
    for (int i = 0; i < anchors.size(); i++) {
        cerr << i << " -> " << anchors[i].getPrec() << endl;
    }

    for (int i=0;i < anchors.size();i++){
        cerr << kdpoints[i]->getId() << " -> priority : " << kdpoints[i]->getPriority() << endl;
    }
    #endif
    
    cout << "x_begin y_begin x_end y_end weight\n";
    printChainRec(anchors.back(), anchors);
    cout << "Score totale: " << anchors.back().getScore() << endl;

    //distruggo kdpoints
    for (auto p : kdpoints)
    delete p;

    

}