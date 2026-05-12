#include "KDtree.h"
#include "KDnode.h"
#include "KDpoint.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>

//VERSIONE NUOVA 

bool fullyInside(const Box& a, const Range& R) {
    return (a.ymin >= R.ymin && a.ymax <= R.ymax && a.xmin >= R.xmin && a.xmax <= R.xmax);
}

bool intersects(const Box& a, const Range& R) {
    
    bool Xsovrapposto = !(a.xmax < R.xmin || a.xmin > R.xmax);
    bool Ysovrapposto = !(a.ymax < R.ymin || a.ymin > R.ymax);
    
    return Ysovrapposto && Xsovrapposto;
}


KDnode* KDtree::buildTree(std::vector<KDnode*>& p, int left, int right, int depth) {
    
    if (left > right) return nullptr;

    if(left==right){

        KDnode* node=p[left];
        KDpoint* pt = node->getPoint();
        int axis = depth % 2;
        node->setAsse(axis);
        Box b;
        b.xmin = b.xmax = pt->getX();
        b.ymin = b.ymax = pt->getY();
        #ifdef USE_MAX_PRIORITY
        node->setMaxPrioritySubtree(nullptr);
        #endif
        node->setRegion(b);
        return node;
    }

    int axis = depth % 2;

    // Ordina per asse
    int mid = left + (right - left)/2;

    
    if (axis == 0) {
        std::nth_element(
            p.begin() + left,
            p.begin() + mid,
            p.begin() + right + 1,
            [](KDnode* a, KDnode* b) {
                return a->getPoint()->getX() < b->getPoint()->getX();
            });
    } 
    else {
        std::nth_element(
            p.begin() + left,
            p.begin() + mid,
            p.begin() + right + 1,
            [](KDnode* a, KDnode* b) {
                return a->getPoint()->getY() < b->getPoint()->getY();
            });
    }

    KDnode* node = p[mid];
    node->setAsse(axis);

    

    KDnode* L = buildTree(p, left, mid - 1, depth + 1);
    KDnode* R = buildTree(p, mid + 1, right, depth + 1);

    node->setLeft(L);
    node->setRight(R);
    if (L) L->setParent(node);
    if (R) R->setParent(node);
    

    //bounding box
    Box b;
    KDpoint* pt = node->getPoint();

    b.xmin = b.xmax = pt->getX();
    b.ymin = b.ymax = pt->getY();

    //int best = node->getPoint()->getPriority();

    if (L) {
        Box bl = L->getRegion();
        b.xmin = std::min(b.xmin, bl.xmin);
        b.xmax = std::max(b.xmax, bl.xmax);
        b.ymin = std::min(b.ymin, bl.ymin);
        b.ymax = std::max(b.ymax, bl.ymax);
        /*#ifdef USE_MAX_PRIORITY
        best = std::max(best, L->getMaxPrioritySubtree());
        #endif*/
    }

    if (R) {
        Box br = R->getRegion();
        b.xmin = std::min(b.xmin, br.xmin);
        b.xmax = std::max(b.xmax, br.xmax);
        b.ymin = std::min(b.ymin, br.ymin);
        b.ymax = std::max(b.ymax, br.ymax);
        /*#ifdef USE_MAX_PRIORITY
        best = std::max(best, R->getMaxPrioritySubtree());
        #endif*/
    }

    #ifdef USE_MAX_PRIORITY
    node->setMaxPrioritySubtree(nullptr);
    #endif
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

    #ifdef USE_MAX_PRIORITY
    if (v->getMaxPrioritySubtree() && best && v->getMaxPrioritySubtreeValue() <= best->getPriority() ){
        #ifndef NDEBUG
        pruned_maxpriority+= countSubtreePriority(v);
        #endif
        return;
}
    #endif

    KDpoint* p = v->getPoint();
    
    /*if (v->isActive() && (!best || p->getPriority() >= best->getPriority()))
        best = p;*/
    
    #ifndef NDEBUG
    visited_nodes++;
    #endif
    if (v->getMaxPrioritySubtree() && 
    (!best || best->getPriority() < v->getMaxPrioritySubtreeValue())) {
        best=v->getMaxPrioritySubtree()->getPoint();
    }
    /*
    else if (v->isActive() && (!best || p->getPriority() >= best->getPriority()))
        best = p;*/

    return;
    /*reportSubtree(v->getLeft(), best);
    reportSubtree(v->getRight(), best); */
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

    #ifdef USE_MAX_PRIORITY
    if (v->getMaxPrioritySubtree() && best && v->getMaxPrioritySubtreeValue() <= best->getPriority() ){
        #ifndef NDEBUG
        pruned_maxpriority+= countSubtreePriority(v);
        #endif
        return;
}
    #endif

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
    
    KDnode* left = v->getLeft();
    KDnode* right = v->getRight();

    int leftPriority = 
        left ? left->getMaxPrioritySubtreeValue() : std::numeric_limits<int>::min();
    int rightPriority = 
        right ? right->getMaxPrioritySubtreeValue() : std::numeric_limits<int>::min();

    if (leftPriority >= rightPriority){

        if (left){

            const Box& b =left->getRegion();
            if(fullyInside(b, R)){
                reportSubtree(left, best);
            }
            else if (intersects(b, R)){
                rmqRec(left, R, best);
            }
            else{
                #ifndef NDEBUG
                pruned_nodes+= countSubtree(left);
                #endif
            }

        }
        if (right){

            const Box& b = right->getRegion();
            if(fullyInside(b, R)){
                reportSubtree(right, best);
            }
            else if (intersects(b, R)){
                rmqRec(right, R, best);
            }
            else{
                #ifndef NDEBUG
                pruned_nodes+= countSubtree(right);
                #endif
            }

        }
    }
    else{
        if (right){

            const Box& b = right->getRegion();
            if(fullyInside(b, R)){
                reportSubtree(right, best);
            }
            else if (intersects(b, R)){
                rmqRec(right, R, best);
            }
            else{
                #ifndef NDEBUG
                pruned_nodes+= countSubtree(right);
                #endif
            }

        }
        if (left){

            const Box& b = left->getRegion();
            if(fullyInside(b, R)){
                reportSubtree(left, best);
            }
            else if (intersects(b, R)){
                rmqRec(left, R, best);
            }
            else{
                #ifndef NDEBUG
                pruned_nodes+= countSubtree(left);
                #endif
            }

        }
    }

}

