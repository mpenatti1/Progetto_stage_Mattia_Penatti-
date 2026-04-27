#include "KDtree.h"
#include "KDnode.h"
#include "KDpoint.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>

bool fullyInside(const Box& a, const Range& R) {
    return (a.ymin >= R.ymin && a.ymax <= R.ymax && a.xmin >= R.xmin && a.xmax <= R.xmax);
}

bool intersects(const Box& a, const Range& R) {
    
    bool Xsovrapposto = !(a.xmax < R.xmin || a.xmin > R.xmax);
    bool Ysovrapposto = !(a.ymax < R.ymin || a.ymin > R.ymax);
    
    return Ysovrapposto && Xsovrapposto;
}


KDnode* KDtree::buildTree(std::vector<KDnode*>& p, int depth) {

    if (p.empty()) return nullptr;
    if (p.size() == 1) {
    KDnode* node = p[0];

    KDpoint* pt = node->getPoint();
    int axis= depth % 2;
    node->setAsse(axis);
    Box b;
    b.xmin = b.xmax = pt->getX();
    b.ymin = b.ymax = pt->getY();
    node->setMaxPrioritySubtree(pt->getPriority());
    node->setRegion(b); 

    return node;
}
    int axis = depth % 2;

    // Ordina per asse
    int mid = p.size() / 2;

if (axis == 0) {
    std::nth_element(
        p.begin(),
        p.begin() + mid,
        p.end(),
        [](KDnode* a, KDnode* b) {
            return a->getPoint()->getX() < b->getPoint()->getX();
        });
} 
else {
    std::nth_element(
        p.begin(),
        p.begin() + mid,
        p.end(),
        [](KDnode* a, KDnode* b) {
            return a->getPoint()->getY() < b->getPoint()->getY();
        });
}

    KDnode* node = p[mid];
    node->setAsse(axis);

    std::vector<KDnode*> p1(p.begin(), p.begin() + mid);
    std::vector<KDnode*> p2(p.begin() + mid + 1, p.end());

    KDnode* L = buildTree(p1, depth + 1);
    KDnode* R = buildTree(p2, depth + 1);

    node->setLeft(L);
    node->setRight(R);
    if (L) L->setParent(node);
    if (R) R->setParent(node);
    

    //buonding box
    Box b;
    KDpoint* pt = node->getPoint();

    b.xmin = b.xmax = pt->getX();
    b.ymin = b.ymax = pt->getY();

    int best = node->getPoint()->getPriority();

    if (L) {
        Box bl = L->getRegion();
        b.xmin = std::min(b.xmin, bl.xmin);
        b.xmax = std::max(b.xmax, bl.xmax);
        b.ymin = std::min(b.ymin, bl.ymin);
        b.ymax = std::max(b.ymax, bl.ymax);
        best = std::max(best, L->getMaxPrioritySubtree());
    }

    if (R) {
        Box br = R->getRegion();
        b.xmin = std::min(b.xmin, br.xmin);
        b.xmax = std::max(b.xmax, br.xmax);
        b.ymin = std::min(b.ymin, br.ymin);
        b.ymax = std::max(b.ymax, br.ymax);
        best = std::max(best, R->getMaxPrioritySubtree());
    }

    node->setMaxPrioritySubtree(best);
    node->setRegion(b);

    return node;
}


void KDtree::reportSubtree(KDnode* v, KDpoint*& best) {
    if (!v) return;

    /*if (best && v->getMaxPrioritySubtree() <= best->getPriority())
    return;*/

    KDpoint* p = v->getPoint();
    if (v->isActive() && (!best || p->getPriority() >= best->getPriority()))
        best = p;
    
    

    reportSubtree(v->getLeft(), best);
    reportSubtree(v->getRight(), best);
}

//rmq aggiunte bounding box
void KDtree::rmqRec(KDnode* v, Range& R, KDpoint*& best) {

    
    if (!v) return;

    if (fullyInside(v->getRegion(), R)) {
        reportSubtree(v, best);
        return;  // Non serve scendere ulteriormente
    }

    //pruning globale
    if (!intersects(v->getRegion(), R))
        return;

    if (best && v->getMaxPrioritySubtree() <= best->getPriority())
    return;

    KDpoint* p = v->getPoint();
    if ((v->isActive() && p->getY() <= R.ymax)) {
        if (!best || p->getPriority() >= best->getPriority())
            best = p;
    }

    //figlio sinistro
    if (v->getLeft()) {
        Box L = v->getLeft()->getRegion();

        if (fullyInside(L, R)) {
            reportSubtree(v->getLeft(), best);
        }
        else if (intersects(L, R)) {
            rmqRec(v->getLeft(), R, best);
        }
    }

    //figlo destro
    if (v->getRight()) {
        Box Rb = v->getRight()->getRegion();

        if (fullyInside(Rb, R)) {
            reportSubtree(v->getRight(), best);
        }
        else if (intersects(Rb, R)) {
            rmqRec(v->getRight(), R, best);
        }
    }
}

KDpoint* KDtree::rmq(int xmax,int ymax) {

    Range R{0,xmax,0, ymax};

    KDpoint* best = nullptr;

    rmqRec(root, R, best);

    return best;
}


void KDtree::updateMaxPriority(KDnode* node) {
   
    while (node) {

    int maxPriority = std::numeric_limits<int>::min();

    if (node->isActive())
        maxPriority = node->getPoint()->getPriority();

    if (node->getLeft())
        maxPriority = std::max(maxPriority, node->getLeft()->getMaxPrioritySubtree());

    if (node->getRight())
        maxPriority = std::max(maxPriority, node->getRight()->getMaxPrioritySubtree());

    #ifndef NDEBUG

        if (node->isActive()) {
            std::cerr << "Updated max priority for node with point (" << node->getPoint()->getX() << "," << node->getPoint()->getY() << ") with priority : " << node->getPoint()->getPriority() << " to: " << maxPriority << std::endl;
        }
        #endif
    node->setMaxPrioritySubtree(maxPriority);

    node = node->getParent();
    }
}

void KDtree::printGraph(KDnode* node, int depth) {

    if (!node)
        return;

    printGraph(node->getRight(), depth + 1);

    for (int i = 0; i < depth; i++)
        std::cerr << "    ";

    KDpoint* p = node->getPoint();
    std::cerr << p->getX() << " , " << p->getY();
    std::cerr << ((node->getAsse() == 0) ? " (X)" : " (Y)") << "\n";

    printGraph(node->getLeft(), depth + 1);
}

void KDtree::printAlbero() { printGraph(root, 0); }


KDtree::KDtree(std::vector<KDnode*> points) {
    root = buildTree(points, 0);
}

void KDtree::destroy(KDnode* node) {
    if (!node) return;

    destroy(node->getLeft());
    destroy(node->getRight());

    delete node;       
}
KDtree::~KDtree() {
    destroy(root);   
}

KDnode* KDtree::getRoot() { return root; }