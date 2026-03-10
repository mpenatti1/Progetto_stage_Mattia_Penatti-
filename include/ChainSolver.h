#pragma once
#include <vector>
#include "Anchors.h"
#include "KDpoint.h"
#include "KDnode.h"
#include "KDtree.h"

struct PointLineSweep{

    int x,y;
    bool isBegin;
    int id;

};

class ChainSolver{

    public :
    
        void solve(std::vector<Anchor>& anchors);

    private : 
        std::vector<KDpoint*> buildkdPoints(const std::vector<Anchor>& anchors);
        std::vector<KDnode*> buildkdNodes(const std::vector<KDpoint*>& kdpoints);
        std::vector<PointLineSweep> buildPti(const std::vector<Anchor>& anchors);
        void printChainRec(Anchor & a, std::vector<Anchor> & anchors);
};