#define KDTREE_H
#pragma once
#include "KDnode.h"
#include <vector>
#include <fstream>
#include <iostream>


struct Range {
    int xmin,xmax,ymin, ymax;

    Range(int xmi, int xma, int ymi, int yma) : xmin(xmi), xmax(xma), ymin(ymi), ymax(yma) {}
    bool contains(const KDpoint* p) const {
        return p->getY() >= ymin &&
               p->getY() <= ymax &&
               p->getX() >= xmin &&
               p->getX() <= xmax;
    }
};


class KDtree {

private:
    KDnode* root;

    KDnode* buildTree(std::vector<KDnode*>& p, int depth);

    void rmqRec(KDnode* node, Range& R, KDpoint*& best);

    void reportSubtree(KDnode* v, KDpoint*& best);

    void printGraph(KDnode* node, int depth);

    void destroy(KDnode* node);

public:

    KDtree(std::vector<KDnode*> points);
    ~KDtree();

    KDpoint* rmq(int xmax,int ymax);

    void printAlbero();

    KDnode* getRoot();
};