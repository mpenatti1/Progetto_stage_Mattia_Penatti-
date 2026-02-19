#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
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
            score=0;
            prec=-1;
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

    KDpoint(int xe,int ye,int i)
        : x(xe), y(ye), id(i),
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


//funzione stampa chain

void printChainRec(Anchor & a, vector<Anchor> anchors) {
    if(a.getPrec() == -1){
        cout << a.getPrec() << " ";  // o niente se vuoi ignorare -1
        return;
    }
    printChainRec(anchors.at(a.getPrec()),anchors); // vai al precedente
    cout << a.getPrec() << " ";
}

//funzione lettura file

vector <Anchor> fileReading(const string& filename, int &max_x, int & max_y){

    ifstream fin(filename);
    if (!fin)
    {
        throw runtime_error("errore apertura file");
    }

    vector <Anchor> anchors;
    max_x=0;
    max_y=0;

    int xb,yb,xe,ye,w;

    while(fin >> xb >> yb >> xe >> ye >> w){

        anchors.emplace_back(xb,yb,xe,ye,w);
        if (xe > max_x) max_x = xe;
        if (ye > max_y) max_y = ye;
    }
    //ancora begin
      
    anchors.insert(anchors.begin(), Anchor(0,0,0,0,0));

    //ancora end
        
    anchors.insert(anchors.end(),Anchor(max_x+ 1, max_y + 1,
                            max_x + 1, max_y + 1,
                            0) );
    
    fin.close();

    return anchors;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {


    if (argc < 2){
        cout << "uso : "<< argv[0] << "file.txt\n";
        return 1;
    }

    int max_x,max_y;

    vector <Anchor> anchors = fileReading(argv[1], max_x, max_y);
    
    cout << "\nStampo ancore lette:\n";
    for (int i = 0; i < anchors.size(); i++) {
        cout << i << ": (" 
         << anchors[i].getXbegin() << "," << anchors[i].getYbegin() << ") -> ("
         << anchors[i].getXend() << "," << anchors[i].getYend() << "), weight: "
         << anchors[i].getWeight() << "\n";
}

    ///creo KDPoints, i punti con solo gli end.



    int n=anchors.size();
    vector<KDpoint*> kdpoints;
    
    for (int i=0;i<n;i++){

        KDpoint * kd =new KDpoint(
                
                anchors[i].getXend(),
                anchors[i].getYend(),
                i
                
        );
        kdpoints.push_back(kd); //vettore kdpoints per costruzione albero
        
    }
    cout << "KDPoints (x_end, y_end) prima del buildTree:\n";
    for (int i = 0; i < kdpoints.size(); i++) {
        cout << i << " -> (" << kdpoints[i]->getX() 
         << "," << kdpoints[i]->getY() << ")\n";
}

    ///costruisco kdtree

    KDtree tree(kdpoints);

    tree.printAlbero();

    ///creo vettore di pti begin end ordinati per line sweep

    vector <pointLineeSweep> pti;
    
    
        
    
    for(int i=0;i<n;i++){

        //begin
        pti.push_back(

            {anchors[i].getXbegin(),
            anchors[i].getYbegin(),
            true,
            i
        });
        
        //end
        pti.push_back({
            anchors[i].getXend(),
            anchors[i].getYend(),
            false,
            i
        });
    }

    
    //ordino per le x

    sort(pti.begin(), pti.end(),
    [](const pointLineeSweep& a, const pointLineeSweep& b){
        if (a.x != b.x) return a.x < b.x;
        return a.isBegin < b.isBegin; 
        // false (end) prima di true (begin)
    });

    //sweep line
    int n_pti=pti.size();
    
    for(int i=0;i< n_pti; i++){

        int idcurr = pti[i].id;

        if(pti.at(i).isBegin){
            

            KDpoint* p = tree.rmq(pti.at(i).x,pti.at(i).y);

            if(p != nullptr && p->getId() != idcurr) {
                int idPrec = p->getId();
                Anchor &prev = anchors[idPrec];
                anchors[idcurr].setPrec(idPrec);            //gapcost
                anchors[idcurr].setScore(prev.getScore()); //-((anchors[idcurr].getXbegin()-anchors[idPrec].getXend())+(anchors[idcurr].getYbegin()-anchors[idPrec].getYend())));
            }
            else {
                anchors[idcurr].setPrec(-1);
                anchors[idcurr].setScore(0);
            }
            
        }
        else if(!pti.at(i).isBegin){

            KDpoint * kd= kdpoints[idcurr];
            kd->setGc(pti[n_pti-1].x, pti[n_pti-1].y);
            kd->setPriority(anchors[idcurr].getScore());
            tree.activatePoint(kd);  
            
        }


    }
    
    cout << "valori precedent:\n";
    for (int i = 0; i < anchors.size(); i++) {
        cout << i << " -> " << anchors[i].getPrec() << endl;
    }
    
    cout << "catena più lunga :\n ";
    printChainRec(anchors.back(), anchors);
    printf("\n");
    cout << "score : ";
    printf("%d\n",anchors.back().getScore());


}
