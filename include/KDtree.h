#define KDTREE_H
#pragma once
#include "KDnode.h"
#include <unordered_map>
#include <vector>
#include "Range.h"


class KDtree {

    private : 
    KDnode* root;
    
    KDnode * buildTree(std::vector<KDnode*>&p, int depth);
    void rmqRec(KDnode* node,  Range& R, KDpoint*& best);
    void printGraph(KDnode* node, int depth);

    public :
    KDtree(std::vector<KDnode*> points);
    KDpoint* rmq(int xmax, int ymax);
    void printAlbero();
    KDnode* getRoot();

};