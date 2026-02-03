#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Anchor{
    
    private:
    int x_begin,y_begin;
    int x_end,y_end;
    int weight;

    public :
        Anchor(int xb,int yb,int xe,int ye,int w){
            x_begin=xb;
            x_end=xe;
            y_begin=yb;
            y_end=ye;
            weight=w; 
        }
            
        int getXbegin() { return  x_begin; }
        int getXend() {return x_end;}
        int getYbegin() {return y_begin;}
        int getYend() {return y_end;}
        int getWeight() {return weight;}

};

//classe kpoint, non ancora inseriti nell' albero
class KDpoint {

private:
    int x,y;
    int id;
    int weight;
    int score;
    int gc;
    int priority;

public:

    KDpoint(int xe,int ye,int id,int w)
        : x(xe), y(ye), id(id), weight(w),
          score(0), gc(0), priority(0) {}

    int getX() const { return x; }
    int getY() const { return y; }

    void setScore(int s) {
        score = s + weight;
    }

    void setGc(int xb, int yb) {
        gc = (xb - x) + (yb - y);
    }

    void computePriority() {
        priority = score - gc;
    }

    int getPriority() const {
        return priority;
    }
    int getId() const { return id; }
};

//punti dell albero che hanno come riferimento i kd point
class KDnode {

private:
    KDpoint* point;   // puntatore
    int axis;

    KDnode* left;
    KDnode* right;

    bool active;

public:

    KDnode(KDpoint* p, int a)
        : point(p), axis(a),
          left(nullptr), right(nullptr),
          active(false) {}

    KDpoint* getPoint() { return point; }

    bool isActive() const {
        return active;
    }

    void activate() {
        active = true;
    }
    KDnode* getLeft() { return left; }

    KDnode* getRight() { return right; }

    void setLeft(KDnode* l) { left = l; }
    void setRight(KDnode* r) { right = r; }

    int getAsse() const { return axis; }
};


   
//rettangolo di ricerca 
struct Range {
    int xmin, xmax;
    int ymin, ymax;

    bool contains(const KDpoint* p) const {
        return p->getX() >= xmin && p->getX() <= xmax &&
               p->getY() >= ymin && p->getY() <= ymax;
    }
};


//classe kdtree in cui ci sono metodi di creazione dell albero buildkdtree
//e metodo di ricerca rmqrec
class KDtree{

private: 

    KDnode* root;

    ///metodo creazione albero

    KDnode * buildTree(vector<KDpoint*>&p, int depth){

        if(p.empty()) return nullptr;
        if(p.size()==1)
            return new KDnode(p[0],depth%2);
        
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

        vector <KDpoint*> p1(p.begin(),p.begin() + valMedio);
        vector <KDpoint*> p2(p.begin()+valMedio+1,p.end());

        KDnode* node= new KDnode(ptMedio,asse);

        node->setLeft(buildTree(p1,depth+1));
        node->setRight(buildTree(p2,depth+1));
        
        return node;
    }

    ///////////////////
    /////////////////// ricerca 

    void rmqRec(KDnode* node,
            const Range& R,
            KDpoint*& best) {

        if (!node) return;

        KDpoint* p = node->getPoint();

        if (node->isActive() && R.contains(p)) {

            if (!best ||
                p->getPriority() > best->getPriority()) {
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
    ////////////////////////////////
    //////////////////////////////


    //stampa tree

    void printGraph(KDnode* node, int depth){

    if (!node) return;

    printGraph(node->getRight(), depth+1);

    for (int i = 0; i < depth; i++)
        cout << "    ";
    
    KDpoint * p=node->getPoint();

    cout << p->getX() << " , " << p->getY() ;
    
    if (node->getAsse() == 0)
        cout << " (X)";
    else
        cout << " (Y)";

    cout << "\n";

    printGraph(node->getLeft(),depth+1);
    
}
    
public : 
    KDtree(vector <KDpoint*>& points){

        vector<KDpoint *> p=points;
        root= buildTree(p,0);
    }

    KDpoint* rmq(int xmax, int ymax) {

        Range R{0, xmax, 0, ymax};

        KDpoint* best = nullptr;

        rmqRec(root, R, best);

        return best;
    }

    KDnode* getRoot() { return root;}

    void printAlbero() {printGraph(root, 0);}
};


/// un estrapolamento dei punti dalle anchor(p.ti begin e end) da usare nello scorrimento della linee sweep
/// per verificare se trattasi di punti begin o end.
struct pointLineeSweep{

    int x,y;
    bool isBegin;
    int id;

};

int main() {

    
   
    /*cout << "\n inserisci numero frammenti : ";
    cin >> n;
    for (int i = 0; i < n; i++) {

    int xb, yb, xe, ye, w;

    cout << "\nFrammento " << i + 1 << ":\n";

    cout << "Inserire x begin: ";
    cin >> xb;

    cout << "Inserire y begin: ";
    cin >> yb;

    cout << "Inserire x end: ";
    cin >> xe;

    cout << "Inserire y end: ";
    cin >> ye;

    cout << "Inserire peso: ";
    cin >> w;

    anchors.emplace_back(xb, yb, xe, ye, w);
}*/

    vector<Anchor> anchors = {

    Anchor(1, 1, 4, 4, 5),
    Anchor(2, 2, 6, 5, 3),
    Anchor(5, 3, 8, 7, 4),
    Anchor(7, 1, 9, 4, 6),
    Anchor(5, 6, 6, 8, 10),
    Anchor(1, 3, 3, 5, 5)

};
    int n=anchors.size();



    ///creo KDPoints, i punti con solo gli end.

    vector<KDpoint*> kdpoints;

    for (int i=0;i<n;i++){

        kdpoints.push_back(

            new KDpoint(
                anchors[i].getXend(),
                anchors[i].getYend(),
                i,
                anchors[i].getWeight()
            )
        );
    }

    KDtree tree(kdpoints);

    tree.printAlbero();

}
