#pragma once
#include <vector>
#include "Anchor.h"
#include "Fenwick.h"


struct PointLineSweep{

    int x,y;
    bool isBegin;
    int id;

};

    
void solve(std::vector<Anchor>& anchors);

std::vector<int>compression(std::vector<Anchor>& anchors);
std::vector<PointLineSweep> buildPti(const std::vector<Anchor>& anchors);
void printChainRec(Anchor & a, std::vector<Anchor> & anchors);
