#define KDTREE_H
#include "KDnode.h"
#include <unordered_map>
#include <vector>
#include "Range.h"

class KDtree {

    private : 
    KDnode* root;
    std::unordered_map<KDpoint*, KDnode*> pointToNode; // mappa per trovare il nodo associato a un punto

    KDnode * buildTree(std::vector<KDpoint*>&p, int depth);
    void rmqRec(KDnode* node,  Range& R, KDpoint*& best);
    void printGraph(KDnode* node, int depth);

    public :
    KDtree(std::vector<KDpoint*> points);
    KDpoint* rmq(int xmax, int ymax);
    void activatePoint(KDpoint* p);
    void printAlbero();
    KDnode* getRoot();

};