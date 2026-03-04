#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
using namespace std;
#include "KDtree.h"
#include "KDnode.h"
#include "KDpoint.h"
#include "Anchors.h"
#include "Range.h"




/// un estrapolamento dei punti dalle anchor(p.ti begin e end) da usare nello scorrimento della linee sweep
/// per verificare se trattasi di punti begin o end.
struct pointLineeSweep{

    int x,y;
    bool isBegin;
    int id;

};


//funzione stampa chain

void printChainRec(Anchor & a, vector<Anchor> anchors) {
    if(a.getPrec() == -1){
        cout << a.getPrec() << " ";  // o niente se vuoi ignorare -1
        return;
    }
    printChainRec(anchors.at(a.getPrec()),anchors); // vai al precedente
    cout << a.getPrec() << " ";
}

//funzione lettura file

vector <Anchor> fileReading(const string& filename, int &max_x, int & max_y){

    ifstream fin(filename);
    if (!fin)
    {
        throw runtime_error("errore apertura file");
    }

    vector <Anchor> anchors;
    max_x=0;
    max_y=0;

    int xb,yb,xe,ye,w;

    while(fin >> xb >> yb >> xe >> ye >> w){

        anchors.emplace_back(xb,yb,xe,ye,w);
        if (xe > max_x) max_x = xe;
        if (ye > max_y) max_y = ye;
    }
    //ancora begin
      
    anchors.insert(anchors.begin(), Anchor(0,0,0,0,0));

    //ancora end
        
    anchors.insert(anchors.end(),Anchor(max_x+ 1, max_y + 1,
                            max_x + 1, max_y + 1,
                            0) );
    
    fin.close();

    return anchors;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {


    if (argc < 2){
        cout << "uso : "<< argv[0] << "file.txt\n";
        return 1;
    }

    int max_x,max_y;

    vector <Anchor> anchors = fileReading(argv[1], max_x, max_y);
    
    cout << "\nStampo ancore lette:\n";
    for (int i = 0; i < anchors.size(); i++) {
        cout << i << ": (" 
         << anchors[i].getXbegin() << "," << anchors[i].getYbegin() << ") -> ("
         << anchors[i].getXend() << "," << anchors[i].getYend() << "), weight: "
         << anchors[i].getWeight() << "\n";
}

    ///creo KDPoints, i punti con solo gli end.



    int n=anchors.size();
    vector<KDpoint*> kdpoints;
    
    for (int i=0;i<n;i++){

        KDpoint * kd =new KDpoint(
                
                anchors[i].getXend(),
                anchors[i].getYend(),
                i
                
        );
        kdpoints.push_back(kd); //vettore kdpoints per costruzione albero
        
    }
    cout << "KDPoints (x_end, y_end) prima del buildTree:\n";
    for (int i = 0; i < kdpoints.size(); i++) {
        cout << i << " -> (" << kdpoints[i]->getX() 
         << "," << kdpoints[i]->getY() << ")\n";
}

    ///costruisco kdtree

    KDtree tree(kdpoints);

    tree.printAlbero();

    ///creo vettore di pti begin end ordinati per line sweep

    vector <pointLineeSweep> pti;
    
    
        
    
    for(int i=0;i<n;i++){

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

    
    //ordino per le x

    sort(pti.begin(), pti.end(),
    [](const pointLineeSweep& a, const pointLineeSweep& b){
        if (a.x != b.x) return a.x < b.x;
        return a.isBegin < b.isBegin; 
        // false (end) prima di true (begin)
    });

    //sweep line
    int n_pti=pti.size();
    
    for(int i=0;i< n_pti; i++){

        int idcurr = pti[i].id;

        if(pti.at(i).isBegin){
            

            KDpoint* p = tree.rmq(pti.at(i).x,pti.at(i).y);
            cout << "punto analizzato :" << pti.at(i).x << "," << pti.at(i).y << "\n";
            cout << "punto rmq: " << p->getX() << "," << p->getY() << "\n";

            if(p != nullptr && p->getId() != idcurr) {
                int idPrec = p->getId();
                cout << " id prec :  " << idPrec << "\n";
                Anchor &prev = anchors[idPrec];
                anchors[idcurr].setPrec(idPrec);            //gapcost
                anchors[idcurr].setScore(prev.getScore()-((anchors[idcurr].getXbegin()-anchors[idPrec].getXend())+(anchors[idcurr].getYbegin()-anchors[idPrec].getYend())));
            }
            else {
                anchors[idcurr].setPrec(-1);
                anchors[idcurr].setScore(0);
            }
            
        }
        else if(!pti.at(i).isBegin){

            KDpoint * kd= kdpoints[idcurr];
            kd->setGc(pti[n_pti-1].x, pti[n_pti-1].y);
            kd->setPriority(anchors[idcurr].getScore());
            tree.activatePoint(kd);  
            
        }


    }
    
    cout << "valori precedent:\n";
    for (int i = 0; i < anchors.size(); i++) {
        cout << i << " -> " << anchors[i].getPrec() << endl;
    }
    
    cout << "catena più lunga :\n ";
    printChainRec(anchors.back(), anchors);
    printf("\n");
    cout << "score : ";
    printf("%d\n",anchors.back().getScore());

}