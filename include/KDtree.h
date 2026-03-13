#define KDTREE_H
#pragma once
#include "KDnode.h"
#include <vector>
#include <fstream>
#include <iostream>



class KDtree {

    private : 
    KDnode* root;
    
    struct Range {
        int y;

        Range(int ymax) : y(ymax) {}
        bool contains(KDpoint* p) const {
            return p->getY() <= y;
        }
    };

    KDnode * buildTree(std::vector<KDnode*>&p, int depth);
    void rmqRec(KDnode* node,  Range& R, KDpoint*& best);
    void printGraph(KDnode* node, int depth);
    void destroy(KDnode* node);

    public :

    
    KDtree(std::vector<KDnode*> points);
    ~KDtree();
    KDpoint* rmq( int ymax);
    void printAlbero();
    KDnode* getRoot();

};