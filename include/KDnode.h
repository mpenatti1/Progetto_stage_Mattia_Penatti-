#ifndef KDNODE_H
#define KDNODE_H
#include "KDpoint.h"

class KDnode{

    private:
    KDpoint* point;
    KDnode* left;
    KDnode* right;
    int axis;
    bool active;


    public : 

        KDnode(KDpoint* p);
        ~KDnode();

        bool isActive() const;
        int getAsse() const;


        KDpoint* getPoint() const;
        KDnode* getLeft() const;
        KDnode* getRight() const;

        void setAsse(int a);
        void setLeft(KDnode* l);
        void setRight(KDnode* r);
        void activate();

};
#endif