#include "KDpoint.h"

    KDpoint::KDpoint(int xe,int ye,int i)
        : x(xe), y(ye), id(i),
           gc(0), priority(0) {}

    int KDpoint::getX() const { return x; }
    int KDpoint::getY() const { return y; }
    int KDpoint::getPriority() const { return priority; }
    int KDpoint::getId() const { return id; }

    void KDpoint::setGc(int xb, int yb) {
        gc = (xb - x) + (yb - y);
    }
    void KDpoint::setPriority(int score) {
        priority = score - gc;
    }

    