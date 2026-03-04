#include "KDnode.h"
#include "KDpoint.h"

KDnode::KDnode(KDpoint* p,int a)
    : point(p), axis(a), left(nullptr), right(nullptr), active(false){}
    
KDnode::~KDnode() {
    delete point;
    delete left;
    delete right;
}

KDpoint* KDnode::getPoint() const { return point; }
KDnode* KDnode::getLeft() const { return left; }
KDnode* KDnode::getRight() const { return right; }
int KDnode::getAsse() const { return axis; }
bool KDnode::isActive() const { return active; }

void KDnode::setLeft(KDnode* l) { left = l; }
void KDnode::setRight(KDnode* r) { right = r; }
void KDnode::activate() { active = true; }
