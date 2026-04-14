#define KDTREE_H
#pragma once
#include "KDnode.h"
#include <vector>
#include <fstream>
#include <iostream>


struct Range {
    int ymin, ymax;

    bool contains(const KDpoint* p) const {
        return p->getY() >= ymin &&
               p->getY() <= ymax;
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

    KDpoint* rmq(int ymax);

    void printAlbero();

    KDnode* getRoot();
};