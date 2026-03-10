#define KDTREE_H
#pragma once
#include "KDnode.h"
#include <unordered_map>
#include <vector>



class KDtree {

    private : 
    KDnode* root;
    
    struct Range {
        int xmin, xmax;
        int ymin, ymax;

        Range(int x_min, int x_max, int y_min, int y_max)
            : xmin(x_min), xmax(x_max), ymin(y_min), ymax(y_max) {}

        bool contains(KDpoint* p) const {
            return p->getX() >= xmin && p->getX() <= xmax &&
                   p->getY() >= ymin && p->getY() <= ymax;
        }
    };
    KDnode * buildTree(std::vector<KDnode*>&p, int depth);
    void rmqRec(KDnode* node,  Range& R, KDpoint*& best);
    void printGraph(KDnode* node, int depth);
    void destroy(KDnode* node);

    public :

    
    KDtree(std::vector<KDnode*> points);
    ~KDtree();
    KDpoint* rmq(int xmax, int ymax);
    void printAlbero();
    KDnode* getRoot();

};