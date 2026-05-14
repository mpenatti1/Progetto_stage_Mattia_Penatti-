#include "KDtree.h"
#include "KDnode.h"
#include "KDpoint.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>

//VERSIONE BASE

bool fullyInside(const Box& a, const Range& R) {
    return (a.ymin >= R.ymin && a.ymax <= R.ymax);
}

bool intersects(const Box& a, const Range& R) {
    
    
    bool Ysovrapposto = !(a.ymax < R.ymin || a.ymin > R.ymax);
    
    return Ysovrapposto;
}


KDnode* KDtree::buildTree(std::vector<KDnode*>& p, int left, int right, int depth) {
    
    if (left > right) return nullptr;

    if(left==right){

        KDnode* node=p[left];
        KDpoint* pt = node->getPoint();
        
        Box b;
       
        b.ymin = b.ymax = pt->getY();
        node->setMaxPrioritySubtree(std::numeric_limits<int>::min());
        
        node->setRegion(b);
        return node;
    }

    
    // Ordina per asse
    int mid = left + (right - left)/2;

    std::nth_element(
        p.begin() + left,
        p.begin() + mid,
        p.begin() + right + 1,
        [](KDnode* a, KDnode* b) {
            return a->getPoint()->getY() < b->getPoint()->getY();
        });


    KDnode* node = p[mid];
    KDnode* L = buildTree(p, left, mid - 1, depth + 1);
    KDnode* R = buildTree(p, mid + 1, right, depth + 1);

    node->setLeft(L);
    node->setRight(R);
    if (L) L->setParent(node);
    if (R) R->setParent(node);
    //bounding box
    Box b;
    KDpoint* pt = node->getPoint();

    b.ymin = b.ymax = pt->getY();

    //int best = node->getPoint()->getPriority();

    if (L) {
        Box bl = L->getRegion();
        
        b.ymin = std::min(b.ymin, bl.ymin);
        b.ymax = std::max(b.ymax, bl.ymax);
       
    }

    if (R) {
        Box br = R->getRegion();
        
        b.ymin = std::min(b.ymin, br.ymin);
        b.ymax = std::max(b.ymax, br.ymax);
       
    }

    
    node->setMaxPrioritySubtree(std::numeric_limits<int>::min());
    
    node->setRegion(b);

    return node;
}

///////////////////
    #ifndef NDEBUG
    int KDtree::countSubtree(KDnode* node) {
        if (!node) return 0;
        return 1 + countSubtree(node->getLeft()) + countSubtree(node->getRight());
    }
    int KDtree::countSubtreePriority(KDnode* node) {
        if (!node) return 0;
        return 1 + countSubtreePriority(node->getLeft()) + countSubtreePriority(node->getRight());
    }
    #endif

//////////////////


void KDtree::reportSubtree(KDnode* v, KDpoint*& best) {
    if (!v) return;

    if (best && v->getMaxPrioritySubtree() <= best->getPriority()){
        #ifndef NDEBUG
        pruned_maxpriority+= countSubtreePriority(v);
        #endif
        return;
}
    

    KDpoint* p = v->getPoint();
    
    if (v->isActive() && (!best || p->getPriority() >= best->getPriority()))
        best = p;
    
    #ifndef NDEBUG
    visited_nodes++;
    #endif

    reportSubtree(v->getLeft(), best);
    reportSubtree(v->getRight(), best);
}

//rmq aggiunte bounding box
void KDtree::rmqRec(KDnode* v, Range& R, KDpoint*& best) {

    
    if (!v) return;

    

    //pruning globale
    if (!intersects(v->getRegion(), R)){
        #ifndef NDEBUG
        pruned_nodes+= countSubtree(v);
        #endif
        return;
    }

    if (best && v->getMaxPrioritySubtree() <= best->getPriority()){
        #ifndef NDEBUG
        pruned_maxpriority+= countSubtreePriority(v);
        #endif
        return;
}

    if (fullyInside(v->getRegion(), R)) {
        reportSubtree(v, best);
        return;  // Non serve scendere ulteriormente
    }

    visited_nodes++;
    
    
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
        #ifndef NDEBUG
        else pruned_nodes+= countSubtree(v->getLeft());
        #endif
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
        #ifndef NDEBUG
        else pruned_nodes+= countSubtree(v->getRight());
        #endif
    }
}

KDpoint* KDtree::rmq(int ymax) {

    pruned_maxpriority=0;
    pruned_nodes=0;
    visited_nodes=0;
    Range R{0, ymax};

    KDpoint* best = nullptr;

    rmqRec(root, R, best);

    #ifndef NDEBUG
    std::cerr << "PRUNED MAXPRIORITY: " << pruned_maxpriority << "\n";
    std::cerr << "PRUNED NODES: " << pruned_nodes << "\n";
    std::cerr << "VISITED NODES: " << visited_nodes << "\n";
    #endif
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
    root = buildTree(points, 0, points.size() - 1, 0);
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