KDpoint* KDtree::rmq(int xmax,int ymax) {

    pruned_maxpriority=0;
    pruned_nodes=0;
    visited_nodes=0;
    Range R{0,xmax,0, ymax};

    KDpoint* best = nullptr;

    rmqRec(root, R, best);

    #ifndef NDEBUG
    std::cerr << "PRUNED MAXPRIORITY: " << pruned_maxpriority << "\n";
    std::cerr << "PRUNED NODES: " << pruned_nodes << "\n";
    std::cerr << "VISITED NODES: " << visited_nodes << "\n";
    #endif
    return best;
}


#ifdef USE_MAX_PRIORITY
void KDtree::updateMaxPriority(KDnode* node) {
   
    while (node) {

        int maxPriority = std::numeric_limits<int>::min();
        KDnode * best= node;

        if (node->isActive()){
            best= node;
            maxPriority =best->getPoint()->getPriority();
        }
        KDnode* left = node->getLeft();
        if(left && left->getMaxPrioritySubtree() &&left->getMaxPrioritySubtreeValue() > maxPriority){
           
            best = left->getMaxPrioritySubtree();
            maxPriority = left->getMaxPrioritySubtreeValue();
        }

        KDnode* right = node->getRight();
        if(right && right->getMaxPrioritySubtree() && right->getMaxPrioritySubtreeValue() > maxPriority){
            best = right->getMaxPrioritySubtree();
            maxPriority = right->getMaxPrioritySubtreeValue();
        }

        #ifndef NDEBUG
            
                std::cerr << "Updated max priority for node with point (" << node->getPoint()->getX() << "," << node->getPoint()->getY() << ") with priority : " << node->getPoint()->getPriority() << " to: " << maxPriority << std::endl;
            
        #endif

        node->setMaxPrioritySubtree(best);
        node->setMaxPrioritySubtreeValue(maxPriority);
        node = node->getParent();
    }
}
#endif
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