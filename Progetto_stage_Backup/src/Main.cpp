#include "Anchor.h"
#include "ChainSolver.h"
#include <vector>
#include "ReadAnchors.h"
using namespace std;

int main(int argc, char* argv[]) {

    vector <Anchor> anchors = readAnchors();
    solve(anchors);
    
}