#include "KDtree.h"
#include "KDnode.h"
#include "KDpoint.h"
#include "Range.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iostream>

KDnode * KDtree::buildTree(std::vector<KDpoint*>& p, int depth) {

    if(p.empty()) return nullptr;
        if(p.size()==1){

            KDnode* node = new KDnode(p[0], depth % 2);
            pointToNode[p[0]] = node;
            return node;
        }
        int asse=depth%2;   

        if(asse%2==0){
            sort(p.begin(),p.end(),
            [](const  KDpoint* a,const KDpoint* b){return a->getX()<b->getX();});
        }
        else{
            
            sort(p.begin(),p.end(),
            [](const  KDpoint* a,const KDpoint* b){return a->getY()<b->getY();});
        }

        int valMedio=p.size()/2;
        KDpoint* ptMedio=p[valMedio];

        std ::vector <KDpoint*> p1(p.begin(),p.begin() + valMedio);
        std::vector <KDpoint*> p2(p.begin()+valMedio+1,p.end());

        KDnode* node= new KDnode(ptMedio,asse);

        pointToNode[ptMedio] = node;

        node->setLeft(buildTree(p1,depth+1));
        node->setRight(buildTree(p2,depth+1));
        
        return node;
}

void KDtree::rmqRec(KDnode* node,  Range& R, KDpoint*& best) {

    if (!node)
        return;

    KDpoint* p = node->getPoint();

    if (node->isActive() && R.contains(p)) {
        
        if (!best || p->getPriority() > best->getPriority()) {
            best = p;
            
        }
    }

    int axis = node->getAsse();

    if (axis == 0) { // split su X
        if (R.xmin <= p->getX())
            rmqRec(node->getLeft(), R, best);
        if (R.xmax >= p->getX())
            rmqRec(node->getRight(), R, best);
    } else { // split su Y
        if (R.ymin <= p->getY())
            rmqRec(node->getLeft(), R, best);
        if (R.ymax >= p->getY())
            rmqRec(node->getRight(), R, best);
    }
}

void KDtree::printGraph(KDnode* node, int depth) {

    if (!node)
        return;

    printGraph(node->getRight(), depth + 1);

    for (int i = 0; i < depth; i++)
        std::cout << "    ";

    KDpoint* p = node->getPoint();
    std::cout << p->getX() << " , " << p->getY();
    std::cout << ((node->getAsse() == 0) ? " (X)" : " (Y)") << "\n";

    printGraph(node->getLeft(), depth + 1);
}

///////////public
KDtree::KDtree(std::vector<KDpoint*> points) {
    root = buildTree(points, 0);
}
KDpoint * KDtree::rmq(int xmax, int ymax) {

    Range R{0, xmax, 0, ymax};

    KDpoint* best = nullptr;

    rmqRec(root, R, best);

    return best;
}

void KDtree::activatePoint(KDpoint* p) {
    auto it = pointToNode.find(p); // mi trova la coppia KdPoint e KdNode
    if (it != pointToNode.end()) {
        it->second->activate(); // attivo il kdnode
    }
}
void KDtree::printAlbero() { printGraph(root, 0); }
KDnode* KDtree::getRoot() { return root; }
