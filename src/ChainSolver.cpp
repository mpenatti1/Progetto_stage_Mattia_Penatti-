#include "ChainSolver.h"
#include "Anchor.h"
#include "KDpoint.h"
#include "KDtree.h"
#include "KDnode.h"
using namespace std;
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>

#include <unordered_set>

vector<KDpoint*> buildkdPoints(const vector<Anchor>& anchors){

    vector<KDpoint*> kdpoints;
    kdpoints.reserve(anchors.size());

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
    kdnodes.reserve(kdpoints.size());
    for(int i=0;i<kdpoints.size();i++){

        KDnode* node = new KDnode(kdpoints[i]);
        kdnodes.push_back(node);
    }

    return kdnodes;
} 

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

void printChainRec(Anchor & a, std::vector<Anchor> & anchors) {
    if(a.getPrec() == -1) {
        cout << a.getXbegin() << " " << a.getYbegin() << " " << a.getXend() << " " << a.getYend() << " " << a.getWeight() << endl;  // o niente se vuoi ignorare -1
        return;
    }
    printChainRec(anchors.at(a.getPrec()),anchors); // vai al precedente
    cout << a.getXbegin() << " " << a.getYbegin() << " " << a.getXend() << " " << a.getYend() << " " << a.getWeight() << endl;
    
}

/////////////////////////////////////////
void printChaindebug(const vector<Anchor>& anchors) {

    vector<int> chain;
    int curr = anchors.back().getId();

    while (curr != -1) {
        chain.push_back(curr);
        curr = anchors[curr].getPrec();
    }

    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        const auto& a = anchors[*it];
        cout << a.getXbegin() << " "
             << a.getYbegin() << " "
             << a.getXend() << " "
             << a.getYend() << " "
             << a.getWeight() << "\n";
    }
}

//////////////////////////

