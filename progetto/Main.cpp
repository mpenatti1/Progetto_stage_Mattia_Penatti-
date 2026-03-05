#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std;
#include "KDtree.h"
#include "KDnode.h"
#include "KDpoint.h"
#include "Anchors.h"
#include "Range.h"
#include "ChainSolver.h"
/// un estrapolamento dei punti dalle anchor(p.ti begin e end) da usare nello scorrimento della linee sweep
/// per verificare se trattasi di punti begin o end.
//funzione stampa chain
//funzione lettura file

vector <Anchor> readAnchors(const string& filename, int &max_x, int & max_y){

    ifstream fin(filename);
    if (!fin)
    {
        throw runtime_error("errore apertura file");
    }

    vector <Anchor> anchors;
    max_x=0;
    max_y=0;

    int xb,yb,xe,ye,w;

    //ancora begin
      
    anchors.insert(anchors.begin(), Anchor(0,0,0,0,0));

    while(fin >> xb >> yb >> xe >> ye >> w){

        anchors.emplace_back(xb,yb,xe,ye,w);
        if (xe > max_x) max_x = xe;
        if (ye > max_y) max_y = ye;
    }
    

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

    //lettura file
    if (argc < 2){
        cout << "uso : "<< argv[0] << "file.txt\n";
        return 1;
    }

    //conversione in ancore
    int max_x,max_y;
    vector <Anchor> anchors = readAnchors(argv[1], max_x, max_y);

    /*
    #ifndef NDEBUG
    cout << "\nStampo ancore lette:\n";
    for (int i = 0; i < anchors.size(); i++) {
        cout << i << ": (" 
         << anchors[i].getXbegin() << "," << anchors[i].getYbegin() << ") -> ("
         << anchors[i].getXend() << "," << anchors[i].getYend() << "), weight: "
         << anchors[i].getWeight() << "\n";
    } 
    #endif
    */

    ///creo KDPoints, i punti con solo gli end.
    int n=anchors.size();
    
    /*#ifndef NBEDUG
    cout << "KDPoints (x_end, y_end) prima del buildTree:\n";
    for (int i = 0; i < kdpoints.size(); i++) {
        cout << "id kdpoint :" << kdpoints[i]->getId() << " -> (" << kdpoints[i]->getX() 
         << "," << kdpoints[i]->getY() << ")\n";
    }
    #endif*/

    ChainSolver solver;
    solver.solve(anchors);

    ///costruisco kdtree
    
    //tree.printAlbero();


    ///creo vettore di pti begin end ordinati per line sweep


    

}