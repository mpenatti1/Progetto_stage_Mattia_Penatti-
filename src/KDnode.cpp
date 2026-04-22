#include "KDnode.h"

KDnode::KDnode(KDpoint* p) : point(p), left(nullptr), right(nullptr), axis(0), active(false) {
    region = { p->getX(), p->getX(), p->getY(), p->getY() };
}

bool KDnode::isActive() const { return active; }
int KDnode::getAsse() const { return axis; }

KDpoint* KDnode::getPoint() const { return point; }
KDnode* KDnode::getLeft() const { return left; }
KDnode* KDnode::getRight() const { return right; }

void KDnode::setAsse(int a) { axis = a; }
void KDnode::activate() { active = true; }

void KDnode::setLeft(KDnode* l) { left = l; }
void KDnode::setRight(KDnode* r) { right = r; }

void KDnode::setRegion(Box b) { region = b; }
Box KDnode::getRegion() const { return region; }

void KDnode::setMaxPrioritySubtree(int p) { maxPrioritySubtree = p; }
int KDnode::getMaxPrioritySubtree() const { return maxPrioritySubtree; }