void solve(std::vector<Anchor>& anchors){
    
    

    std::vector<KDpoint*> kdpoints = buildkdPoints(anchors);

    std::vector<KDnode*> kdnodes  = buildkdNodes(kdpoints);

    KDtree tree(kdnodes);
    
    
    std::vector <PointLineSweep> pti= buildPti(anchors);
    
    int n_anchors=anchors.size();
    //sweep line
    int n_pti=pti.size();
    
    /*
    #ifndef NDEBUG
        for(int i=0;i<kdnodes.size();i++){
            cerr << "KDnode " << i << " -> point id: " << kdnodes[i]->getPoint()->getId() << " with coordinates (" << kdnodes[i]->getPoint()->getX() << ", " << kdnodes[i]->getPoint()->getY() << ")" << endl;
        }

    #endif*/
    /*
    #ifndef NDEBUG
    cerr << "numero ancore:" << n_anchors << endl;
    cerr << "numero pti:" << n_pti << endl;
    #endif*/
    int c=1;
    int idPrec=0;
    bool debug=false;
    int nextPercent=9;
    auto start = std::chrono::high_resolution_clock::now();
    for(int i=0;i< n_pti; i++){

        int idcurr = pti[i].id;
        
        
        if(pti[i].isBegin){
            
            /*#ifndef NDEBUG
            cerr << "\nBEGIN"<< endl;
            cerr << "Processing point: (" << pti[i].x << ", " << pti[i].y << ") - id: " << idcurr << endl;
            #endif*/

            #ifndef NDEBUG
            if (i >= n_pti * nextPercent / 100) {
                cerr << nextPercent << "'%' completato\n";
                debug = true;
                nextPercent += 10;
            }

            else {
                debug=false;
            } 
            #endif
            KDpoint* p = tree.rmq(pti[i].x,pti[i].y,debug);
            if(p != nullptr) {

                idPrec = p->getId();
            
                
                if (idPrec <0 || idPrec >= anchors.size() || idcurr < 0 || idcurr >= anchors.size()) {
                    cerr << "Error: idPrec or idcurr out of bounds. idPrec: " << idPrec << ", idcurr: " << idcurr << endl;
                    continue; // Skip this iteration to avoid out-of-bounds access
                }

                Anchor &prev = anchors[idPrec];
                anchors[idcurr].setPrec(idPrec);
                
                /*#ifndef NDEBUG
                cerr << "Predecessor anchor id: " << idPrec << " with coordinates (" << prev.getXbegin() << ", " << prev.getYbegin() << ", " << prev.getXend() << ", " << prev.getYend() << ") and priority: " << kdpoints[idPrec]->getPriority() << endl;
                #endif*/
                //gapcost

                //caso prima ancora fittizia
                if(idPrec == 0){
                    anchors[idcurr].setScore(0);
                    /*#ifndef NDEBUG
                    cerr << "prima ancora fittizia (come predecessore), non calcolo gap cost" << endl;
                    cerr << "Best point found: (" << p->getX() << ", " << p->getY() << ") - id: " << idPrec << endl;
                    cerr << "peso di "<< anchors[idcurr].getId() << " : " << anchors[idcurr].getWeight() << endl;
                    cerr << "score di "<< anchors[idcurr].getId() << " : " << anchors[idcurr].getScore() << endl;
                    #endif*/
                    continue;
                }
                if(idcurr == n_anchors-1){
                    anchors[idcurr].setScore(prev.getScore());
                    continue;
                }

                int gap = ((anchors[idcurr].getXbegin()-anchors[idPrec].getXend())+(anchors[idcurr].getYbegin()-anchors[idPrec].getYend()));
                
                /*#ifndef NDEBUG
                cerr << "Best point found: (" << p->getX() << ", " << p->getY() << ") - id: " << idPrec << endl;
                cerr << "peso di "<< anchors[idcurr].getId() << " : " << anchors[idcurr].getWeight() << endl;
                cerr << "score predecessore : " << prev.getScore() << endl;
                cerr << "gap cost : " << gap << endl;
                #endif*/

                anchors[idcurr].setScore(prev.getScore()-gap);

                /*#ifndef NDEBUG
                cerr << "Updated anchor score: " << anchors[idcurr].getScore() << endl;
                #endif*/
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
            if(idcurr==n_anchors-1){
                /*#ifndef NDEBUG
                cerr << "idcurr è l'ultima ancora fittizia, assegno stessa priorità della penultima ancora" << endl;
                #endif*/
                kdpoints[idcurr]->setPriority(kdpoints[idPrec]->getPriority());
                cerr << "priority : " << kdpoints[idcurr]->getPriority() << endl;
                continue;
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
            /*#ifndef NDEBUG
            cerr << "\nEND"<< endl;
            cerr << "Processing point: (" << pti[i].x << ", " << pti[i].y << ") - id: " << idcurr << endl;
            cerr << "priority : " << kdpoints[idcurr]->getPriority() << endl;
            
            #endif*/
            #ifdef USE_MAX_PRIORITY
            tree.updateMaxPriority(kdnodes[idcurr]);
            #endif

            
        }


    }
    cerr << "Sweep line completed. Constructing optimal chain...\n";
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cerr << "Tempo di esecuzione: " << duration.count() << " ms" << endl;

    /*#ifndef NDEBUG
    tree.printAlbero();
    cerr << "valori precedent:\n";
    for (int i = 0; i < anchors.size(); i++) {
        cerr << i << " -> " << anchors[i].getPrec() << endl;
    }

    for (int i=0;i < anchors.size();i++){
        cerr << kdpoints[i]->getId() << " -> priority : " << kdpoints[i]->getPriority() << endl;
    }
    #endif
    */


    cout << "Score totale: " << anchors.back().getScore() << endl;
    cerr << "anchors size" << anchors.size() << endl;
    cout << "x_begin y_begin x_end y_end weight\n";
    printChaindebug(anchors);
    cout << "Score totale: " << anchors.back().getScore() << endl;

    //distruggo kdpoints
    for (auto p : kdpoints)
    delete p;

    

}