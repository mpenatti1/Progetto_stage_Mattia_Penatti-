#include "Anchors.h"
#include "ChainSolver.h"
#include <vector>
#include "ReadAnchors.h"
using namespace std;
/// un estrapolamento dei punti dalle anchor(p.ti begin e end) da usare nello scorrimento della linee sweep
/// per verificare se trattasi di punti begin o end.
//funzione stampa chain
//funzione lettura file


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

    vector <Anchor> anchors = readAnchors();

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
   
    ChainSolver solver;
    solver.solve(anchors);
    
}