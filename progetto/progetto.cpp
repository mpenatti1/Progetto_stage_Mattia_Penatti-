#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;


class Anchor{
    
    private:
    int x_begin,y_begin;
    int x_end,y_end;
    int weight;
    int score;
    int prec;

    public :
        Anchor() : x_begin(0), y_begin(0), x_end(0), y_end(0), weight(0) {}
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
        int getScore() {return score;}
        int getPrec(){return prec;}
        void setPrec(int p){prec=p;}
        void setScore(int s){score=weight+s;}

};

//classe kpoint, non ancora inseriti nell' albero
class KDpoint {

private:
    int x,y;
    int id;
    int gc;
    int priority;

public:

    KDpoint(int xe,int ye,int id)
        : x(xe), y(ye), id(id), 
           gc(0), priority(0) {}

    int getX() const { return x; }
    int getY() const { return y; }

  

    void setGc(int xb, int yb) {
        gc = (xb - x) + (yb - y);
    }

    void setPriority(int score) {
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
    /*uso hashmap per tenere traccia dei kdnode per essere attivati, 
    altrimenti dal ciclo nella sweep line, che funziona sui kdpoints, non posso trovare il kdnode associato*/
    unordered_map<KDpoint*, KDnode*> pointToNode; 
    ///metodo creazione albero

    KDnode * buildTree(vector<KDpoint*>&p, int depth){

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

        vector <KDpoint*> p1(p.begin(),p.begin() + valMedio);
        vector <KDpoint*> p2(p.begin()+valMedio+1,p.end());

        KDnode* node= new KDnode(ptMedio,asse);

        pointToNode[ptMedio] = node;

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

    void activatePoint(KDpoint* p) {
        auto it = pointToNode.find(p);//mi trova la coppia KdPoint e KdNode
        if (it != pointToNode.end()) {
            it->second->activate();     //attivo il kdnode
        }
    }
};


/// un estrapolamento dei punti dalle anchor(p.ti begin e end) da usare nello scorrimento della linee sweep
/// per verificare se trattasi di punti begin o end.
struct pointLineeSweep{

    int x,y;
    bool isBegin;
    int id;

};

void printChainRec(Anchor & a, unordered_map<int, Anchor>& anchors) {
    if(a.getPrec() == -1){
        cout << a.getPrec() << " ";  // o niente se vuoi ignorare -1
        return;
    }
    printChainRec(anchors.at(a.getPrec()), anchors); // vai al precedente
    cout << a.getPrec() << " ";
}

int main() {


    struct InputAnchor {
        int id, xb, yb, xe, ye, w;
    };

    vector<InputAnchor> data = {
        {101, 1,1,4,4,5},
        {205, 2,2,6,5,3},
        {77,  5,3,15,7,4},
        {10, 10, 15, 20, 25,6},
        {13, 25,30, 40, 35,8},
        {29, 13, 18, 17, 19,7},
        {21, 7, 10, 10, 12 ,13}
    };

    unordered_map<int, Anchor> anchors;
    int max_x = 0;
    int max_y = 0;
    for (auto& d : data) {

        anchors[d.id] =
            Anchor(d.xb, d.yb, d.xe, d.ye, d.w);

            max_x = std::max(max_x, d.xe);
            max_y = std::max(max_y, d.ye);
    }

    //ancora begin
      
    anchors[-1] = Anchor(0, 0, 0, 0, 0);

    //ancora end
        
    anchors[-2] = Anchor(max_x + 1, max_y + 1,
                            max_x + 1, max_y + 1,
                            0);

    ///creo KDPoints, i punti con solo gli end.

    unordered_map<int, KDpoint *> IdKdPoints;
    vector<KDpoint*> kdpoints;

    for (auto& [id,a] : anchors){

        KDpoint * kd =new KDpoint(
                a.getXend(),
                a.getYend(),
                id
        );
        kdpoints.push_back(kd); //vettore kdpoints per costruzione albero
        IdKdPoints[id]=kd; //hashmap kdpoints per tracciare id
    }

    ///costruisco kdtree

    KDtree tree(kdpoints);

    tree.printAlbero();

    ///creo vettore di pti begin end ordinati per line sweep

    vector <pointLineeSweep> pti;
    
    
        
    
    for(auto & [id,a] : anchors){

        //begin
        pti.push_back(

            {a.getXbegin(),
            a.getYbegin(),
            true,
            id
        });
        
        //end
        pti.push_back({
            a.getXend(),
            a.getYend(),
            false,
            id
        });
    }

    
    //ordino per le x

    sort(pti.begin(),pti.end(),
        [](const pointLineeSweep& a, const pointLineeSweep& b){
            return a.x<b.x;
        });

    //sweep line
    int n=pti.size();
    
    for(int i=0;i< n; i++){

        int idSelected= pti.at(i).id;
        Anchor& curr = anchors.at(idSelected);

        if(pti.at(i).isBegin){
            
            KDpoint* p = tree.rmq(pti.at(i).x,pti.at(i).y);

            if (p != nullptr) {
                int idPrec = p->getId();
                Anchor& prev = anchors.at(idPrec);

                curr.setPrec(idPrec);
                curr.setScore(prev.getScore());
            }
            else {
                curr.setPrec(-1);
                curr.setScore(0);
            }
            


        }
        else if(!pti.at(i).isBegin){

            KDpoint * kd= IdKdPoints[idSelected];
            kd->setGc(pti[n-1].x, pti[n-1].y);
            kd->setPriority(curr.getScore());
            tree.activatePoint(kd);  
            
        }


    }
    
    printf("%d",anchors.at(-2).getPrec()); 
    printf("\n");
    
    printChainRec(anchors.at(-2), anchors);
    printf("\n");

    // test per la commit
}
