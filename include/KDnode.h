#pragma once
#define KDNODE_H
#include "KDpoint.h"

struct Box {
    int xmin, xmax, ymin, ymax;
};

class KDnode {

private:

    KDpoint* point;
    KDnode* left;
    KDnode* right;
    int axis;
    bool active;
    Box region;  

    KDnode* parent;
    int maxPrioritySubtree;      
public:

    KDnode(KDpoint* p);

    bool isActive() const;
    int getAsse() const;

    KDpoint* getPoint() const;
    KDnode* getLeft() const;
    KDnode* getRight() const;

    void setAsse(int a);
    void setLeft(KDnode* l);
    void setRight(KDnode* r);

    void setParent(KDnode* p);
    KDnode* getParent() const;
    
    void activate();

    void setRegion(Box b);   
    void setMaxPrioritySubtree(int p);

    Box getRegion() const;  
    int getMaxPrioritySubtree() const;
    ~KDnode() {
}
};
