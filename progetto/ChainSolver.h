#pragma once
#include <vector>
#include "Anchors.h"
#include "KDpoint.h"
#include "PointLineSweep.h"

class ChainSolver{

    public :
    
        void solve(std::vector<Anchor>& anchors);

    private : 
        std::vector<KDpoint*> buildkdPoints(const std::vector<Anchor>& anchors);
        std::vector<PointLineSweep> buildPti(const std::vector<Anchor>& anchors);
        void printChainRec(Anchor & a, std::vector<Anchor> anchors);
};